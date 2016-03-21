//---- src/vulkawrap/device/filter.cc ---------------------- -*- C++ -*- ----//
//
//                                Vulkawrap
//                                
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   filter.cc
/// \brief  Implementation of device filtering functionality for Vulkawrap,
///         which provides a clean interface for filtering out physical devices
///         based on their type and the queue types which they support.
//
//---------------------------------------------------------------------------//

#include "vulkawrap/device/filter.h"
#include "vulkawrap/util/assert.hpp"

namespace vwrap {
 
//---- Public ---------------------------------------------------------------//

bool DeviceFilter::addIfQueuesAreSupported(
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

std::vector<VkPhysicalDevice> DeviceFilter::getPhysicalDevices() const {
  uint32_t deviceCount = 0;
  VkResult result      = vkEnumeratePhysicalDevices(Instance->vkInstance, 
                           &deviceCount, nullptr);
  util::AssertSuccess(result, "Failed to enumerate physical devices.\n");
  util::Assert(deviceCount >= 1, "Failed to find any physical devices.\n");

  std::vector<VkPhysicalDevice> physicalDevices;
  physicalDevices.resize(deviceCount); 
  result = vkEnumeratePhysicalDevices(Instance->vkInstance, &deviceCount, 
             physicalDevices.data());
  util::AssertSuccess(result, "Could not enumerate physical devices.\n");
  return physicalDevices;
}

}  // namespace vwrap
