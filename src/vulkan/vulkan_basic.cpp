//---- src/vulkan/vulkan_basic.cpp - Implementation for vulkan basic-- ------//
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
// \file   vulkan_basic.cpp
// \brief  Implementation of the VulkanBasic class.
//---------------------------------------------------------------------------//

#include "vulkan_basic.h"
#include "../utilities/vulkan_utils.h"
#include <cassert>
#include <iostream>

namespace {

unsigned constexpr hash(char const* input) {
  return *input ? static_cast<unsigned int>(*input) + 33 * hash(input + 1) 
                : 5381;
}

}  // annonymous namespace

//---- Public ---------------------------------------------------------------//

// TODO : Update this so that you can set the name and add windowing extensions
//      : Add in non fatal error handling
VulkanBasic::VulkanBasic()
: GraphicsQueueId(0), SetupCmndBuffer(VK_NULL_HANDLE) {
  VkResult error = createInstance();
  assert(!error && "Could not create Vulkan instance : Fatal Error\n");

  setPhysicalDevice();    // Terminates if no device is found
  findGraphicsQueue();    // Terminates of no gfx queue is found

  // Create the properties for the logical devices
  const float queuePriorities       = 0.0f;
  VkDeviceQueueCreateInfo queueInfo = {};
  queueInfo.sType               =  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.queueFamilyIndex        = GraphicsQueueId;
  queueInfo.pQueuePriorities        = &queuePriorities;

  error = createDevice(queueInfo);
  assert(!error && "Failed to create device : Fatal Error\n");
 
  // Get the device memory properties
  vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &DeviceMemProps);

  // Get the graphics queue
  vkGetDeviceQueue(Device, GraphicsQueueId, 0, &Queue);

  // Find a suitable depth format.
  VkBool32 validDepthFormat = vkutil::getSupportedDepthFormat(PhysicalDevice, 
                                &DepthFormat); 
  assert(validDepthFormat && "Failed to get a valid depth format : " 
         && "Fatal Error\n");
}

void VulkanBasic::setup(uint32_t width, uint32_t height, uint32_t queueNodeId,
    const VkScBufferVec& swapchainBuffers) {
  createCommandPool(queueNodeId);
  createSetupCommandBuffer();
  startBufferRecording(buffers::setupBuffer);
  createCommandBuffers(swapchainBuffers.size());
  setupDepthStencil(width, height);
  setupRenderPass();
  createPipelineCache();
  setupFramebuffers(width, height, swapchainBuffers);

  flushSetupCommandBuffer();      // Write and then remove setup buffer.
  // CreateSetupCommandBuffer();   // Recreate setup buffer for other class
}

void VulkanBasic::startBufferRecording(const char* bufferType) {
  VkResult                 result;
  VkCommandBufferBeginInfo cmndBufferInfo = {};
  cmndBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  switch(hash(bufferType)) {
    case hash(buffers::setupBuffer) : {
      result = vkBeginCommandBuffer(SetupCmndBuffer, &cmndBufferInfo);
    } break;
    default : break;
  }
  assert(!result && "Failed to begin command buffer recording : "
         && "Fatal Error\n");
}

VkResult VulkanBasic::createInstance(const char* appName, 
    const char* engineName, const std::vector<const char*>& extensions) {
  // Set the application properties
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = appName;
  appInfo.pEngineName       = engineName;

  // Might need workaround here for invalid vulkan version
  appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 2);
 
  // Create a vector with the default extensions 
  // and then add the user defined extensions
  std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME }; 
  for (const auto& ext : extensions) 
    enabledExtensions.push_back(ext);

  // Create and set the properties of a vulkan instance
  VkInstanceCreateInfo instanceInfo = {};
  instanceInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext                = nullptr;
  instanceInfo.pApplicationInfo     = &appInfo;
  if (enabledExtensions.size() > 0) {
    instanceInfo.enabledExtensionCount   = 
      static_cast<uint32_t>(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();
  }
  return vkCreateInstance(&instanceInfo, nullptr, &Instance);
}

VkResult VulkanBasic::createDevice(VkDeviceQueueCreateInfo requestedQueues) {
  std::vector<const char*> enabledExtensions = 
    { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  VkDeviceCreateInfo deviceInfo   = {};
  deviceInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.pNext                = nullptr;
  deviceInfo.queueCreateInfoCount = 1;
  deviceInfo.pQueueCreateInfos    = &requestedQueues;
  deviceInfo.pEnabledFeatures     = nullptr;

  if (enabledExtensions.size() > 0) {
    deviceInfo.enabledExtensionCount =
      static_cast<uint32_t>(enabledExtensions.size());
    deviceInfo.ppEnabledExtensionNames = enabledExtensions.data();
  }
  return vkCreateDevice(PhysicalDevice, &deviceInfo, nullptr, &Device);
}

//---- Protected ------------------------------------------------------------//

void VulkanBasic::createCommandPool(uint32_t queueNodeId) {
  VkCommandPoolCreateInfo cmndPoolInfo = {};
  cmndPoolInfo.sType  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmndPoolInfo.flags  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  cmndPoolInfo.queueFamilyIndex = queueNodeId;
  VkResult result = vkCreateCommandPool(Device, &cmndPoolInfo, nullptr,
    &CmndPool);
  assert(!result && "Failed to create a command pool : Fatal Error\n");
}

void VulkanBasic::createCommandBuffers(uint32_t numBuffers) {
  DrawCmndBuffers.resize(numBuffers);

  // Add util for command buffer initialization
  VkCommandBufferAllocateInfo cmndBufferAllocInfo =
    vkutil::init::commandBufferAllocateInfo(
      CmndPool                                    , 
      VK_COMMAND_BUFFER_LEVEL_PRIMARY             , 
      static_cast<uint32_t>(DrawCmndBuffers.size())
    );

  VkResult error = vkAllocateCommandBuffers(Device, &cmndBufferAllocInfo,
                     DrawCmndBuffers.data());
  assert(!error && "Failed to allocate command buffers : Fatal Error\n");

  // Create a command buffer for submitting
  // a post present image memory barrier.
  cmndBufferAllocInfo.commandBufferCount = 1;

  error = vkAllocateCommandBuffers(Device, &cmndBufferAllocInfo, 
            &PpCmndBuffer);
  assert(!error && "Failed to allocate post present command buffer " 
         && "Fatal Error\n");
}

void VulkanBasic::destroyCommandBuffers() {
  vkFreeCommandBuffers(Device, CmndPool, 
    static_cast<uint32_t>(DrawCmndBuffers.size()), DrawCmndBuffers.data());
  vkFreeCommandBuffers(Device, CmndPool, 1, &PpCmndBuffer);
}

void VulkanBasic::findGraphicsQueue() {
  // Check that this has not been done already
  if (GraphicsQueueId > 0) return;
  uint32_t queueCount;

  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount,
    nullptr);
  assert(queueCount >= 1 && "No queue for physical device : Fatal Error\n");

  std::vector<VkQueueFamilyProperties> queueProperties;
  queueProperties.resize(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount,
    queueProperties.data());

  // Check if we can find a queue which supports graphics
  for (; GraphicsQueueId < queueCount; ++GraphicsQueueId) {
    if (queueProperties[GraphicsQueueId].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      break;
  }
  assert(GraphicsQueueId < queueCount && "No graphics queue found for " &&
    "physical device : Fatal Error\n");
}

void VulkanBasic::setPhysicalDevice() {
  assert(Instance != nullptr && "VkInstance not initialized before device "
         && "counting : Fatal Error\n");

  uint32_t deviceCount = 0;
  VkResult error       = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
                           nullptr);

  std::cout << error << "\n";
  assert(!error && "Error during device count : Fatal Error\n");
  assert(deviceCount > 0 && "No valid vulkan devices found : Fatal Error\n");

  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  error = vkEnumeratePhysicalDevices(Instance, &deviceCount, 
            physicalDevices.data());

  assert(!error && "Could not enumerate physical devices : Fatal Error\n");

  // Set the physical device to the first one in the list for now
  PhysicalDevice = physicalDevices[0];
}

void VulkanBasic::createSetupCommandBuffer() {
  if (SetupCmndBuffer != VK_NULL_HANDLE) {
    vkFreeCommandBuffers(Device, CmndPool, 1, &SetupCmndBuffer);
    SetupCmndBuffer = VK_NULL_HANDLE;
  }

  VkCommandBufferAllocateInfo cmndBufferAllocInfo =
    vkutil::init::commandBufferAllocateInfo(CmndPool,
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

  VkResult result = vkAllocateCommandBuffers(Device, &cmndBufferAllocInfo, 
                      &SetupCmndBuffer);
  assert(!result && "Failed to allocate command buffer : Fatal Error\n");
}

void VulkanBasic::flushSetupCommandBuffer() {
  VkResult result;

  if (SetupCmndBuffer == VK_NULL_HANDLE) 
    std::cerr << "Error : Attempt to flush setup buffer failed\n";

  result = vkEndCommandBuffer(SetupCmndBuffer); 
  assert(!result && "Failed to end setup command buffer : Fatal Error\n");

  VkSubmitInfo submitInfo = {};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &SetupCmndBuffer;

  result = vkQueueSubmit(Queue, 1, &submitInfo, VK_NULL_HANDLE);
  assert(!result && "Failed to submit buffer to queue : Fatal Error\n");
  
  result = vkQueueWaitIdle(Queue);
  assert(!result && "Failed to make queue wait idle\n");

  vkFreeCommandBuffers(Device, CmndPool, 1, &SetupCmndBuffer);
  SetupCmndBuffer = VK_NULL_HANDLE; // TODO: Check if necessary.
}


void VulkanBasic::setupDepthStencil(uint32_t width, uint32_t height) {
  VkResult result;

  VkImageCreateInfo imageInfo = {};
  imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.pNext             = nullptr;
  imageInfo.imageType         = VK_IMAGE_TYPE_2D;
  imageInfo.format            = DepthFormat;
  imageInfo.extent            = { width, height, 1 };
  imageInfo.mipLevels         = 1;
  imageInfo.arrayLayers       = 1;
  imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling            = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage             = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                              | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  imageInfo.flags             = 0;

  VkMemoryAllocateInfo memAllocInfo = {};
  memAllocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocInfo.pNext                = nullptr;
  memAllocInfo.allocationSize       = 0;
  memAllocInfo.memoryTypeIndex      = 0;

  VkImageViewCreateInfo viewInfo           = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.pNext                           = nullptr;
  viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format                          = DepthFormat;
  viewInfo.flags                           = 0;
  viewInfo.subresourceRange                = {};
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT 
                                           | VK_IMAGE_ASPECT_STENCIL_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.levelCount     = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;

  VkMemoryRequirements memRequirements;

  result = vkCreateImage(Device, &imageInfo, nullptr, &DepthStencil.image);
  assert(!result && "Failed to create image : Fatal Error\n");

  vkGetImageMemoryRequirements(Device, DepthStencil.image, &memRequirements);
  memAllocInfo.allocationSize = memRequirements.size;
  getMemoryType(memRequirements.memoryTypeBits,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);
  result = vkAllocateMemory(Device, &memAllocInfo, nullptr, 
             &DepthStencil.memory);
  assert(!result && "Failed to allocate memory for depth stencil : "
         && "Fatal Error\n");
  
  result = vkBindImageMemory(Device, DepthStencil.image, DepthStencil.memory,
             0);  
  assert(!result && "Failed to bind image memory for depth stencil : "
          && "Fatal Error\n");
  vkutil::transformImageLayout(SetupCmndBuffer, DepthStencil.image,
    VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  viewInfo.image = DepthStencil.image;
  result = vkCreateImageView(Device, &viewInfo, nullptr, &DepthStencil.view);
  assert(!result && "Failed to create depth stencil image view : "
         && "Fatal Error\n");
}

void VulkanBasic::createPipelineCache() {
  VkPipelineCacheCreateInfo pipelineCacheInfo = {};
  pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  VkResult result = vkCreatePipelineCache(Device, &pipelineCacheInfo, nullptr,
                      &PipelineCache);
  assert(!result && "Failed to create pipeline cache : Fatal Error\n");
}

void VulkanBasic::setupFramebuffers(uint32_t width, uint32_t height,
    const VkScBufferVec& swapchainBuffers) {
  VkImageView attachments[2];

  // The depth and stencil attachments are the same for all frame buffers.
  attachments[1] = DepthStencil.view;

  VkFramebufferCreateInfo frameBufferInfo = {};
  frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  frameBufferInfo.pNext            = nullptr;
  frameBufferInfo.renderPass       = RenderPass;
  frameBufferInfo.attachmentCount  = 2;
  frameBufferInfo.pAttachments     = attachments;
  frameBufferInfo.width            = width;
  frameBufferInfo.height           = height;
  frameBufferInfo.layers           = 1;

  // Create a famebuffer for each of the swapchain images
  FrameBuffers.resize(swapchainBuffers.size());
  for (size_t i = 0; i < swapchainBuffers.size(); ++i) {
    attachments[0]  = swapchainBuffers[0].view;
    VkResult result = vkCreateFramebuffer(Device, &frameBufferInfo, nullptr,
                        &FrameBuffers[i]);
    assert(!result && "Failed to create swapbuffer : Fatal Error\n");
  }
}

void VulkanBasic::setupRenderPass() {
  VkResult                result;
  VkAttachmentDescription attachments[2];

  // Setup the color attachment.
  attachments[0].format         = ColorFormat;
  attachments[0].samples        = VK_SAMPLE_COUNT_1_BIT;
  attachments[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[0].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  attachments[0].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Setup the depth attachment.
  attachments[1].format         = DepthFormat;
  attachments[1].samples        = VK_SAMPLE_COUNT_1_BIT;
  attachments[1].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[1].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[1].initialLayout  = 
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  attachments[1].finalLayout    = 
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorReference = {};
  colorReference.attachment = 0;
  colorReference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthReference = {};
  depthReference.attachment = 1;
  depthReference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassInfo    = {};
  subpassInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassInfo.flags                   = 0;
  subpassInfo.inputAttachmentCount    = 0;
  subpassInfo.pInputAttachments       = nullptr;
  subpassInfo.colorAttachmentCount    = 1;
  subpassInfo.pColorAttachments       = &colorReference;
  subpassInfo.pResolveAttachments     = nullptr;
  subpassInfo.pDepthStencilAttachment = &depthReference;
  subpassInfo.preserveAttachmentCount = 0;
  subpassInfo.pPreserveAttachments    = nullptr;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.pNext           = nullptr;
  renderPassInfo.attachmentCount = 2;
  renderPassInfo.pAttachments    = attachments;
  renderPassInfo.subpassCount    = 1;
  renderPassInfo.pSubpasses      = &subpassInfo;
  renderPassInfo.dependencyCount = 0;
  renderPassInfo.pDependencies   = nullptr;

  result = vkCreateRenderPass(Device, &renderPassInfo, nullptr, &RenderPass);
  assert(!result && "Failed to create render pass : Fatal Error\n");
}

//---- Private --------------------------------------------------------------//

VkBool32 VulkanBasic::getMemoryType(uint32_t typeBits, VkFlags properties,
    uint32_t* typeIndex) {
  for (uint32_t i = 0; i < 32; ++i) {
    if ((typeBits & 1) == 1) {
      if ((DeviceMemProps.memoryTypes[i].propertyFlags & properties) ==
           properties) {
        *typeIndex = i;
        return true;
      }
    }
    typeBits >>= 1;
  }
  return false;
}
