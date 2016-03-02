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

//---- Public ---------------------------------------------------------------//

VulkanBase::VulkanBase(const char* appName, const char* engineName , 
    const std::vector<const char*>& extensions)
:   PhysicalDevices(0), PhysicalDevicesMemProps(0) { 
   createInstance(appName, engineName, extensions);
   getPhysicalDevices();
   getPhysicalDevicesMemoryProperties();
}

bool VulkanBase::findQueue(uint32_t deviceIdx, VkQueueFlagBits queueFlags,
    uint32_t* queueIdx) {
  uint32_t queueCount, queueId = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[deviceIdx], 
    &queueCount, nullptr);

  if (queueCount < 1)       // No queues for physical device.
    return false;

  VwQueueFamPropVec queueProperties;
  queueProperties.resize(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[deviceIdx], 
    &queueCount, queueProperties.data());

  // Check if the requested queue can be found.
  for (; queueId < queueCount; ++queueId) {
    if (queueProperties[queueId].queueFlags & queueFlags)
      break;
  }
 
  if (queueId < queueCount) { 
    *queueIdx = queueId;    
    return true;
  }  else { 
    return false;
  }
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
  instanceInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext            = nullptr;
  instanceInfo.pApplicationInfo = &appInfo;

  if (enabledExtensions.size() > 0) {
    instanceInfo.enabledExtensionCount   = 
      static_cast<uint32_t>(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();
  }
  return vkCreateInstance(&instanceInfo, nullptr, &Instance); 
}
 
void VulkanBase::getPhysicalDevices() {
  assert(Instance != nullptr && "VkInstance not initializes so physical "
         && "devices cannot be found\n");

  uint32_t deviceCount =0;
  VkResult result      = vkEnumeratePhysicalDevices(Instance, &deviceCount,
                           nullptr);
  assert(!result && "Failed to enumerate physical devices : Fatal Error\n");
  assert(deviceCount >= 1 && "Failed to find any physical devices : "
         && "Fatal Error\n");

  PhysicalDevices.resize(deviceCount); 
  result = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
             PhysicalDevices.data());
  assert(!result && "Could not enumerate physical devices : Fatal Error\n");
}

void VulkanBase::getPhysicalDevicesMemoryProperties() {
  PhysicalDevicesMemProps.resize(PhysicalDevices.size());
  for (size_t i = 0; i < PhysicalDevices.size(); ++i) {
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevices[i], 
      &PhysicalDevicesMemProps[i]); 
  }
}
