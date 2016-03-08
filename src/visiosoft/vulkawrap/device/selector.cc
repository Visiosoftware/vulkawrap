//---- src/visiosoft/vulkawrap/device/selector.cpp --------- -*- C++ -*- ----//
//---- Implementation of the device selector functionality for VulkaWrap.----//
//
//                           Visiosoft VulkaWrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   selector.cc
/// \brief  Implementation of the device selection functionality for VulkaWrap,
///         providing a clean interface for choosing Vulkan devices based on 
///         their physical types and the Vulkan queues.
//
//---------------------------------------------------------------------------//

#include "visiosoft/vulkawrap/device/selector.h"
#include "visiosoft/vulkawrap/util/error.hpp"

namespace vs    {
namespace vwrap {
namespace       {

/// Checks if a Vulkan Physical Device matches a device specifier types. 
/// Returns true if the physical device meets the type requirements of the 
/// specifier.
///
/// \param vkPhysicalDevice The vulkan physical device to check.
/// \param deviceSpecifier  The specifier to try and find a match with.
bool physicalDeviceTypeIsCorrect(const VkPhysicalDevice& vkPhysicalDevice, 
                                 const DeviceSpecifier&  deviceSpecifier) {
  VkPhysicalDeviceProperties physicalDeviceProperties = {};
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);

  if (static_cast<uint8_t>(physicalDeviceProperties.deviceType) ==
      static_cast<uint8_t>(deviceSpecifier.deviceType)          ) {
    return true;
  }
  if (deviceSpecifier.deviceType == DeviceType::VW_ANY)
    return true;

  return false;
}

} // annonymous namespace
 
//---- Public ---------------------------------------------------------------//

DeviceSelector::DeviceSelector(DeviceSpecifier& deviceSpecifier, 
    bool deviceMustSupportAllQueues, const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions)
:   PhysicalDevices(0) { 
  createInstance(appName, engineName, extensions);
  vs::util::Assert(Instance != nullptr , "Vulkan instance not initialized.\n");

  auto physicalDevices = getPhysicalDevices();

  // Go through the physical devices and add those which match the specifier
  for (auto& physicalDevice : physicalDevices) {
    if (!physicalDeviceTypeIsCorrect(physicalDevice, deviceSpecifier))
      continue;  // Go to next iteration if the device type is incorrect.

    if (addIfQueuesAreSupported(physicalDevice, deviceSpecifier.queueTypes, 
          deviceMustSupportAllQueues) == true) {
      deviceSpecifier.valid = true;
    } else {
      deviceSpecifier.valid = false;
    }
  }
}


DeviceSelector::DeviceSelector(DeviceSpecifierVec& deviceSpecifiers, 
    bool devicesMustSupportAllQueues, const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions)
:   PhysicalDevices(0) { 
  createInstance(appName, engineName, extensions);
  vs::util::Assert(Instance != nullptr , "Vulkan instance not initialized.\n");

  auto physicalDevices = getPhysicalDevices();

  for (auto& physicalDevice : physicalDevices) {
    for (auto& deviceSpecifier : deviceSpecifiers) {
      if (!physicalDeviceTypeIsCorrect(physicalDevice, deviceSpecifier))
        continue;  // Go to next iteration if the device type is incorrect.

      if (addIfQueuesAreSupported(physicalDevice, deviceSpecifier.queueTypes, 
            devicesMustSupportAllQueues)) {
        deviceSpecifier.valid = true;
      } else {
        deviceSpecifier.valid = false;
      }
    }
  }
}

bool DeviceSelector::addIfQueuesAreSupported(
    const VkPhysicalDevice& vkPhysicalDevice, 
    const QueueTypeVec& queueTypes          , 
    bool  mustSupportAllQueues              ) {
  PhysicalDevices.emplace_back(vkPhysicalDevice);
  PhysicalDevices.back().addSupportedQueues(queueTypes);

  // Remove the device if it must support all queues
  // and not all the requested queus were found.
  if (mustSupportAllQueues && 
      (PhysicalDevices.back().queueTypes.size() != queueTypes.size())) {
    PhysicalDevices.pop_back();
    return false;
  } 
  return true;
}

//---- Private --------------------------------------------------------------//

VkResult DeviceSelector::createInstance(const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions) {
  // Set the application properties.
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = appName;
  appInfo.pEngineName       = engineName;

  // Currently need workaround here for invalid vulkan version.
  // TODO: Change when error is resolved.
  appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 2);
   
  // Create a vector with the default extensions 
  // and then add the user defined extensions.
  std::vector<const char*> enabledExtensions = 
    { VK_KHR_SURFACE_EXTENSION_NAME }; 
   
  for (const auto& extension : extensions) 
    enabledExtensions.push_back(extension);

  // Create and set the properties for a vulkan instance.
  VkInstanceCreateInfo instanceInfo = {};
  instanceInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext                = nullptr;
  instanceInfo.pApplicationInfo     = &appInfo;

  if (enabledExtensions.size() > 0) {
    instanceInfo.enabledExtensionCount   = 
      static_cast<uint32_t>(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();
  }
  return vkCreateInstance(&instanceInfo, nullptr, &Instance); 
}

std::vector<VkPhysicalDevice> DeviceSelector::getPhysicalDevices() const {
  uint32_t deviceCount = 0;
  VkResult result      = vkEnumeratePhysicalDevices(Instance, &deviceCount,
                           nullptr);
  util::AssertSuccess(result, "Failed to enumerate physical devices.\n");
  vs::util::Assert(deviceCount >= 1, "Failed to find any physical devices.\n");

  std::vector<VkPhysicalDevice> physicalDevices;
  physicalDevices.resize(deviceCount); 
  result = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
             physicalDevices.data());
  util::AssertSuccess(result, "Could not enumerate physical devices.\n");
  return physicalDevices;
}

}  // namespace vwrap
}  // namespace vs
