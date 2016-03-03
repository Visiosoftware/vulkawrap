//---- src/vulkan/vulkan_wrapper.h - Wrappers around some vulkan types -----//
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
// \file   vulkan_wrapper.h
// \brief  Defines wrappers around some vulkan types.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_VULKAN_VULKAN_WRAPPER_H
#define VULKAN_VULKAN_VULKAN_WRAPPER_H

#include <vulkan/vulkan.h>
#include <vector>

//---- Aliases --------------------------------------------------------------//

using VwPhysDeviceVec    = std::vector<VkPhysicalDevice>;
using VwPhysDeviceMemVec = std::vector<VkPhysicalDeviceMemoryProperties>;
using VwQueueFamPropVec  = std::vector<VkQueueFamilyProperties>;

//---- Structs --------------------------------------------------------------//

// Wrapper around a vulkan physical device
struct VwPhysicalDevice {
  // Default constructor 
  VwPhysicalDevice() {};

  // Constructor so that we can return a VwPhysicalDevice given a vulkan
  // physical device and its memory properties.
  //
  // \param physicalDevice The physical device to wrap.
  // \param memoryProperties The memory properties of the physical device to
  // wrap
  VwPhysicalDevice(const VkPhysicalDevice& physicalDevice, 
                   const VkPhysicalDeviceMemoryProperties& memProperties)
  :  device(physicalDevice), memoryProperties(memProperties) {}

  VkPhysicalDevice                 device;
  VkPhysicalDeviceMemoryProperties memoryProperties;
};

enum class VwDeviceType : uint8_t {
  VW_OTHER          = 0,
  VW_INTEGRATED_GPU = 1,
  VW_DISCRETE_GPU   = 2,
  VW_VIRTUAL_GPU    = 3,
  VW_CPU            = 4,
  VW_ANY            = 5
};

enum class VwQueueType : uint8_t {
  VW_GRAPHICS_QUEUE       = 0x01,
  VW_COMPUTE_QUEUE        = 0x02,
  VW_TRANSFER_QUEUE       = 0x04,
  VW_SPARSE_BINDING_QUEUE = 0x08,
  VW_ANY                  = 0x10
};

using VwQueueTypeVec = std::vector<VwQueueType>;
 
// Struct to allow the device type and the queues it must support be specified.
struct VwDeviceSpecifier {
  // Default constructor -- specify the device type and the queues it needs to
  // support.
  explicit VwDeviceSpecifier(VwDeviceType device, VwQueueTypeVec queues)
  : deviceType(device), queueTypes(queues) {}

  VwDeviceType    deviceType; // The type of device to look for.
  VwQueueTypeVec  queueTypes; // The types of queues the device must support.
};

//---- Functions ------------------------------------------------------------//


#endif  // VULKAN_VULKAN_VULKAN_WRAPPER_H


