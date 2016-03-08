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

/// Checks if a vkPhysicalDevice fits one of the DeviceSpecifier type
/// specificationas. If a match is found then the index of the match is 
/// returned, otherwise a one past the end index is returned (which will be
/// equal to the number of DeviceSpecifiers provided).
///
/// \param vkPhysicalDevice The vulkan physical device to check.
/// \param deviceSpecifiers The specifiers to try and find a match with.
size_t findDeviceTypeIndex(const VkPhysicalDevice&   vkPhysicalDevice, 
                           const DeviceSpecifierVec& deviceSpecifiers) {
  size_t                     specifierIndex           = 0;
  VkPhysicalDeviceProperties physicalDeviceProperties = {};

  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);

  for (const auto& deviceSpecifier : deviceSpecifiers) {
    if (static_cast<uint8_t>(physicalDeviceProperties.deviceType) ==
        static_cast<uint8_t>(deviceSpecifier.deviceType)          ) {
      break;
    }
    if (deviceSpecifier.deviceType == DeviceType::VW_ANY)
      break;

    ++specifierIndex;
  }
  // We return the inverse because we want to return true for all devices
  // which are 
  return specifierIndex;
}

} // annonymous namespace
 
//---- Public ---------------------------------------------------------------//

DeviceSelector::DeviceSelector(const DeviceSpecifierVec& deviceSpecifiers, 
    bool devicesMustSupportAllQueues, const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions)
:   PhysicalDevices(0) { 
  createInstance(appName, engineName, extensions);

  // Filter out all the devices which do not match the physical 
  // device types and the queue types for those devices.
  filterPhysicalDevices(
    std::forward<const DeviceSpecifierVec&&>(deviceSpecifiers), 
    devicesMustSupportAllQueues                               
  );
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
 
void DeviceSelector::filterPhysicalDevices(
    const DeviceSpecifierVec& deviceSpecifiers, 
    bool devicesMustSupportAllQueues          ) {
  vs::util::Assert(Instance != nullptr , "Vulkan instance not initialized.\n");

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

  // Go through all the physical devices and check if they are
  // one of the requested types and have the correct queues.
  size_t deviceTypeId = 0;
  for (const auto& vkPhysicalDevice : physicalDevices) {
    deviceTypeId = findDeviceTypeIndex(vkPhysicalDevice, deviceSpecifiers);
    if (deviceTypeId == deviceSpecifiers.size())
      continue;   // Not a supported device -- next iteration.
    
    // The device has a correct type, so create a VulkaWrap PhysicalDevice 
    // and add the requested queues which the device supports.
    PhysicalDevices.emplace_back(vkPhysicalDevice);
    PhysicalDevices.back().addSupportedQueues(
      deviceSpecifiers[deviceTypeId].queueTypes
    );

    // Remove the device if it must support all queues
    // and not all the requested queus were found.
    if (devicesMustSupportAllQueues && 
        (PhysicalDevices.back().queueTypes.size() != 
         deviceSpecifiers[deviceTypeId].queueTypes.size())) {
      PhysicalDevices.pop_back();
    }
  }

  vs::util::Assert(PhysicalDevices.size() >= 1, "Failed to find a physical " +
    std::string("device which meets the requested specifiers.\n"));
}

}  // namespace vwrap
}  // namespace vs
