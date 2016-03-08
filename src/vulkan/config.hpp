//---- src/vulkan/config.hpp - Configuration file for vulkan  ---------------//
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
/// \file   config.hpp
/// \brief  Defines the configuration parameters for the build. Setting
/// certain parameters such as allowing error checking, etc.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_CONFIG_HPP
#define VULKAN_CONFIG_HPP

namespace vs {
namespace config {

//---- Constants ------------------------------------------------------------//

static constexpr uint8_t Disabled = 0;
static constexpr uint8_t Enabled  = 1;

/// Allows or removes assert checking code, it can be:
///   Enabled  - Enables assert cheecking code.
///   Disabled - Removes all assert checking code for performance
static constexpr uint8_t AssertHandlingSupport = Enabled;

/// Allows or removes error handling code, it can be:
///   Enabled  - Enables error handling code.
///   Disabled - Removes all error handling code for performance.
static constexpr uint8_t ErrorHandlingSupport = Enabled;

}  // namespace config
}  // namespace vs

#endif  // VULKAN_CONFIG_HPP


