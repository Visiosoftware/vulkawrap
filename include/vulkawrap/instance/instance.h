//---- include/vulkawrap/instance/instance.h --------------- -*- C++ -*- ----//
//
//                                Vulkawrap
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
///        instance with a more modern, cleaner C++ interface.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_INSTANCE_INSTANCE_H
#define VULKAWRAP_INSTANCE_INSTANCE_H

#include "vulkawrap/util/assert.hpp"
#include <vulkan/vulkan.h>
#include <atomic>
#include <memory>
#include <vector>

namespace vwrap  {

//---- Forward Declataions --------------------------------------------------//

class ConcurrentReferenceCounter;
class NonConcurrentReferenceCounter;

template <typename ReferenceCounterType>
class SharedInstance;

namespace detail {
 struct Instance;
} // namespace detail

//---- Aliases --------------------------------------------------------------//

/// Alias for a concurrent shared instance.
using ConcurrentSharedInstance =
  SharedInstance<ConcurrentReferenceCounter>;

/// Alias for a non-concurrent shared instance.
using NonConcurrentSharedInstance =
  SharedInstance<NonConcurrentReferenceCounter>;

/// Alias for a unique Instance, for when complete
/// control over the instance is required.
using UniqueInstance = std::unique_ptr<detail::Instance>;

//---- Implementations ------------------------------------------------------//

/// Reference counting class. This implementation is thread safe and can
/// be used concurrently, but incurrs the overhead that the cuncurrent 
/// incrementation and decrementation brings.
class ConcurrentReferenceCounter {
 public:
  /// Initializes the count.
  void initialize() {
    Count.store(1);
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
    return Count.load();
  }

 private:
  std::atomic<uint32_t> Count;  //!< The number of references.
};

/// Reference counting class. This implementation is not thread safe, and is
/// provided for the case that thread safety is not required and the additional
/// performance gained by removing the thread-safe functionality is justified.
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

namespace detail {

/// Wrapper around a Vulkan Instance with a cleaner interface, and automatic
/// resource handling of the instance. This is designed as an implementation
/// detail class which should be further wrapped by an instance couning
/// classes, to provide shared and unique instance functionality.
struct Instance {
  VkInstance vkInstance;  //!< The vulkan instance which is being wrapped.

  /// Constructor to create an Instance.
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
};

} // namespace detail 

/// A shared Instance, so that a single vulkan instance can be used multiple
/// times, and we can make sure that the instance is never cleaned if there is
/// a pointer to it.
///
/// \tparam RefCounter The reference counter type -- this can be an atomic or a
///         non-atomic counter, deoending on what is required. 
template <typename RefCounter>
class SharedInstance {
 public:
  /// Constructor to create a shared instance. This initializes
  /// the instance count, creating a completelty new instance.
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
  /// \param otherInstance The other shared instance to create this shared
  /// instance from.
  SharedInstance(const SharedInstance<RefCounter>& otherInstance)
  :   InstanceCounter(otherInstance.InstanceCounter),
      VulkanInstance(otherInstance.VulkanInstance) {
    InstanceCounter->increment();
  }

  /// Gets the vulkan instance. This is designed as an accessor, so that the
  /// class can be used to provide a raw Vulkan instance to functions which
  /// require a raw vulkan instance. If this is used to get the raw Vulkan
  /// instance which is then copied, the counting functionality is then broken
  /// since when the copy is made for the raw Vulkan instance, no counting
  /// functionality is invoked.
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

#endif  // VULKAWRAP_INSTANCE_INSTANCE_H

