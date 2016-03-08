//---- include/visiosoft/vulkawrap/config/definitions.h ---- -*- C++ -*- ----//
//---- Configuration related efinitions for the Visiosoft VulkaWrap      ----//
//---- library.                                                          ----//
//
//                          Visiosoft VulkaWrap
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
///         file to define the configuration for the build.
//
//---------------------------------------------------------------------------//

#ifndef VISIOSOFT_VULKAWRAP_CONFIG_DEFINITIONS_H
#define VISIOSOFT_VULKAWRAP_CONFIG_DEFINITIONS_H

namespace vs     {
namespace vwrap  {
namespace config {

/// Definition which can be used to specify disabling functionality.
static constexpr uint8_t DisabledCx = 0;

/// Definition which can be used to specify enabling functionality.
static constexpr uint8_t EnabledCx  = 1;

} // namespace config
} // namespace vwrap
} // namespace vs

#endif  // VISIOSOFT_VULKAWRAP_CONFIG_DEFINITIONS_H


