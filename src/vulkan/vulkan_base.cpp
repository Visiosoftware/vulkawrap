//---- src/vulkan/vulkan_base.cpp - Implementation of minimal vulkan --------//
//---- functionality.                                                --------//
//
//                        Vulkan Examples/Tutorials
//                          
//                        Copyright (c) 2016 Rob Clucas        
//                      Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
// \file   vulkan_base.cpp
// \brief  Implementation of a vulkna base class.
//
//---------------------------------------------------------------------------//

#include "vulkan_base.h"
#include <cassert>

namespace {

// Checks if a vkPhysicalDevice fits one of the specifications. If a match is
// found then the index of the match is returned, otherwise a one past the end
// index is returned (the number of specifiers)
//
// \param vkPhysicalDevice    The vulkan physical device to check.
// \param vwDeviceSpecifiers  The specifiers to find a match with.
size_t findDeviceTypeIndex(const VkPhysicalDevice& vkPhysicalDevice, 
                           const VwDeviceSpecVec& vwDeviceSpecifiers) {
  VkResult                   result;
  size_t                     specifierIndex           = 0;
  VkPhysicalDeviceProperties physicalDeviceProperties = {};

  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);
  assert(!result && "Failed to get properties of physical device : "
         && "Fatal Error\n");

  for (const auto& deviceSpecifier : vwDeviceSpecifiers) {
    if (static_cast<uint8_t>(physicalDeviceProperties.deviceType) ==
        static_cast<uint8_t>(deviceSpecifier.deviceType)          ) {
      break;
    }
    if (deviceSpecifier.deviceType == VwDeviceType::VW_ANY)
      break;

    ++specifierIndex;
  }
  // We return the inverse because we want to return true for all devices
  // which are 
  return specifierIndex;
}

}  // annonymous namespace
 
//---- Public ---------------------------------------------------------------//

VulkanBase::VulkanBase(const VwDeviceSpecVec& deviceSpecifiers, 
    bool devicesMustSupportAllQueues, const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions)
:   PhysicalDevices(0) { 
  createInstance(appName, engineName, extensions);

  // Filter out all the devices which do not match the physical 
  // device types and the queue types for those devices .
  getPhysicalDevices(std::forward<const VwDeviceSpecVec&&>(deviceSpecifiers), 
                     devicesMustSupportAllQueues);
}

//---- Private --------------------------------------------------------------//

VkResult VulkanBase::createInstance(const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions) {
  // Set the application properties
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = appName;
  appInfo.pEngineName       = engineName;

  // Might need workaround here for invalid vulkan version
  appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 2);
   
  // Create a vector with the default extensions 
  // and then add the user defined extensions
  std::vector<const char*> enabledExtensions = 
    {VK_KHR_SURFACE_EXTENSION_NAME}; 
   
  for (const auto& extension : extensions) 
    enabledExtensions.push_back(extension);

  // Create and set the properties of a vulkan instance
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
 
void VulkanBase::getPhysicalDevices(const VwDeviceSpecVec& deviceSpecifiers, 
    bool devicesMustSupportAllQueues) {
  assert(Instance != nullptr && "VkInstance not initializes so physical "
         && "devices cannot be found\n");

  uint32_t deviceCount = 0;
  VkResult result      = vkEnumeratePhysicalDevices(Instance, &deviceCount,
                           nullptr);
  assert(!result && "Failed to enumerate physical devices : Fatal Error\n");
  assert(deviceCount >= 1 && "Failed to find any physical devices : "
         && "Fatal Error\n");

  std::vector<VkPhysicalDevice> physicalDevices;
  physicalDevices.resize(deviceCount); 
  result = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
             physicalDevices.data());
  assert(!result && "Could not enumerate physical devices : Fatal Error\n");

  // Go through all the physical devices and check if they are
  // one of the requested types and have the correct queues.
  size_t deviceTypeId = 0;
  for (const auto& vkPhysicalDevice : physicalDevices) {
    deviceTypeId = findDeviceTypeIndex(vkPhysicalDevice, deviceSpecifiers);
    if (deviceTypeId == deviceSpecifiers.size())
      continue;   // Not a supported device -- next iteration.
    
    // The device has a correct type, so create a VwPhysicalDevice 
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

  assert(PhysicalDevices.size() >= 1 && "Failed to find a physical device "
         && "which meets the reequested specifiers : Fatal Error\n");
}
