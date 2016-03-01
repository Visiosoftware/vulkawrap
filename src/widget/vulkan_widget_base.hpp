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

#include "widget_traits.h"
#include "../vulkan/vulkan_basic.h"
#include "../utilities/vulkan_utils.h"
#include <cassert>
#include <cstdlib>
#include <limits>

//---- Utility Macros -------------------------------------------------------//

// Gets the procedure address based on a Vulkan insance.
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                              \
{                                                                             \
  fp##entrypoint =                                                            \
    (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk"#entrypoint);         \
  if (fp##entrypoint == nullptr) {                                            \
    exit(1);                                                                  \
  }                                                                           \
}

// Gets the procedure address based on a Vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                 \
{                                                                             \
  fp##entrypoint =                                                            \
    (PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk"#entrypoint);            \
  if (fp##entrypoint == nullptr) {                                            \
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
// \tparam WidgetTraits The traits for a widget.
template <typename WsiType, typename WidgetTraits>
class VulkanWidgetBase : VulkanBasic {
 public:
  using VkImageVec  = std::vector<VkImage>;               // Image container.
  using VkBufferVec = std::vector<SwapChainBuffer>;       // Buffer container.
  using HandleType  = typename WidgetTraits::HandleType;  // Handle for window.
  using WindowType  = typename WidgetTraits::WindowType;  // Type of window

  // TODO: Read doc and comment these
  VkFormat        colorFormat;
  VkColorSpaceKHR colorSpace;
  VkSwapchainKHR  swapChain;    
  uint32_t        imageCount;   // Number of images 
  uint32_t        queueNodeId;  // Index of node in the queue.
  VkImageVec      images;       // Images used in the swapchain.
  VkBufferVec     buffers;      // Buffers for the swapchain images.
  
  // Constructor -- initializes the Vulkan variables.
  // TODO: Change UINT32_MAX
  VulkanWidgetBase() 
    : swapChain(VK_NULL_HANDLE), 
      queueNodeId(std::numeric_limits<uint32_t>::max()) {
    // The base VulkanBasic class provides the badic functionality for vulkan.
    // We then connect the vulkan basic functionality to the widget. 
    connectVulkan();
    
    // Setup the VulkanBasic base class 
    this->setup(queueNodeId);
  }

  // Frees all the vulkan resources used by the swapchain.
  ~VulkanWidgetBase() {
    for (auto& buffer : buffers) 
      vkDestroyImageView(Device, buffer.view, nullptr);
    fpDestroySwapchainKHR(Device, swapChain, nullptr);
    vkDestroySurfaceKHR(Instance, Surface, nullptr);
  }

  // Gets the next iamge in the swapchain. Results the result of the operation.
  //
  // TODO: Add more detail here
  // \param semaphore The present complete semaphore ...
  // \param currentBuffer The current buffer
  VkResult acquireNextImage(VkSemaphore semaphore, uint32_t* currentBuffer) {
    return fpAcquireNextImageKHR(Device, swapChain, 
             std::numeric_limits<uint64_t>::max(), semaphore, 
             static_cast<VkFence>(nullptr), currentBuffer);
  }

  // Creates the vulkan swap chain and gets the image sizes and widhts.
  //
  // \param width The image width.
  // \param height The image height.
  void createSwapchain(uint32_t* width, uint32_t* height);

  // Initializes the widget -- specifically this initializes the vulkan
  // surface by calling the platform specific implemenatation and then 
  // performing the non platform specific initialization.
  //
  // \param window The window to initialize the surface for.
  // \param handle The handle for the window.
  void initialize(WindowType window, HandleType handle = nullptr);

  // Presents the current image to the present queue. Returns the result of the
  // operation.
  //
  // \param queue The queue to present to.
  // \param currentBuffer The current buffer.
  VkResult queuePresent(VkQueue queue, uint32_t currentBuffer) {
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext           = nullptr;
    presentInfo.swapchainCount  = 1; 
    presentInfo.pSwapchains     = &swapChain;
    presentInfo.pImageIndices   = &currentBuffer;
    return fpQueuePresentKHR(queue, &presentInfo);
  }

 protected:
  VkSurfaceKHR Surface;  // The surface to draw to.

  // Provides access to the instance for derived classes.
  VkInstance instance() { return this->Instance; }

 private:
  //---- Function pointers --------------------------------------------------//

  // Function pointer to allow for cheching if the WSI supports presentation.
  PFN_vkGetPhysicalDeviceSurfaceSupportKHR 
    fpGetPhysicalDeviceSurfaceSupportKHR;   

  // Function pointer to determine the capabiliteis of 
  // a surface which is required to create a swapchain.
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR 
    fpGetPhysicalDeviceSurfaceCapabilitiesKHR; 

  // Function pointer to determine the swapchain
  // format-colorspace paits for a surface
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR 
    fpGetPhysicalDeviceSurfaceFormatsKHR;

  // Function pointer to determine the supported presentation modes.
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR 
    fpGetPhysicalDeviceSurfacePresentModesKHR;

  // Function pointer to create a swapchain.
	PFN_vkCreateSwapchainKHR    fpCreateSwapchainKHR;   

  // Function pointer to destroy a swapchain.
	PFN_vkDestroySwapchainKHR   fpDestroySwapchainKHR;    

  // FUnction pointer to ger the iamges for a swapchain.
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;

  // Function poiner to get the next image in the 
  // swapchain so that is can be presented. 
	PFN_vkAcquireNextImageKHR   fpAcquireNextImageKHR;

  // Function pointer to present the queue for display.
	PFN_vkQueuePresentKHR       fpQueuePresentKHR;

  //---- Base access --------------------------------------------------------//

  // Provides non-const access to the derived class.
  WsiType* wsiType() {
    return static_cast<WsiType*>(this);
  }

  // Provides const access to the derived class.
  const WsiType* wsiType() const {
    return static_cast<const WsiType*>(this);
  }
  
  //---- Rest ---------------------------------------------------------------// 
  
  // Connects the instance and the device and gets all the required function
  // pointers based on the instance and the device.
  void connectVulkan();

  // Create the buffers for the swapchain.
  void createSwapchainBuffers(VkCommandBuffer commandBuffer);
};

//---- Public ---------------------------------------------------------------//

template <typename WsiType, typename WidgetTraits>
void VulkanWidgetBase<WsiType, WidgetTraits>::connectVulkan() {

  // Get all the function pointers
  GET_INSTANCE_PROC_ADDR( Instance, GetPhysicalDeviceSurfaceSupportKHR      );
  GET_INSTANCE_PROC_ADDR( Instance, GetPhysicalDeviceSurfaceCapabilitiesKHR );
  GET_INSTANCE_PROC_ADDR( Instance, GetPhysicalDeviceSurfaceFormatsKHR      );
  GET_INSTANCE_PROC_ADDR( Instance, GetPhysicalDeviceSurfacePresentModesKHR );
  GET_DEVICE_PROC_ADDR( Device, CreateSwapchainKHR    );
  GET_DEVICE_PROC_ADDR( Device, DestroySwapchainKHR   );
  GET_DEVICE_PROC_ADDR( Device, GetSwapchainImagesKHR );
  GET_DEVICE_PROC_ADDR( Device, AcquireNextImageKHR   );
  GET_DEVICE_PROC_ADDR( Device, QueuePresentKHR       );
}

template <typename WsiType, typename WidgetTraits>
void VulkanWidgetBase<WsiType, WidgetTraits>::createSwapchain(uint32_t* width, 
    uint32_t* height) {
  VkResult        error;
  VkSwapchainKHR  oldSwapchain  = swapChain;
  VkCommandBuffer commandBuffer = this->SetupCmndBuffer; 

  // Get the surface properties and formats for the physical device.
  VkSurfaceCapabilitiesKHR surfCapabilities;
  error = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, 
            &surfCapabilities);
  assert(!error && "Could not get surface capabilites : Fatal Error\n");

  // Get the available present modes for the surface and phy device.
  uint32_t numPresentModes;
  error = fpGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, 
            &numPresentModes, nullptr);
  assert(!error && "Failed to get number of present modes : Fatal Error\n");
  assert(numPresentModes > 0 && "No present modes for physical devices : "
    && "Fatal Error\n");

  // Create the present modes and get the data for them.
  std::vector<VkPresentModeKHR> presentModes(numPresentModes);
  error = fpGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface,
            &numPresentModes, presentModes.data());
  assert(!error && "Failed to create present modes : Fatal Error\n");

  VkExtent2D swapChainExtent = {};

  // EIther both the width and the height are -1 or none are.
  if (surfCapabilities.currentExtent.width == -1) {
    // If the surface is not defined, then set the size 
    // to the size requested by the function arguments.
    swapChainExtent.width  = *width;
    swapChainExtent.height = *height;
  } else {
    // If the surface size is defined, then 
    // the swapchain size needs to match.
    swapChainExtent = surfCapabilities.currentExtent;
    *width          = surfCapabilities.currentExtent.width;
    *height         = surfCapabilities.currentExtent.height;
  }

  // Mailbox mode is chosen if present because it's 
  // the lowest latency non-tearning present mode.
  VkPresentModeKHR swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (const auto& presentMode : presentModes) {
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      swapChainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
    if (swapChainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR &&
        presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) { 
      swapChainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
  }
  
  // Determine the number of images.
  uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;
  if (surfCapabilities.maxImageCount > 0 &&
      desiredNumberOfSwapChainImages > surfCapabilities.maxImageCount) {
    desiredNumberOfSwapChainImages = surfCapabilities.maxImageCount;
  } 
  
  // Get the supported transforms for the surface.
  VkSurfaceTransformFlagsKHR preTransform;
  if (surfCapabilities.supportedTransforms & 
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  } else {
    preTransform = surfCapabilities.currentTransform;
  }
  
  // Define the creation information for the swapchain
  VkSwapchainCreateInfoKHR swapChainInfo = {};
  swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainInfo.pNext                 = nullptr;
  swapChainInfo.surface               = Surface;
  swapChainInfo.minImageCount         = desiredNumberOfSwapChainImages;
  swapChainInfo.imageFormat           = colorFormat;
  swapChainInfo.imageColorSpace       = colorSpace;
  swapChainInfo.imageExtent           = { swapChainExtent.width, 
                                          swapChainExtent.height };
  swapChainInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainInfo.preTransform          =
    static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
  swapChainInfo.imageArrayLayers      = 1;
  swapChainInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
  swapChainInfo.queueFamilyIndexCount = 0;
  swapChainInfo.pQueueFamilyIndices   = nullptr;
  swapChainInfo.presentMode           = swapChainPresentMode;
  swapChainInfo.oldSwapchain          = oldSwapchain;
  swapChainInfo.clipped               = true;
  swapChainInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  // Create the swapchain.
  error = fpCreateSwapchainKHR(Device, &swapChainInfo, nullptr, &swapChain);
  assert(!error && "Failed to creat swapchain : Fatal Error\n");

  // If an existing swap chain is re-created the destroy the old one.
  // Additionally, this cleans up all the presentatble images.
  if (oldSwapchain != VK_NULL_HANDLE) 
    fpDestroySwapchainKHR(Device, oldSwapchain, nullptr);

  // Get all the swapchain images.
  error = fpGetSwapchainImagesKHR(Device, swapChain, &imageCount, nullptr);
  assert(!error && "Failed to creat swapchain images : Fatal Error\n");

  // Create the swapchain buffers with the vulkan image and imageview
  createSwapchainBuffers(commandBuffer);
  
}

template <typename WsiType, typename WidgetTraits>
void VulkanWidgetBase<WsiType, WidgetTraits>::initialize(
    typename WidgetTraits::WindowType window, 
    typename WidgetTraits::HandleType handle) {
  VkResult error;

  // First initialize the platform specific window with vulkan.
  wsiType()->initialize(window, handle);

  // Now do everything that is not platform specific
  
  // Get the available queue family properties.
  uint32_t queueCount;
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, 
    nullptr);
  assert(queueCount > 1 && "No queue family properties found : Fatal Error\n");

  std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount,
    queueProperties.data());

  // Iterate over each of the queues and check if the queue can present.
  // Find a queue which does support presentation which will be used with 
  // the swapchain and the windowing system to render the widget.
  std::vector<VkBool32> supportsPresentation(queueCount);
  for (uint32_t i = 0; i < queueCount; ++i) 
    fpGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface,
      &supportsPresentation[i]);

  // Search for a graphics and a present queue in the vector of 
  // queue families and try to find one which supports both.
  uint32_t graphicsQueueId = std::numeric_limits<uint32_t>::max();
  uint32_t presentQueueId  = std::numeric_limits<uint32_t>::max();
  for (uint32_t i = 0; i < queueCount; ++i) {
    if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      if (supportsPresentation[i] == VK_TRUE) {
        graphicsQueueId = i;
        presentQueueId  = i;
        break;
      }
    
      if (graphicsQueueId == std::numeric_limits<uint32_t>::max()) 
        graphicsQueueId = i;
    }
  }

  // If there is no queue which supports both graphics and 
  // presentation then find one which supports presentation.
  if (presentQueueId == std::numeric_limits<uint32_t>::max()) {
    for (uint32_t i = 0; i < queueCount; ++i) {
      if (supportsPresentation[i] != VK_TRUE) 
        continue;
      presentQueueId = i; 
      break;
    }
  }

  // If either a graphics or present queue has not been 
  // found, then we cannot continue, so we terminate.
  // TODO: Add support for different graphics and present queues.
  assert((graphicsQueueId != std::numeric_limits<uint32_t>::max()  ||
          presentQueueId  != std::numeric_limits<uint32_t>::max()) &&
          "Graphics or present queue not found : Fatal Error\n");
  assert(graphicsQueueId == presentQueueId && "Graphics and present queue "
         && "do not match : Fatal Error\n");

  // No failure, so the class queue node id can be set.
  queueNodeId = graphicsQueueId;

  // Get a list of supported surface formats
  uint32_t formatCount;
  error = fpGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface,
            &formatCount, nullptr);
  assert(!error && "Failed to get the number of surface formats : " &&
         "Fatal Error\n");
  assert(formatCount > 0 && "Failed to find any supported surface formats " 
         && "Fatal Error\n");

  std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
  error = fpGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice,Surface, 
            &formatCount, surfaceFormats.data());
  assert(!error && "Failed to create surface formats : Fatal Error\n");

  // If the format list has only a single entry and
  // it is VK_FORMAT_UNDEFINED then the format which
  // is assumed is VK_FORMAT_B8G8R8A8_UNORM.
  if (formatCount == 1 && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) 
    colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
  else 
    colorFormat = surfaceFormats[0].format;

  colorSpace = surfaceFormats[0].colorSpace;

}

//---- Private --------------------------------------------------------------//

template <typename WsiType, typename WidgetTraits>
void VulkanWidgetBase<WsiType, WidgetTraits>::createSwapchainBuffers(
    VkCommandBuffer commandBuffer) {
  VkResult error;
  buffers.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i) {
    VkImageViewCreateInfo colorAttachmentViewInfo = {};
    colorAttachmentViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorAttachmentViewInfo.pNext      = nullptr;
    colorAttachmentViewInfo.format     = colorFormat;
    colorAttachmentViewInfo.components = { VK_COMPONENT_SWIZZLE_R, 
                                           VK_COMPONENT_SWIZZLE_G, 
                                           VK_COMPONENT_SWIZZLE_B, 
                                           VK_COMPONENT_SWIZZLE_A };

    colorAttachmentViewInfo.subresourceRange.aspectMask     =
      VK_IMAGE_ASPECT_COLOR_BIT;
    colorAttachmentViewInfo.subresourceRange.baseMipLevel   = 0;
    colorAttachmentViewInfo.subresourceRange.levelCount     = 1;
    colorAttachmentViewInfo.subresourceRange.baseArrayLayer = 0;
    colorAttachmentViewInfo.subresourceRange.layerCount     = 1;

    colorAttachmentViewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
    colorAttachmentViewInfo.flags      = 0;

    // Set the vulkan image in the buffer to this image.
    buffers[i].image = images[i];

    // Transform the image from the undefined to the present layout.
    vkutil::transformImageLayout(commandBuffer, buffers[i].image,
      VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, 
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    

    colorAttachmentViewInfo.image = buffers[i].image;
    error = vkCreateImageView(Device, &colorAttachmentViewInfo, nullptr, 
              &buffers[i].view);
    assert(!error && "Failed to create image view for image : Fatal Error\n"); 
  }
}

//---- Alias's --------------------------------------------------------------//

// Aliases for platform specific widgets.
#if defined(_WIN32)
  // #include "vulkan_widget_windows.h"
  // using VulkanWidget = VulkanWidgetWindows;
#elif defined(__linux__)
  #include "vulkan_widget_linux.h"
  using DrawableWidget = VulkanWidgetLinux;
#elif defined(__ANDROID__)
  // #include "vulkan_widget_android.h"
  // using VulkanWidget = VulkanWidgetAndroid;
#endif
  
#endif  // VULKAN_WIDGET_VULKAN_WIDGET_BASE_HPP


