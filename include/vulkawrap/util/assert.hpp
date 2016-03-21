//---- include/vulkawrap/util/assert.hpp ------------------- -*- C++ -*- ----//
//
//                                 Vulkawrap
//                          
//                        Copyright (c) 2016 Rob Clucas        
//                      Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   assert.hpp
/// \breif  Defines assert handling functionality for Vulkawrap. The assert
///         handling functionality is designed to be enabled/disabled in
///         the configuration file vulkawrap/config/config/.hpp.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_UTIL_ASSERT_HPP
#define VULKAWRAP_UTIL_ASSERT_HPP

#include "testing.hpp"
#include "vulkawrap/io.h"
#include "vulkawrap/config/config.hpp"
#include <vulkan/vulkan.h>
#include <iostream>
#include <string>
#include <type_traits>

//---- Forward Declaration --------------------------------------------------//

namespace vwrap {

/// Alias for the specifier for an Assert Handling
/// Type to make the code read more clearly.
using AssertHandlingType = uint8_t;

namespace util  {

void assert(bool, const std::string&, const std::string&, int);
void assertSuccess(VkResult, const std::string&, const std::string&, int); 

//---- Macros -----------------------------------------------------------------

#define Assert(condition, message)                  \
  assert<vwrap::config::AssertHandlingCx>(          \
    condition, message, __FILE__, __LINE__)

#define AssertSuccess(condition, message)           \
  assertsSuccess<vwrap::config::AssertHandlingCx>(  \
    condition, message, __FILE__, __LINE__)

//---- Implementations ------------------------------------------------------//

namespace detail {

/// Metafunction to test if assert handling is enabled.
///
/// \tparam HandlingType The type of assert handling which is set.
template <AssertHandlingType HandlingType>
struct assert_handling_enabled {
  static constexpr bool value = HandlingType == config::EnabledCx;
};

/// Metafunction to test if assert handling is disbled.
///
/// \tparam HandlingType The type of assert handling which is set.
template <AssertHandlingType HandlingType>
struct assert_handling_disabled {
  static constexpr bool value = HandlingType == config::DisabledCx;
};

} // namespace detail

/// Function which asserts a condition, and takes an optinal message to 
/// describe the error. If the assertation fails the program exits.
///
/// \param  condition      The condition to assert.
/// \param  message        The optional message to print if the assertation
///         fails.
/// \param  file           The file where the assertation is.
/// \param  line           The line in the file where the assertation is.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = config::AssertHandlingCx>
typename std::enable_if<
  detail::assert_handling_enabled<AssertHandling>::value, void
>::type
inline assert(bool condition, const std::string& message = "",
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
/// \param  file           The file where the assertation is.
/// \param  line           The line in the file where the assertation is.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = config::AssertHandlingCx>
typename std::enable_if<
  detail::assert_handling_disabled<AssertHandling>::value, void
>::type
inline assert(bool condition, const std::string& message = "", 
    const std::string& file = "", int line = 0) {
  // Does nothing so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

/// Function which asserts a condition, and takes an optional message to
/// describe the error. This instance of the function removes the assertation
/// checking code and just writes the message to the error buffer for testing 
/// purposes.
///
/// \param  condition      The condition to assert.
/// \param  message        The optinal messge to print if the assertation
///         fails.
/// \param  file           The file where the assertation is.
/// \param  line           The line in the file where the assertation is.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = config::AssertHandlingCx>
typename std::enable_if<
  test::testing_enabled<AssertHandling>::value, void
>::type
inline assert(bool condition, const std::string& message = "", 
    const std::string& file = "", int line = 0) {
  // Just writes a message, but doesn't assert.
  if (!condition) {
    std::cerr << "Failure at         : " << file    << " : " << line << ".\n" 
              << "Additional message : " << message << ".\n\n";
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
  detail::assert_handling_enabled<AssertHandling>::value, void
>::type 
inline assertSuccess(VkResult result, const std::string& message = "",
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
  detail::assert_handling_disabled<AssertHandling>::value, void
>::type
inline assertSuccess(VkResult result, const std::string& message = "",
    const std::string file = "", int line = 0) {
  // Does nothing so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

/// Function which asserts that the result of a vulkan operation was a success,
/// and takes an optional message to print when the assertation fails. This
/// instance just writes the error to the output buffer for testing.
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
  test::testing_enabled<AssertHandling>::value, void
>::type
inline assertSuccess(VkResult result, const std::string& message = "",
    const std::string file = "", int line = 0) {
  // Does nothing so that when this instance of the assert is called, the
  // compiler can optimize it out ...
   // Just writes a message, but doesn't assert.
  if (result != 0) {
    std::cerr << "Failure at         : " << file << " : " << line << ".\n"
              << "Error code         : ";
    io::printVulkanResult(result);
    std::cerr << "Additional message : " << message << "\n";
  } 
}

} // namespace util
} // namespace vwrap 

#endif  // VULKAWRAP_UTIL_ASSERT_HPP

