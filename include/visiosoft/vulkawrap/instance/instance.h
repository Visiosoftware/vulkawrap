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

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace vs    {
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
struct Instance {
  VkInstance vkInstance;  //!< The vulkan instance which is being wrapped.

  /// Constructor to create an instance.
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

