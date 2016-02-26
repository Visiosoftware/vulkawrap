//---- src/widget/vulkan_widget_base.h - Base vulkan widget -----------------//
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
// \file   vulkan_widget_base.hpp
// \brief  Defines a base class for a vulkan widget which can render images to
// the screen.
//---------------------------------------------------------------------------//

#ifndef VULKAN_WIDGET_VULKAN_WIDGET_BASE_HPP
#define VULKAN_WIDGET_VULKAN_WIDGET_BASE_HPP

//---- Utility Macros -------------------------------------------------------//

// Gets the procedure address based on a Vulkan insance.
#define GET_INSTANCE_PROC_ADDR(instance, entrypoint)                          \
{                                                                             \
  fp##entrypoint = (PFN_vk##entrypoint)                                       \
    vkGetInstanceProcAddr(instance, "vk"#entrypoint);                         \
  if (fp#entrypoint == nullptr) {                                             \
    exit(1);                                                                  \
  }                                                                           \
}

// Gets the procedure address based on a Vulkan device
#define GET_DEVICE_INSTANCE_PROC_ADDR(device, entrypoint)                     \
{                                                                             \
  fp##entrypoint = (PFN_vk##entrypoint)                                       \
    vkGetDeviceProcAddr(device, "vk"#entrypoint);                             \
  if (fp#entrypoint == nullptr) {                                             \
    exit(1);                                                                  \
  }                                                                           \
}

//---- Utility Struct -------------------------------------------------------//

typedef struct SwapChainBufferWrapper {
  VkImage     image;
  VkImageView view;
} SwapChainBuffer;

// Defines a class which defines the general functionality required for
// rendering to the window system with Vulkan. 
//
// \tparam WsiType The Window System Integration which is used -- so the native
// window platform for whatever OS this is running on.
template <typename WsiType>
class VulkanWidgetBase {
 public:
  // Alias for vector of images
  using VkImageVec  = std::vector<VkImage>;           // Image container alias.
  using VkBufferVec = std::vector<SwapChainBuffer>;   // Buffer container.

  // TODO: Read doc and comment these
  VkFormat        colorFormat;
  VkColorSpaceKHR colorSpace;
  VkSwapchainKHR  swapChain;    
  uint32_t        imageCountt;  // Number of images -- not sure why =/.
  uint32_t        queueNodeId;  // Index of node in the queue.
  VkImageVec      images;       // Images used in the swapchain.
  VkBufferVec     buffers;      // Buffers for the swapchain images.
  
  // Constructor -- initializes the Vulkan variables.
  // TODO: Change UINT32_MAX
  VulkanWidgetBase() 
  : swapChain(VK_NULL_HANDLE), QueueNodeIndex(UIN32_MAX) {
  }

  // Connects the instance and the device and gets all the required function
  // pointers based on the instance and the device.
  // 
  // \param instance The vulkan instance to use to set this class's instance
  // \param physicalDevice The vulkan physical device to use to set this
  // class's physical device to.
  // \param device The Vulkan logical device to set this class's logical device
  // to.
  void connectVulkanParams(VkInstance instance, VkPhysicalDevice physicalDevice,
    VkDevice device);

 private:
  VkInstance       Instance;       // The vulkan instance used for the widget.
  VkDevice         Device;         // The logical device.
  VkPhysicalDevice PhysicalDevice; // The hardware device which draws.
  VkSurfaceKHR     Surface;        // The surface to draw to.

  //---- Function pointers --------------------------------------------------//

  // Function pointer to allow for cheching if the WSI supports presentation.
  PFN_vkGetPhysicalDeviceSurfaceSupportKHR 
    FpGetPhysicalDeviceSurfaceSupportKHR;   

  // Function pointer to determine the capabiliteis of 
  // a surface which is required to create a swapchain.
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR 
    FpGetPhysicalDeviceSurfaceCapabilitiesKHR; 

  // Function pointer to determine the swapchain
  // format-colorspace paits for a surface
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR 
    FpGetPhysicalDeviceSurfaceFormatsKHR;

  // Function pointer to determine the supported presentation modes.
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR 
    FpGetPhysicalDeviceSurfacePresentModesKHR;

  // Function pointer to create a swapchain.
	PFN_vkCreateSwapchainKHR    FpCreateSwapchainKHR;   

  // Function pointer to destroy a swapchain.
	PFN_vkDestroySwapchainKHR   FpDestroySwapchainKHR;    

  // FUnction pointer to ger the iamges for a swapchain.
	PFN_vkGetSwapchainImagesKHR FpGetSwapchainImagesKHR;

  // Function poiner to get the next image in the 
  // swapchain so that is can be presented. 
	PFN_vkAcquireNextImageKHR   FpAcquireNextImageKHR;

  // Function pointer to present the queue for display.
	PFN_vkQueuePresentKHR       FpQueuePresentKHR;

  // Provides non-const access to the derived class.
  WsiType* wsiType() {
    return static_cast<WsiType*>(this);
  }

  // Provides const access to the derived class.
  const WsiType* wsiType() const {
    return static_cast<const WsiType*>(this);
  }

};

//---- Public ---------------------------------------------------------------//

template <typename WsiType>
void VulkanWidgetBase::connectVulkanParams(VkInstance instance, 
    VkPhysicalDevice physicalDevice, VkDevice device) {
  Instance        = instance;
  PhysicalDevice  = physicalDevice;
  Device          = device;

  // Get all the function pointers
  GET_INSTANCE_PROC_ADDR( instance, GetPhysicalDeviceSurfaceSupportKHR      );
  GET_INSTANCE_PROC_ADDR( instance, GetPhysicalDeviceSurfaceCapabilitiesKHR );
  GET_INSTANCE_PROC_ADDR( instance, GetPhysicalDeviceSurfaceFormatsKHR      );
  GET_INSTANCE_PROC_ADDR( instance, GetPhysicalDeviceSurfacePresentModeKHR  );
  GET_DEVICE_PROC_ADDR( device, CreateSwapchainKHR   );
  GET_DEVICE_PROC_ADDR( device, DestroySwapchainKHR  );
  GET_DEVICE_PROC_ADDR( device, GetSwapchainImageKHR );
  GET_DEVICE_PROC_ADDR( device, AquireNextImageKHR   );
  GET_DEVICE_PROC_ADDR( device, QueuePresentKHR      );
}

#endif  // VULKAN_WIDGET_VULKAN_WIDGET_BASE_HPP


