//---- src/vulkan/vulkan_base.h - Provides minimal vulkan functionality -----//
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
// \file   vulkan_base.h
// \brief  Defines a class with minimal basic functionality -- this is designed
// to easily specify the types of devices and queues which each of the device
// types must have.
//---------------------------------------------------------------------------//

#ifndef VULKAN_VW_BASE_H
#define VULKAN_VW_BASE_H

#include "vulkan_wrapper.h"

// Base wrapper class for vullkan which lets the device type and the queues
// which each device must suppot be specified.
class VulkanBase {
 public:
  // Constructor which:
  //   - Creates a Vulkan Instance
  //   - Enumerates Physical Devices
  //
  // \param deviceSpecifiers            The specifiers for the properties of 
  // each device.
  // \param devicesMustSupportAllQueues Devices are only added if the device
  // supports all the requested queues, and not only some of them.
  // \param appName                     The name of the vulkan application for 
  // this instance.
  // \param engineName                  The name of the engine for this 
  // application.   
  // \param extensions                  The vulkan extensions to use.
  VulkanBase(const VwDeviceSpecVec& deviceSpecifiers,
    bool devicesMustSupportAllQueues          = true                      , 
    const char* appName                       = ""                        , 
    const char* engineName                    = ""                        ,
    const std::vector<const char*>&extensions = std::vector<const char*>{});

  // Gets a vulkan physical device from the available physical devices.
  //
  // \param deviceIdx The index of the device to set.
  VkPhysicalDevice getVkPhysicalDevice(size_t deviceIdx) const {
    return PhysicalDevices[deviceIdx].device;
  }

  // Gets a vulkan wrapper physical device.
  //
  // \param deviceIdx The index of the wrapper physical device to get.
  VwPhysicalDevice getVwPhysicalDevice(size_t deviceIdx) const {
    return PhysicalDevices[deviceIdx];
  }

  protected:
   VkInstance         Instance;                 // Store per-application state.
   VwPhysDeviceVec    PhysicalDevices;          // CPUs | GPUs for vulkan.

  private:
   // Creates the vulkan instance.
   //
   // \param appName    The name of the vulkan application.
   // \param engineName The name of the engine application.
   // \param extensions The extensions to enable for the instance.
   VkResult createInstance(const char* appName, const char* engineName,
     const std::vector<const char*>& extensions);

  // Gets all the physical devices which meet the requirements specified.
  //
  // \param deviceSpecifiers            A vector of device specifiers.
  // \param devicesMustSupportAllQueus  If the device must only be added if all
  // of the queues in the VwDeviceSpecifer are found.
  void getPhysicalDevices(const VwDeviceSpecVec& deviceSpecifiers,
                          bool devicesMustSupportAllQueues);
};

#endif  // VULKAN_VULKAN_VULKAN_BASE_H
