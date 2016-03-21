//---- include/vulkawrap/util/testing.hpp ------------------ -*- C++ -*- ----//
//
//                                 Vulkawrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                   https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   testing.hpp
/// \brief  Defines functionality to help with testing the library.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_UTIL_TESTING_HPP 
#define VULKAWRAP_UTIL_TESTING_HPP

#include "vulkawrap/config/definitions.h"

namespace vwrap {
namespace util  {
namespace test  {

/// Metafunction to test if testing is enabled.
///
/// \tparam ConfigValue The cofiguration value to check against testing.
template <uint8_t ConfigValue>
struct testing_enabled {
  static constexpr bool value = ConfigValue == config::TestingCx;
};

} // namespace test
} // namespace util
} // namespace vwrap

#endif  // VULKAWRAP_UTIL_TESTING_HPP