//---- src/visiosoft/vulkawrap/instnace/instance.h --------- -*- C++ -*- ----//
//---- Implementation of functionality for selecting vulkan devices.     ----//
//
//                           Visiosoft Vulkawrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file  instance.cc
/// \brief Implementation of the instance class which is a wrapper around a 
///        Vulkan with a cleaner interface.
//
//---------------------------------------------------------------------------//

#include "visiosoft/vulkawrap/instance/instance.h"
#include "visiosoft/vulkawrap/util/error.hpp"

namespace vs     { 
namespace vwrap  {
namespace detail {

Instance::Instance(const char* appName, const char* engineName, 
    const std::vector<const char*>& extensions                , 
    const std::vector<const char*>& layers                    ,
    uint32_t apiVersion                                       ) {
  // Set the application properties.
  VkApplicationInfo appInfo = {};
  appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName  = appName;
  appInfo.pEngineName       = engineName;
  appInfo.apiVersion        = apiVersion;

  // Create a vector with the default extensions 
  // and then add the user defined extensions.
  std::vector<const char*> enabledExtensions = 
    { VK_KHR_SURFACE_EXTENSION_NAME }; 
  for (const auto& extension : extensions) 
    enabledExtensions.push_back(extension);

  // Create and set the properties for a vulkan instance.
  VkInstanceCreateInfo instanceInfo = {};
  instanceInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext                = nullptr;
  instanceInfo.pApplicationInfo     = &appInfo;

  if (enabledExtensions.size() > 0) {
    instanceInfo.enabledExtensionCount   = 
      static_cast<uint32_t>(enabledExtensions.size());
    instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();
  }

  if (layers.size() > 0) {
    instanceInfo.enabledLayerCount   = static_cast<uint32_t>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
  }
  VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vkInstance);  
  util::AssertSuccess(result, "Failed to create instance.\n");
}

} // namespace detail
} // namespace vwrap
} // namespace vs


