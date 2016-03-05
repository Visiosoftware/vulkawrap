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
#include <algorithm>
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

using VwQueueFamPropVec  = std::vector<VkQueueFamilyProperties>;
using VwQueueTypeVec     = std::vector<VwQueueType>;
using VwQueueIdVec       = std::vector<uint32_t>;

//---- Structs --------------------------------------------------------------//

// Wrapper for Vulkan Physical Devices to include the queus which are
// assosciated with the physical device, so that these types of devices can be
// used to let the user speciffy a device type and the queues which it must
// support.
struct VwPhysicalDevice {
  VkPhysicalDevice    device;
  VwQueueTypeVec      queueTypes;
  VwQueueIdVec        queueIds;

  // Default constructor 
  VwPhysicalDevice() 
  : queueTypes(0), queueIds(0) {};

  // Constructor which takes a vulkan physical device.
  //
  // \param vkPhysicalDevice The vulkan physical device.
  VwPhysicalDevice(const VkPhysicalDevice& vkPhysicalDevice) 
  : device(vkPhysicalDevice), queueTypes(0), queueIds(0) {}

  // Constructor which takes the device, the queue types and indexes of those
  // queues.
  //
  // \param vkPhysicalDevice The vulkan physical device.
  // \param qTypes           The types of queus which are supported.
  // \param qIds             The indexes of each queue type.
  VwPhysicalDevice(const VkPhysicalDevice vkPhysicalDevice, 
    const VwQueueTypeVec& qTypes, const VwQueueIdVec qIds) 
  : device(vkPhysicalDevice), queueTypes(qTypes), queueIds(qIds) {}

  // Checks for the requested queues, and adds those which are a match.
  //
  // \param requestedQueueType The type of queues the user requests the device
  // should have.
  void addSupportedQueues(const VwQueueTypeVec& requestedQueueTypes) {
    uint32_t queueCount, queueId = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    if (queueCount < 1) return;

    VwQueueFamPropVec queueProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, 
      queueProperties.data());
    
    for (; queueId < queueCount; ++queueId) {
      for (const auto& queueType : requestedQueueTypes) {
        if (!(static_cast<uint8_t>(queueProperties[queueId].queueFlags) & 
              static_cast<uint8_t>(queueType))) {
          continue;
        }

        queueIds.push_back(queueId);
        queueTypes.push_back(queueType);
      }
    }
  }
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
  VwDeviceSpecifier(VwDeviceType device, QType qType, QTypes... qTypes)
  : deviceType(device), queueTypes{qType, qTypes...} {}

  VwDeviceType    deviceType; // The type of device to look for.
  VwQueueTypeVec  queueTypes; // The types of queues the device must support.
};

//---- Aliases2 -------------------------------------------------------------//

using VwDeviceSpecVec = std::vector<VwDeviceSpecifier>;
using VwPhysDeviceVec = std::vector<VwPhysicalDevice>;

//---- Functions ------------------------------------------------------------//

namespace algo {

// Wrapper around erase and remove_if to remove elements from a vector over the
// entire vector.
template <typename VecType, typename Predicate>
inline void vecRemoveIf(VecType& vec, Predicate predicate) {
  vec.erase(std::remove_if(vec.begin(), vec.end(), predicate), vec.end());
}

// Wrapper around erase and remove_if to remove elements when the start and end
// iterator for the vector are given.
template <typename VecType, typename ForwardIt, typename Predicate>
inline void vecRemoveIf(VecType& vec, ForwardIt first, ForwardIt last, 
       Predicate predicate) {
  vec.erase(std::remove_if(first, last, predicate), last);
}

}  // namespace algo

#endif  // VULKAN_VULKAN_VULKAN_WRAPPER_H


