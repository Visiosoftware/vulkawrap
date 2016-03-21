//---- include/vulkawrap/config/config.hpp ----------------- -*- C++ -*- ----//
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
/// \file   config.hpp
/// \brief  Defines the configuration parameters for the build.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_CONFIG_CONFIG_HPP
#define VULKAWRAP_CONFIG_CONFIG_HPP

#include "definitions.h"

namespace vwrap  {
namespace config {

/// Allows or removes assert checking code, it can be:
///   EnabledCx  - Enables assert checking code.
///   DisabledCx - Removes all assert checking code.
///   TestingCx  - Enables testing specializations of classes and functions.
static constexpr uint8_t AssertHandlingCx = TestingCx; 

/// Allows or removes error handling code, it can be:
///   EnabledCx  - Enables error handling code.
///   DisabledCx - Removes all error handling code.
///   TestingCx  - Enables testing specializations of classes and functions.
static constexpr uint8_t ErrorHandlingCx = TestingCx;

}  // namespace config
}  // namespace vwrap

#endif  // VULKAWRAP_CONFIG_CONFIG_HPP


