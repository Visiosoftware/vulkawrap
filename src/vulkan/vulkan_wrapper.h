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

//---- Structs --------------------------------------------------------------//

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

//---- Functions ------------------------------------------------------------//


//---- Aliases --------------------------------------------------------------//

using VwPhysDeviceVec    = std::vector<VkPhysicalDevice>;
using VwPhysDeviceMemVec = std::vector<VkPhysicalDeviceMemoryProperties>;
using VwQueueFamPropVec  = std::vector<VkQueueFamilyProperties>;

#endif  // VULKAN_VULKAN_VULKAN_WRAPPER_H


