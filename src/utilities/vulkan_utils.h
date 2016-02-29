//---- src/vulkan/vulkan_utils.h - Utility functions for vulkan -------------//
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
// \file   vulkan_util.h
// \brief  Defines utility functions for vulkan.
//---------------------------------------------------------------------------//

#ifndef VULKAN_UTILITIES_VULKAN_UTILS_H
#define VULKAN_UTILITIES_VULKAN_UTILS_H

#include <vulkan/vulkan.h>

namespace vkutil {

// Creates an image memory barrier to change the image layout and puts it into
// the supplied command buffer. For more info see sec 11.4 of Vulkan spec.
//
// \param commandBuffer The command buffer to put the image mem bariier into.
// \param image         The image to create the barrier for.
// \param aspectMask    
// \param oldImageLayout The image layout to modify.
// \param newImageLayout The image layout to change to.
void transformImageLayout(VkCommandBuffer commandBuffer, VkImage image, 
  VkImageAspectFlags aspectFlags, VkImageLayout oldImageLayout, 
  VkImageLayout newImageLayout);

// Gets all supported depth formats for a physical device. Returns false if
// there are no valid depth formats.
//
// \param physicalDevice The physical device to get the depth formats for.
// \param depthFormat The depth formats.
VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice,
  VkFormat* depthFormat);

namespace init {

inline VkImageMemoryBarrier imageMemoryBarrier() {
  VkImageMemoryBarrier imageMemoryBarrier = {};
  imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageMemoryBarrier.pNext = nullptr;
  imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  return imageMemoryBarrier;
}

} // namespace init
} // namespace vkutil

#endif  // VULKAN_UTILITIES_VULKAN_UTILS_H

