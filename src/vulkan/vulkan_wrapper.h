//---- src/vulkan/vulkan_wrapper.h - Wrappers around some vulkan types -----//
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
// \file   vulkan_wrapper.h
// \brief  Defines wrappers around some vulkan types.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_VULKAN_VULKAN_WRAPPER_H
#define VULKAN_VULKAN_VULKAN_WRAPPER_H

#include <vulkan/vulkan.h>
#include <algorithm>
#include <iostream>
#include <vector>

//---- Enums ----------------------------------------------------------------//

// Enums for the types of physical devices which are supported -- these are the
// same values as they are in vulkan, but with the addition of the any type, to
// allow for indifference.
enum class VwDeviceType : uint8_t {
  VW_OTHER          = 0,
  VW_INTEGRATED_GPU = 1,
  VW_DISCRETE_GPU   = 2,
  VW_VIRTUAL_GPU    = 3,
  VW_CPU            = 4,
  VW_ANY            = 5
};

// Enum for the types of queues which are supported -- these are the same
// values as for pure vulkan, but with the addition of the any type, so that
// any queue can be specified.
enum class VwQueueType : uint8_t {
  VW_GRAPHICS_QUEUE       = 0x01,
  VW_COMPUTE_QUEUE        = 0x02,
  VW_TRANSFER_QUEUE       = 0x04,
  VW_SPARSE_BINDING_QUEUE = 0x08,
  VW_ANY                  = 0x10
};

//---- Aliases --------------------------------------------------------------//

using VwQueueFamPropVec  = std::vector<VkQueueFamilyProperties>;
using VwQueueTypeVec     = std::vector<VwQueueType>;
using VwQueueIdVec       = std::vector<uint32_t>;

//---- Structs --------------------------------------------------------------//

// Wrapper for Vulkan Physical Devices to include the queus which are
// assosciated with the physical device, so that these types of devices can be
// used to let the user speciffy a device type and the queues which it must
// support.
struct VwPhysicalDevice {
  VkPhysicalDevice    device;
  VwQueueTypeVec      queueTypes;
  VwQueueIdVec        queueIds;

  // Default constructor 
  VwPhysicalDevice() 
  : queueTypes(0), queueIds(0) {};

  // Constructor which takes a vulkan physical device.
  //
  // \param vkPhysicalDevice The vulkan physical device.
  VwPhysicalDevice(const VkPhysicalDevice& vkPhysicalDevice) 
  : device(vkPhysicalDevice), queueTypes(0), queueIds(0) {}

  // Constructor which takes the device, the queue types and indexes of those
  // queues.
  //
  // \param vkPhysicalDevice The vulkan physical device.
  // \param qTypes           The types of queus which are supported.
  // \param qIds             The indexes of each queue type.
  VwPhysicalDevice(const VkPhysicalDevice vkPhysicalDevice, 
    const VwQueueTypeVec& qTypes, const VwQueueIdVec qIds) 
  : device(vkPhysicalDevice), queueTypes(qTypes), queueIds(qIds) {}

  // Checks for the requested queues, and adds those which are a match.
  //
  // \param requestedQueueType The type of queues the user requests the device
  // should have.
  void addSupportedQueues(const VwQueueTypeVec& requestedQueueTypes) {
    uint32_t queueCount, queueId = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    if (queueCount < 1) return;

    VwQueueFamPropVec queueProperties;
    queueProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, 
      queueProperties.data());
    
    for (; queueId < queueCount; ++queueId) {
      for (const auto& queueType : requestedQueueTypes) {
        if ((!(static_cast<uint8_t>(queueProperties[queueId].queueFlags)  & 
               static_cast<uint8_t>(queueType))                         ) &&
            (queueType != VwQueueType::VW_ANY                           )) {
          continue;
        }

        queueIds.push_back(queueId);
        queueTypes.push_back(queueType);
      }
    }
  }
};

// Struct to allow the device type and the queues it must support be specified.
struct VwDeviceSpecifier {
  // Default constructor -- specify the device type and the queues it needs to
  // support.
  //
  // \note While only the first type is checked for equivalence with
  // VwQueueType, if any of the parameter pack is not VwQueueType then
  // construction of the vector will fail, which is what we want.
  //
  // \param QType   The type of the first queue which must be supported for
  // the device
  // \param QTypes  The types of the rest of the queues which must be
  // supported
  template <typename QType, typename... QTypes, typename = 
            std::enable_if_t<std::is_same<VwQueueType, QType>::value>>
  VwDeviceSpecifier(VwDeviceType device, QType qType, QTypes... qTypes)
  : deviceType(device), queueTypes{qType, qTypes...} {}

  VwDeviceType    deviceType; // The type of device to look for.
  VwQueueTypeVec  queueTypes; // The types of queues the device must support.
};

// Struct which can handles all errors.
struct ErrorHandler {
};

// Struct which removes all error handling code.
struct ErrorRemover {
};

// Struct which can handle asserts.
struct AssertHandler {
};

// Struct which can remove asserts.
struct AssertRemover {
};

using ErrorHandling  = ErrorHandler;
using AssertHandling = AssertHandler;

//---- Aliases2 -------------------------------------------------------------//

using VwDeviceSpecVec = std::vector<VwDeviceSpecifier>;
using VwPhysDeviceVec = std::vector<VwPhysicalDevice>;

//---- Functions ------------------------------------------------------------//

namespace algo {

// Wrapper around erase and remove_if to remove elements from a vector over the
// entire vector.
template <typename VecType, typename Predicate>
inline void vecRemoveIf(VecType& vec, Predicate predicate) {
  vec.erase(std::remove_if(vec.begin(), vec.end(), predicate), vec.end());
}

// Wrapper around erase and remove_if to remove elements when the start and end
// iterator for the vector are given.
template <typename VecType, typename ForwardIt, typename Predicate>
inline void vecRemoveIf(VecType& vec, ForwardIt first, ForwardIt last, 
       Predicate predicate) {
  vec.erase(std::remove_if(first, last, predicate), last);
}

}  // namespace algo

namespace detail {

// Metafunction to check if a type supports error handling.
//
// \tparam Type The type to check for error handling.
template <typename Type> 
struct supports_error_handling {
  static constexpr bool value = std::is_same<ErrorHandler, Type>::value ||
                                std::is_base_of<ErrorHandler, Type>::value;
};

// Metafunction to check if a type supports assert handling.
//
// \tparam Type The type for check for assert handling.
template <typename Type>
struct supports_assert_handling {
  static constexpr bool value = std::is_same<AssertHandler, Type>::value ||
                                std::is_base_of<AssertHandler, Type>::value;
};

}  // namespace detail

namespace util {

// Prints the the meaning of a VkResult code.
//
// \param result The code for result to print.
void printVulkanResult(VkResult result) {
#define GET_VULKAN_RES_CODE(code, msg)                                        \
  case (code) : std::cerr << (msg); break

  switch (result) {
    GET_VULKAN_RES_CODE(  0 , "VK_SUCCESS\n"                                 );
    GET_VULKAN_RES_CODE(  1 , "VK_NOT_READY\n"                               );
    GET_VULKAN_RES_CODE(  2 , "VK_TIMEOUT\n"                                 );
    GET_VULKAN_RES_CODE(  3 , "VK_EVENT_SET\n"                               );
    GET_VULKAN_RES_CODE(  4 , "VK_EVENT_RESET\n"                             );
    GET_VULKAN_RES_CODE(  5 , "VK_INCOMPLETE\n"                              );
    GET_VULKAN_RES_CODE( -1 , "VK_ERROR_OUT_OF_HOST_MEMORY\n"                );
    GET_VULKAN_RES_CODE( -2 , "VK_ERROR_OUT_OF_DEVICE_MEMORY\n"              );
    GET_VULKAN_RES_CODE( -3 , "VK_ERROR_INITIALIZATION_FAILED\n"             );
    GET_VULKAN_RES_CODE( -4 , "VK_ERROR_DEVICE_LOST\n"                       );
    GET_VULKAN_RES_CODE( -5 , "VK_ERROR_MEMORY_MAP_FAILED\n"                 );
    GET_VULKAN_RES_CODE( -6 , "VK_ERROR_LAYER_NOT_PRESENT\n"                 );
    GET_VULKAN_RES_CODE( -7 , "VK_ERROR_EXTENSION_NOT_PRESENT\n"             );
    GET_VULKAN_RES_CODE( -8 , "VK_ERROR_FEATURE_NOT_PRESENT\n"               );
    GET_VULKAN_RES_CODE( -9 , "VK_ERROR_INCOMPATIBLE_DRIVER\n"               );
    GET_VULKAN_RES_CODE( -10, "VK_ERROR_TOO_MANY_OBJECTS\n"                  );
    GET_VULKAN_RES_CODE( -11, "VK_ERROR_FORMAT_NOT_SUPPORTED\n"              );
    GET_VULKAN_RES_CODE( -1000000000, "VK_ERROR_SURFACE_LOST_KHR\n"          );
    GET_VULKAN_RES_CODE( -1000000001, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR\n"  );
    GET_VULKAN_RES_CODE(  1000001003, "VK_SUBOPTIMAL_KHR\n"                  );
    GET_VULKAN_RES_CODE( -1000001004, "VK_ERROR_OUT_OF_DATE_KHR\n"           );
    GET_VULKAN_RES_CODE( -1000003001, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR\n"  );
    GET_VULKAN_RES_CODE( -1000011001, "VK_ERROR_VALIDATION_FAILED_EXT\n"     );
    GET_VULKAN_RES_CODE(  0x7FFFFFFF, "VK_RESULT_MAX_ENUM\n"                 );
    GET_VULKAN_RES_CODE( VK_RESULT_RANGE_SIZE , "VK_RESULT_RANGE_SIZE\n"     );
    default : std::cerr << "UNKNOWN_ERROR\n";
  }
}

// Function to check for an error, print the vulkan error and the optional
// error message, if error handling is enabled.
//
// \param AssertHandlingType The type of assert handling -- this either enabled
// the assertation function or removes the assert code, depending on the type.
template <typename AssertHandlingType = AssertHandling>
typename std::enable_if<
  detail::supports_assert_handling<AssertHandlingType>::value, void>::type
vwAssert(bool condition, const std::string& message = "",
    const std::string file = "", int line = 0) {
  if (!condition) {
    std::cerr << "Failure at         : " << file    << " : " << line << ".\n" 
              << "Additional message : " << message << ".\n\n";
    exit(EXIT_FAILURE);
  }
}

// Implementation of the assert function for when assertations are not enabled.
//
// \param AssertHanldingType The type of assert handling -- for this insance
// this is a type which does not support assert handling.
template <typename AssertHandlingType = AssertHandling>
typename std::enable_if<
  !detail::supports_assert_handling<AssertHandlingType>::value, void>::type
vwAssert(bool condition, const std::string& message = "", 
    const std::string& file = __FILE__, int line = __LINE__) {
  // Does noting so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

// Function which asserts when assertation handling is enabled and if the reult
// code of the vulkan result type is a failure.
//
// \tparam AssertHandlingType The type to check for assertation handling -- for
// this insance of the function this will be true.
template <typename AssertHandlingType = AssertHandling>
typename std::enable_if<
  detail::supports_assert_handling<AssertHandlingType>::value, void>::type 
vwAssertSuccess(VkResult result, const std::string& message = "",
    const std::string& file = "", int line = 0) {
  if (result != 0) {
    std::cerr << "Failure at         : " << file << " : " << line << ".\n"
              << "Error code         : ";
    printVulkanResult(result);
    std::cerr << "Additional message : " << message << "\n";
    exit(EXIT_FAILURE);
  }
}

// Function which removed the assertation check when assrtation handling is
// disabled. 
//
// \tparam AssertHandlingType The type to checks for assertation handling --
// for this inastance of the function this will be false.
template <typename AssertHandlingType = AssertHandling>
typename std::enable_if<
  !detail::supports_assert_handling<AssertHandlingType>::value, void>::type
vwAssertSuccess(VkResult result, const std::string& message = "",
    const std::string file = "", int line = 0) {
  // Does nothing so that when this instance of the assert is called, the
  // compiler can optimize it out ...
}

}  // namespace util

//---- MACROS =/ ------------------------------------------------------------//

#define vwAssert(condition, message)                                          \
  util::vwAssert(condition, message, __FILE__, __LINE__)

#define vwAssertSuccess(result, message)                                      \
  util::vwAssertSuccess(result, message, __FILE__, __LINE__)
 
#endif  // VULKAN_VULKAN_VULKAN_WRAPPER_H


