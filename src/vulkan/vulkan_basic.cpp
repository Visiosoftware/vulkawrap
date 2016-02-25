//---- src/vulkan/vulkan_basic.cpp - Implementation for vulkan basic-- ------//
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
// \file   vulkan_basic.cpp
// \brief  Implementation of the VulkanBasic class.
//---------------------------------------------------------------------------//

#include "vulkan_basic.h"
#include <cassert>

//---- Public ---------------------------------------------------------------//

// TODO : Update this so that you can set the name and add windowing extensions
//      : Add in non fatal error handling
VulkanBasic::VulkanBasic()
: GraphicsQueueId(0) {
  VkResult error = createInstance();
  assert(!error && "Could not create Vulkan instance : Fatal Error\n");

  setPhysicalDevice();    // Terminates if no device is found
  findGraphicsQueue();    // Terminates of no gfx queue is found

  // Create the properties for the logical devices
  const float queuePriorities       = 0.0f;
  VkDeviceQueueCreateInfo queueInfo = {};
  queueInfo.sType               =  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.queueFamilyIndex        = GraphicsQueueId;
  queueInfo.pQueuePriorities        = &queuePriorities;

  error = createDevice(queueInfo);
  assert(!error && "Failed to create device : Fatal Error\n");
 
  // Get the device memory properties
  vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &DeviceMemProps);

  // Get the graphics queue
  vkGetDeviceQueue(Device, GraphicsQueueId, 0, &Queue);

  // TODO: Depth format and swapchain

}

VkResult VulkanBasic::createInstance(const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions) {
  // Set the application properties
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = appName;
  appInfo.pEngineName       = engineName;

  // Might need workaround here for invalid vulkan version
 
  // Create a vector with the default extensions 
  // and then add the user defined extensions
  std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME }; 
  for (const auto& ext : extensions) 
    enabledExtensions.push_back(ext);

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

VkResult VulkanBasic::createDevice(VkDeviceQueueCreateInfo requestedQueues) {
  std::vector<const char*> enabledExtensions = 
    { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  VkDeviceCreateInfo deviceInfo   = {};
  deviceInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.pNext                = nullptr;
  deviceInfo.queueCreateInfoCount = 1;
  deviceInfo.pQueueCreateInfos    = &requestedQueues;
  deviceInfo.pEnabledFeatures     = nullptr;

  if (enabledExtensions.size() > 0) {
    deviceInfo.enabledExtensionCount =
      static_cast<uint32_t>(enabledExtensions.size());
    deviceInfo.ppEnabledLayerNames = enabledExtensions.data();
  }
  return vkCreateDevice(PhysicalDevice, &deviceInfo, nullptr, &Device);
}

//---- Private --------------------------------------------------------------//

void VulkanBasic::findGraphicsQueue() {
  // Check that this has not been done already
  if (GraphicsQueueId > 0) return;
  uint32_t queueCount;

  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount,
    nullptr);
  assert(queueCount > 1 && "No queue for physical device : Fatal Error\n");

  std::vector<VkQueueFamilyProperties> queueProperties;
  queueProperties.resize(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount,
    queueProperties.data());

  // Check if we can find a queue which supports graphics
  for (; GraphicsQueueId < queueCount; ++GraphicsQueueId) {
    if (queueProperties[GraphicsQueueId].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      break;
  }
  assert(GraphicsQueueId < queueCount && "No graphics queue found for " &&
    "physical device : Fatal Error\n");
}

void VulkanBasic::setPhysicalDevice() {
  assert(Instance == 0 && "VkInstance not initialized before device counting\n");

  uint32_t deviceCount = 0;
  VkResult error       = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
                           nullptr);

  assert(!error && "Error during device count : Fatal Error\n");
  assert(deviceCount > 0 && "No valid vulkan devices found : Fatal Error\n");

  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  error = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
            physicalDevices.data());

  assert(!error && "Could not enumerate physical devices : Fatal Error\n");

  // Set the physical device to the first one in the list for now
  PhysicalDevice = physicalDevices[0];
}
