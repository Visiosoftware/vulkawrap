//---- src/vulkan/vulkan_basic.h - Class which does basic vulkan setup ------//
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
// \file   vulkan_basic.h
// \brief  Defines a class whith basic vulkan functionality.
//---------------------------------------------------------------------------//

#ifndef VULKAN_VULKAN_VULKAN_BASIC_H
#define VULKAN_VULKAN_VULKAN_BASIC_H

#include <vulkan/vulkan.h>
#include <vector>

// Defines the possible command buffers to atrt recording
namespace buffers {

const constexpr char* setupBuffer = "Setup Buffer";

} // namespace buffers 

// Class which does some basic setup for vulkan.
class VulkanBasic {
 public:
   // Alias for a command buffer container.
   using CmndBufferVec = std::vector<VkCommandBuffer>;

   // Constructor which:
   //  - Creates a VulkanInstance
   //  - Enumerates physical devices and uses the first one
   //  - Finds a graphics queue
   //  - Creates the logical device -- view of physical device
   //  - Sets the device memory properties
   //
   // Any fatal errors terminate the application, non terminal errors are left
   // alone for the moment
   VulkanBasic();

   // Sets up the basic vulkan functionality.
   //
   // \param width       The width of the screen (in pixels).
   // \param height      The hight of the screen (in pixels).
   // \param queueNodeId The index of the node of the queue to use.
   // \param numBuffers  The number of command buffers to create.
   void setup(uint32_t width, uint32_t height, uint32_t queueNodeId, 
     uint32_t numBuffers);

   // Makes a command buffer start recording.
   //
   // \param bufferType The type of command buffer to start recording for. The
   // buffer type can be any of the buffers defined in the buffers namespace.
   void startBufferRecording(const char* bufferType);

   // Creates the vulkan instance.
   //
   // param: appName    The name of the vulkan application
   // param: engineName The name of the vulkan engine
   // param: extensions The extentions which should be used. 
   VkResult createInstance(const char* appName = "", const char* engineName = "",
    const std::vector<const char*>& extensions = std::vector<const char*>{});

   // Creates a vulkan device
   //
   // param: requestedQueues The queues to use
   VkResult createDevice(VkDeviceQueueCreateInfo requestedQueues);

 protected:
  uint32_t         GraphicsQueueId; // The index of the graphics queue 
  VkInstance       Instance;        // Stores per-application state
  VkPhysicalDevice PhysicalDevice;  // Device (CPU | GPU) for vulkan to use
  VkPhysicalDeviceMemoryProperties
                   DeviceMemProps;  // Memory properties of the device
  VkDevice         Device;          // Logical device -- application's view
  VkQueue          Queue;           // Handle to device graphics queue
  VkFormat         DepthFormat;     // Format for depth buffer.
  VkFormat         ColorFormat;     // Format for the color buffer.
  VkCommandPool    CmndPool;        // Command buffer pool.
  VkCommandBuffer  DrawCmndBuffer;  // Command buffer for setup.
  VkCommandBuffer  PpCmndBuffer;    // For submitting a post present barrier.
  VkCommandBuffer  SetupCmndBuffer; // Command buffer for setup.
  CmndBufferVec    DrawCmndBuffers; // Command buffers for rendering.

  // Defines a struct for a depth stencil.
  struct {
    VkImage        image;
    VkImageView    view;
    VkDeviceMemory memory;
  } DepthStencil;

  // Finds a queue which supports graphics operations
  //
  // Postconditions : A graphics queue is found
  void findGraphicsQueue();

  // Sets the physical device to the first available one -- edit this to 
  // provide more functionality.
  //
  // Preconditions : VkInstance needs to have been initialized
  void setPhysicalDevice();

  // Creates the command buffers.
  //
  // \param numBuffers The number of buffers to create.
  void createCommandBuffers(uint32_t numBuffers);

  // Destroys the command buffers.
  void destroyCommandBuffers();

  // Creates a new command pool to store command buffers.
  //
  // \param queueNodeId The index of the queue node to use.
  void createCommandPool(uint32_t queueNodeId);

  // Creates the command buffer for setup commands.
  void createSetupCommandBuffer();

  // Sets up the depth stencil.
  //
  // \param width The width of the image in the depth dtencil.
  // \param height The hright of the image in the depth stencil.
  void setupDepthStencil(uint32_t width, uint32_t height);

 private:
  // Gets a type of memory for vulkan -- returns truw if the requested memory
  // type is found.
  //
  // \param typeBits The bits for the memory type information.
  // \param properties The properties of the memory allocation.
  // \param typeindex The index of the requested memory type.
  VkBool32 getMemoryType(uint32_t typeBits, VkFlags properties, 
    uint32_t* typeIndex);
};

#endif // VULKAN_VULKAN_VULKAN_BASIC_H
