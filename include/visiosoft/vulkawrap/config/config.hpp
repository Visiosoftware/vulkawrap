//---- include/visiosoft/vulkawrap/config/config.hpp ------- -*- C++ -*- ----//
//---- Configuration file for Visiosoft Vulkawrap library.               ----//
//
//                           Visiosoft VulkaWrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   config.hpp
/// \brief  Defines the configuration parameters for the build.
//
//---------------------------------------------------------------------------//

#ifndef VISIOSOFT_VULKAWRAP_CONFIG_CONFIG_HPP
#define VISIOSOFT_VULKAWRAP_CONFIG_CONFIG_HPP

#include "definitions.h"

namespace vs     {
namespace vwrap  {
namespace config {

/// Allows or removes assert checking code, it can be:
///   Enabled  - Enables assert cheecking code.
///   Disabled - Removes all assert checking code for performance
static constexpr uint8_t AssertHandlingCx = EnabledCx;

/// Allows or removes error handling code, it can be:
///   Enabled  - Enables error handling code.
///   Disabled - Removes all error handling code for performance.
static constexpr uint8_t ErrorHandlingCx = EnabledCx;

}  // namespace config
}  // namespace vwrap
}  // namespace vs

#endif  // VISIOSOFT_VULKAWRAP_CONFIG_CONFIG_HPP


