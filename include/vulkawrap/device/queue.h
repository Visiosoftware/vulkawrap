//---- include/vulkawrap/device/queue.h -------------------- -*- C++ -*- ----//
//
//                                  Vulkawrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file  queue.h
/// \brief Defines wrappers around a Vulkan Queue to simplify the use the raw
///        Vulkan Queue.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_DEVICE_QUEUE_H
#define VULKAWRAP_DEVICE_QUEUE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace vwrap {
 
/// The types of queues which are supported -- these are the same values as
/// defined by vulkan, but with the addition of the VW_ANY type, to allow for
/// type of queue to be sepecified.
enum class QueueType : uint8_t {
  VW_GRAPHICS_QUEUE       = 0x01,
  VW_COMPUTE_QUEUE        = 0x02,
  VW_TRANSFER_QUEUE       = 0x04,
  VW_SPARSE_BINDING_QUEUE = 0x08,
  VW_ANY                  = 0x10
};

//---- Aliases --------------------------------------------------------------//

using QueueTypeVec        = std::vector<QueueType>;
using QueueIdVec          = std::vector<uint32_t>;
using QueueFamilyPropVec  = std::vector<VkQueueFamilyProperties>;

} // namespace vwrap 

#endif  // VULKAWRAP_DEVICE_QUEUE_H


