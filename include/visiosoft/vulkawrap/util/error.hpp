//---- include/visiosoft/vulkawrap/util/error.hpp ---------- -*- C++ -*- ----//
//---- Error handling functionality for Visiosoft VulkaWrap.             ----//
//
//                             Visiosoft VulkaWrap
//                          
//                        Copyright (c) 2016 Rob Clucas        
//                      Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   error.hpp
/// \brief  Defines error handling functionality for the VulkaWrap library, to
///         allow error handling and assertation functonality to be removed
///         using a configuration paramter when performance is required.
//
//---------------------------------------------------------------------------//

#ifndef VISIOSOFT_VULKAWRAP_UTIL_ERROR_HPP
#define VISIOSOFT_VULKAWRAP_UTIL_ERROR_HPP

#include "visiosoft/vulkawrap/config/config.hpp"
#include "visiosoft/vulkawrap/io.h"
#include <type_traits>

namespace vs {

using ErrorHandlingType  = uint8_t;
using AssertHandlingType = uint8_t;

namespace util   {
namespace detail {

/// Metafunction to test if error handling is enabled.
///
/// \tparam HandlingType The type of error handling which is set -- this is
///         either config::ErrorHandlingOn or config::ErrorHandlingOff.
template <ErrorHandlingType HandlingType> 
struct error_handling_enabled {
  static constexpr bool value = HandlingType;
};

/// Metafunction to test if assert handling is enabled.
///
/// \tparam HandlingType The type of assert handling which is set -- this is 
///         either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType HandlingType>
struct assert_handling_enabled {
  static constexpr bool value = HandlingType;
};

} // namespace detail

/// Function which assert a condition, and takes an optinal message to describe
/// the error. If the assertation fails the program exits.
///
/// \param  condition      The condition to assert.
/// \param  message        The optional message to print if the assertation
///         fails.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = vwrap::config::AssertHandlingCx>
typename std::enable_if<
  detail::assert_handling_enabled<AssertHandling>::value, void
>::type
assert(bool condition, const std::string& message = "",
    const std::string file = "", int line = 0) {
  if (!condition) {
    std::cerr << "Failure at         : " << file    << " : " << line << ".\n" 
              << "Additional message : " << message << ".\n\n";
    exit(EXIT_FAILURE);
  }
}

/// Function which asserts a condition, and takes an optional message to
/// describe the error. This instance of the function removes the assertation
/// checking code for when additional performance is required in place of
/// checking for correctness.
///
/// \param  condition      The condition to assert.
/// \param  message        The optinal messge to print if the assertation
///         fails.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = vwrap::config::AssertHandlingCx>
typename std::enable_if<
  !detail::assert_handling_enabled<AssertHandling>::value, void
>::type
assert(bool condition, const std::string& message = "", 
    const std::string& file = "", int line = 0) {
  // Does noting so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

#define Assert(condition, message)                                            \
  assert(condition, message, __FILE__, __LINE__)

} // namespace util

namespace vwrap {
namespace util  {

/// Function which asserts that the result of a vulkan operation was a success,
/// and takes an optional message to print when the assertation fails.
///
/// \param  result         The result to assert for success.
/// \param  message        The optinal message to print when the success 
///         assertation fails.
/// \param  file           The file where the assertation is checked.
/// \param  line           The line in the file where the assertation is 
///         checked.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = config::AssertHandlingCx>
typename std::enable_if<
  vs::util::detail::assert_handling_enabled<AssertHandling>::value, void
>::type 
assertSuccess(VkResult result, const std::string& message = "",
    const std::string& file = "", int line = 0) {
  if (result != 0) {
    std::cerr << "Failure at         : " << file << " : " << line << ".\n"
              << "Error code         : ";
    io::printVulkanResult(result);
    std::cerr << "Additional message : " << message << "\n";
    exit(EXIT_FAILURE);
  }
}

/// Function which asserts that the result of a vulkan operation was a success,
/// and takes an optional message to print when the assertation fails.
///
/// \param  result         The result to assert for success.
/// \param  message        The optinal message to print when the success 
///         assertation fails.
/// \param  file           The file where the assertation is checked.
/// \param  line           The line in the file where the assertation is 
///         checked.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = config::AssertHandlingCx>
typename std::enable_if<
  !vs::util::detail::assert_handling_enabled<AssertHandling>::value, void
>::type
assertSuccess(VkResult result, const std::string& message = "",
    const std::string file = "", int line = 0) {
  // Does nothing so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

#define AssertSuccess(result, message)                                        \
  assertSuccess(result, message, __FILE__, __LINE__)
 
} // namespace util
} // namespace vwrap
} // namespace vs 

#endif  // VISIOSOFT_VULKAWRAP_UTIL_ERROR_HPP

