//---- include/visiosoft/vulkawrap/instnace/instance.h ----- -*- C++ -*- ----//
//---- Provides functionality for selecting vulkan devices.              ----//
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
/// \file  instance.h
/// \brief Defines an instance class which is a wrapper around a Vulkan
///        with a cleaner interface.
//
//---------------------------------------------------------------------------//

#ifndef VISIOSOFT_VULKAWRAP_INSTANCE_INSTANCE_H
#define VISIOSOFT_VULKAWRAP_INSTANCE_INSTANCE_H

#include "visiosoft/vulkawrap/util/error.hpp"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

//---- Forward Declataions --------------------------------------------------//
namespace vs     {

class ConcurrentReferenceCounter;
class NonConcurrentReferenceCounter;

namespace vwrap  {

template <typename ReferenceCounterType>
class SharedInstance;

} // namespace vwrap
} // namespace vs

//---- Aliases --------------------------------------------------------------//

namespace vs    {
namespace vwrap {

/// Alias for a concurrent shared instance.
using ConcurrentSharedInstance =
  SharedInstance<ConcurrentReferenceCounter>;

/// Alias for a concurrent shared instance.
using NonConcurrentSharedInstance =
  SharedInstance<NonConcurrentReferenceCounter>;

} // namespace vwrap
} // namespace vs

//---- Implementation -------------------------------------------------------//

namespace vs    {

/// Reference counting class. This implementation is thread safe and can be
/// concurrently, but incurrs the overhead that the cuncurrent incrementation
/// decrementation brings.
class ConcurrentReferenceCounter {

};

/// Reference counting class. This implementation is not thread safe, and is
/// provided for the case that thread safety is not required and the additional
/// performance gained by not including thread safety is desired.
class NonConcurrentReferenceCounter {
 public:
  /// Initializes the count.
  void initialize() {
    Count = 1;
  }

  /// Increments the reference count.
  void increment() { 
    ++Count;
  }

  /// Decrements the reference count.
  void decrement() {
    --Count;
  }

  /// Gets the reference count.
  uint32_t count() const {
    return Count;
  }

 private:
  uint32_t Count;  //!< The number of references.
};

namespace vwrap {

//---- Forward Declarations -------------------------------------------------//

namespace detail {
 struct Instance;
}

//---- Aliases --------------------------------------------------------------//

/// Alias for a unique Instance type. Instances should be unique, hence the
/// wrapper around std::unique_ptr for an instance.
///
/// At a later stage additional supoprt may be added to allow UniqueInstances
/// to be created from other UniqueInstances.
using UniqueInstance = std::unique_ptr<detail::Instance>;

namespace detail {

/// Wrapper around a Vulkan with a cleaner interface with automatic destruction
/// when the instance goes out of scope. This class should be used as a
/// std::unique_ptr or an std::shared_ptr, depending on what ir required.
class Instance {
 public:
  /// Constructor to create an instance wrapper.
  ///
  /// \param appName    The name of the application for this instance.
  /// \param engineName The name of the engine for this application.
  /// \param extensions The vulkan extensions to use.
  /// \param layers     The layers which must be enabled.
  /// \param apiVersion The version of the vulkan API to use.
  Instance(
    const char* appName                        = ""                        , 
    const char* engineName                     = ""                        ,
    const std::vector<const char*>& extensions = std::vector<const char*>{},
    const std::vector<const char*>& layers     = std::vector<const char*>{},
    uint32_t apiVersion                        = VK_MAKE_VERSION(1, 0, 2)
  );

  // Destructor to destroy the instance when it goes out of scope.     
  ~Instance() {
     vkDestroyInstance(vkInstance, nullptr);
   } 

  VkInstance vkInstance;  //!< The vulkan instance which is being wrapped.
};

} // namespace detail 

/// Shared instance, so that a single vulkan instance can be used multiple
/// times, and we can make sure that the instance is never cleaned if there is
/// a pointer to it.
///
/// \tparam RefCounter The reference counter type -- this can be an atomic or a
///         non-atomic counter, deoending on what is required. 
template <typename RefCounter>
class SharedInstance {
 public:
  /// Constructor to create a shared instance. This initializes the instnace
  /// count, creating a completelty new instance.
  ///
  /// \param appName    The name of the application for this instance.
  /// \param engineName The name of the engine for this application.
  /// \param extensions The vulkan extensions to use.
  /// \param layers     The layers which must be enabled.
  /// \param apiVersion The version of the vulkan API to use.
  SharedInstance(
    const char* appName                        = ""                        , 
    const char* engineName                     = ""                        ,
    const std::vector<const char*>& extensions = std::vector<const char*>{},
    const std::vector<const char*>& layers     = std::vector<const char*>{},
    uint32_t apiVersion                        = VK_MAKE_VERSION(1, 0, 2)
  );

  /// Copy constructor, to create a SharedInstance from another SharedInstance.
  /// This will result in both the shared instances having the same Vulkan
  /// Instance handle.
  ///
  /// \param sharedInstance The other shared instance to create this shared
  /// instance from.
  SharedInstance(const SharedInstance<RefCounter>& otherInstance)
  :   InstanceCounter(otherInstance.InstanceCounter),
      VulkanInstance(otherInstance.VulkanInstance) {
    InstanceCounter->increment();
  }

  /// Gets the vulkan instance. This could be used to create other Vulkan
  /// instances, in which case the counter would not work, since there would be
  /// an uncounted vulkan instance. However, if the function is only used to
  /// as a way to access the vulkan instance, they there will be no problem.
  VkInstance getVkInstance() const {
    return VulkanInstance;
  }
 
 private:
  RefCounter*     InstanceCounter;    //!< The counter for the instances.
  VkInstance      VulkanInstance;     //!< The vulkan instance being shared.
};

//---- Implementation -------------------------------------------------------//

template <typename RefCounter>
SharedInstance<RefCounter>::SharedInstance(
    const char* appName                                                   , 
    const char* engineName                                                ,
    const std::vector<const char*>& extensions                            ,
    const std::vector<const char*>& layers                                ,
    uint32_t apiVersion                                                   ) 
:   InstanceCounter(new RefCounter()) {
  InstanceCounter->initialize();

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
  VkResult result = vkCreateInstance(&instanceInfo, nullptr, &VulkanInstance);
  util::AssertSuccess(result, "Failed to create instance.\n");
}

/// Wrapper around make_unique specifically for instances. Returns a
/// UniqueInstance.
///
/// \param  arg  The first argument.
/// \param  args The rest of the arguments.
/// \tparam Arg  The type the first argument.
/// \tparam Args The types of the rest of the arguments.
template <typename Arg, typename... Args>
static UniqueInstance makeUniqueInstance(Arg arg, Args... args) {
  return std::make_unique<detail::Instance>(
           std::forward<Arg&&, Args&&...>(arg, args...)
         );
}

static UniqueInstance makeUniqueInstance() {
  return std::make_unique<detail::Instance>();
}

} // namespace vwrap
} // namespace vs 

#endif  // VISIOSOFT_VULKAWRAP_INSTANCE_INSTANCE_H

