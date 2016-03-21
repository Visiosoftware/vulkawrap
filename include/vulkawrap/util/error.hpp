//---- include/vulkawrap/util/error.hpp -------------------- -*- C++ -*- ----//
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
/// \file   error.hpp
/// \breif  Defines error handling functionality for Vulkawrap. The error
///         handling functionality is designed to be enabled/disabled in
///         the configuration file visiosoft/config/config/.hpp.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_UTIL_ERROR_HPP
#define VULKAWRAP_UTIL_ERROR_HPP

#include "vulkawrap/config/config.hpp"
#include <type_traits>

namespace vwrap {

/// Alias for an Error Handling Type 
/// to make the code more readable.
using ErrorHandlingType  = uint8_t;

namespace util   {
namespace detail {

/// Metafunction to test if error handling is enabled.
///
/// \tparam HandlingType The type of error handling which is set -- this is
///         either config::ErrorHandlingOn or config::ErrorHandlingOff.
template <ErrorHandlingType HandlingType> 
struct error_handling_enabled {
  static constexpr bool value = HandlingType == config::EnabledCx;
};

} // namespace detail
} // namespace util
} // namespace vwrap

#endif  // VULKAWRAP_UTIL_ASSERT_HPP

