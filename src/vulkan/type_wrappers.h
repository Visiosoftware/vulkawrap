//----src/vulkan/type_wrappers.h - Utility types for vulkan -----------------//
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
// \file   type_wrappers.h
// \brief  Defines vulkan type wrappers.
//---------------------------------------------------------------------------//

#ifndef VULKAN_VULKAN_TYPE_WRAPPERS_H
#define VULKAN_VULKAN_TYPE_WRAPPERS_H

#include <vulkan/vulkan.h>
#include <vector>

//----- Structs -------------------------------------------------------------//

struct SwapchainBuffer {
  VkImage     image;
  VkImageView view;
};

struct StencilBuffer {
  VkImage        image;
  VkImageView    view;
  VkDeviceMemory memory;
};

//---- Aliases --------------------------------------------------------------//

using VkCmndBufferVec         = std::vector<VkCommandBuffer>;
using VkFramebufferVec        = std::vector<VkFramebuffer>; 
using VkImageVec              = std::vector<VkImage>;
using VkPhysDeviceVec         = std::vector<VkPhysicalDevice>;
using VhPhysDeviceMemPropVec  = std::vector<VkPhysicalDeviceMemoryProperties>;
using VkScBufferVec           = std::vector<SwapchainBuffer>;

#endif  // VULKAN_VULKAN_TYPE_WRAPPERS_H
