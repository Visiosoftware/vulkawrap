//---- src/vulkan/error.hpp - Vulkan errors handling functions --------------//
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
// \file   error.hpp
// \brief  Defines error handling functions for vulkan.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_VULKAN_ERROR_HPP
#define VULKAN_VULKAN_ERROR_HPP

#include "config.hpp"
#include "io.h"
#include <type_traits>

namespace vs {

using ErrorHandlingType  = uint8_t;
using AssertHandlingType = uint8_t;

namespace err {
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

}  // namespace detail

/// Function which assert a condition, and takes an optinal message to describe
/// the error. If the assertation fails the program exits.
///
/// \param  condition      The condition to assert.
/// \param  message        The optional message to print if the assertation
///         fails.
/// \tparam AssertHandling The type of assert handling which is supported. This
///         is either config::AssertHandlingOn or config::AssertHandlingOff.
template <AssertHandlingType AssertHandling = config::AssertHandlingSupport>
typename std::enable_if<
  detail::assert_handling_enabled<AssertHandling>::value, void>::type
vwAssert(bool condition, const std::string& message = "",
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
template <AssertHandlingType AssertHandling = config::AssertHandlingSupport>
typename std::enable_if<
  !detail::assert_handling_enabled<AssertHandling>::value, void>::type
vwAssert(bool condition, const std::string& message = "", 
    const std::string& file = "", int line = 0) {
  // Does noting so that when this instance of the assert is called, the
  // compiler can optimize it out ...
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
template <AssertHandlingType AssertHandling = config::AssertHandlingSupport>
typename std::enable_if<
  detail::assert_handling_enabled<AssertHandling>::value, void>::type 
vwAssertSuccess(VkResult result, const std::string& message = "",
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
template <AssertHandlingType AssertHandling = config::AssertHandlingSupport>
typename std::enable_if<
  !detail::assert_handling_enabled<AssertHandling>::value, void>::type
vwAssertSuccess(VkResult result, const std::string& message = "",
    const std::string file = "", int line = 0) {
  // Does nothing so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

//---- MACROS =/ ------------------------------------------------------------//

#define vwAssert(condition, message)                                          \
  util::vwAssert(condition, message, __FILE__, __LINE__)

#define vwAssertSuccess(result, message)                                      \
  util::vwAssertSuccess(result, message, __FILE__, __LINE__)
 

}  // namespace err
}  // namespace vs

#endif  // VULKAN_VULKAN_ERROR_HPP

