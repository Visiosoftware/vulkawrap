//---- src/vulkan/vulkan_utils.cpp - Implementation of vulkan utilities -----//
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
// \file   vulkan_util.cpp
// \brief  Implementation file of utilitiy functions for vulkan.
//---------------------------------------------------------------------------//

#include "vulkan_utils.h"

namespace vkutil {

namespace {

inline void checkSourceLayouts(VkImageMemoryBarrier& imageMemoryBarrier,
    const VkImageLayout oldImageLayout) {
  // Check for undefined layout -- only allowed as initial layout
  // Needs to check that all writes to the image have completed.
  if (oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED) 
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT 
                                     | VK_ACCESS_TRANSFER_WRITE_BIT;

  // Checks if the old layout is a color attachment -- needs to 
  // make sure any writes to the color buffer have finished.
  if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // Checks if the old layout is a transfer source -- needs to 
  // make sure that any read from the image have finished.
  if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) 
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

  // Check if the old layout is a shader read (a sample or input attachment)
  // Needs to check that any shader reads from the input image have finished.
  if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT; 
}

inline void checkDestinationLayouts(VkImageMemoryBarrier& imageMemoryBarrier,
    const VkImageLayout newImageLayout) {
  // Checks if the new layout is a transfer destination -- copy, blit 
  // Needs to make sure that any copies to the image have finished.
  if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

  // Checks if the new layout is a transfer source -- copy, blit
  // Needs to make sure any reads from the image have finished.
  if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
    imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask 
                                     | VK_ACCESS_TRANSFER_READ_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  }

  // Checks if the new layout is a color attachment -- needs
  // to make sure that writes to the color buffer have finished.
  if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  }

  // Checks if the new layout is a depth attchment -- needs to make
  // sure that any write to the depth/stencil buffer have finished.
  if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
    imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask 
      | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  // Checks if the new layout is a shader read -- sampler, inputattachment
  // Needs to make sure all write to the image have finished.
  if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT
                                     | VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  }
}

}  // annonymous namespace

void transformImageLayout(VkCommandBuffer commandBuffer, VkImage image, 
    VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, 
    VkImageLayout newImageLayout) {
  // Create an image memory barrier object.
  VkImageMemoryBarrier imageMemoryBarrier = init::imageMemoryBarrier();
  imageMemoryBarrier.oldLayout = oldImageLayout;
  imageMemoryBarrier.newLayout = newImageLayout;
  imageMemoryBarrier.image     = image;

  imageMemoryBarrier.subresourceRange.aspectMask   = aspectMask;
  imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
  imageMemoryBarrier.subresourceRange.levelCount   = 1;
  imageMemoryBarrier.subresourceRange.layerCount   = 1;

  checkSourceLayouts(imageMemoryBarrier, oldImageLayout);
  checkDestinationLayouts(imageMemoryBarrier, newImageLayout);

  // Put the barrier at the top of the pipeline.
  VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags dstStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  // Put the barrier inside the command buffer.
  vkCmdPipelineBarrier(commandBuffer, srcStageFlags, dstStageFlags, 0, 0, 
    nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

  
}  // namespace vkutil
