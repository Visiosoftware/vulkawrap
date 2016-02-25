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

// Class which does some basic setup for vulkan.
class VulkanBasic {
 public:
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

 private:
   uint32_t         GraphicsQueueId;// The index of the graphics queue 
   VkInstance       Instance;       // Stores per-application state
   VkPhysicalDevice PhysicalDevice; // Device (CPU | GPU) for vulkan to use
   VkPhysicalDeviceMemoryProperties
                    DeviceMemProps; // Memory properties of the device
   VkDevice         Device;         // Logical device -- application's view
   VkQueue          Queue;          // Handle to device graphics queue

   // Finds a queue which supports graphics operations
   //
   // Postconditions : A graphics queue is found
   void findGraphicsQueue();

   // Sets the physical device to the first available one -- edit this to
   // provide more functionality.
   //
   // Preconditions : VkInstance needs to have been initialized
   void setPhysicalDevice();
};

#endif // VULKAN_VULKAN_VULKAN_BASIC_H
