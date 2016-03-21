//---- include/vulkawrap/config/definitions.h -------------- -*- C++ -*- ----//
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
/// \brief  Defines configuration constants which can be used in the config.hpp 
///         file to define configuration properties for the build.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_CONFIG_DEFINITIONS_H
#define VULKAWRAP_CONFIG_DEFINITIONS_H

#include <cstdint>

namespace vwrap  {
namespace config {

/// Definition which can be used to specify disabling functionality.
static constexpr uint8_t DisabledCx = 0;

/// Definition which can be used to specify enabling functionality.
static constexpr uint8_t EnabledCx  = 1;

/// Definition which can be used to enable testing functionality for functions
/// which are difficult to test but could allow a specific testing version.
static constexpr uint8_t TestingCx  = 2;

} // namespace config
} // namespace vwrap

#endif  // VULKAWRAP_CONFIG_DEFINITIONS_H


