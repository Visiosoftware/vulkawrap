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

//---- Enums ----------------------------------------------------------------//

// Enums for the types of physical devices which are supported -- these are the
// same values as they are in vulkan, but with the addition of the any type, to
// allow for indifference.
enum class VwDeviceType : uint8_t {
  VW_OTHER          = 0,
  VW_INTEGRATED_GPU = 1,
  VW_DISCRETE_GPU   = 2,
  VW_VIRTUAL_GPU    = 3,
  VW_CPU            = 4,
  VW_ANY            = 5
};

// Enum for the types of queues which are supported -- these are the same
// values as for pure vulkan, but with the addition of the any type, so that
// any queue can be specified.
enum class VwQueueType : uint8_t {
  VW_GRAPHICS_QUEUE       = 0x01,
  VW_COMPUTE_QUEUE        = 0x02,
  VW_TRANSFER_QUEUE       = 0x04,
  VW_SPARSE_BINDING_QUEUE = 0x08,
  VW_ANY                  = 0x10
};

//---- Aliases --------------------------------------------------------------//

using VwPhysDeviceVec    = std::vector<VkPhysicalDevice>;
using VwPhysDeviceMemVec = std::vector<VkPhysicalDeviceMemoryProperties>;
using VwQueueFamPropVec  = std::vector<VkQueueFamilyProperties>;
using VwQueueTypeVec     = std::vector<VwQueueType>;

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

// Struct to allow the device type and the queues it must support be specified.
struct VwDeviceSpecifier {
  // Default constructor -- specify the device type and the queues it needs to
  // support.
  //
  // \note While only the first type is checked for equivalence with
  // VwQueueType, if any of the parameter pack is not VwQueueType then
  // construction of the vector will fail, which is what we want.
  //
  // \param QType   The type of the first queue which must be supported for
  // the device
  // \param QTypes  The types of the rest of the queues which must be
  // supported
  template <typename QType, typename... QTypes, typename = 
            std::enable_if_t<std::is_same<VwQueueType, QType>::value>>
  explicit VwDeviceSpecifier(VwDeviceType device, QType qType, QTypes... qTypes)
  : deviceType(device), queueTypes{qType, qTypes...} {}

  VwDeviceType    deviceType; // The type of device to look for.
  VwQueueTypeVec  queueTypes; // The types of queues the device must support.
};

//---- Aliases2 -------------------------------------------------------------//

using VwDeviceSpecVec = std::vector<VwDeviceSpecifier>;

//---- Functions ------------------------------------------------------------//


#endif  // VULKAN_VULKAN_VULKAN_WRAPPER_H


