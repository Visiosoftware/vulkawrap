//---- include/vulkawrap/io.h ------------------------------ -*- C++ -*- ----//
//
//                                Vulkarap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   io.h
/// \brief  Defines all the io related functions for the Vulkawrap library.
//
//---------------------------------------------------------------------------//

#ifndef VULKAWRAP_IO_H
#define VULKAWRAP_IO_H

#include <iostream>
#include <vulkan/vulkan.h>

namespace vwrap {
namespace io    {    

/// Prints the name of a VkResult type.
///
/// \param result The VkResult type to print the name of.
static void printVulkanResult(VkResult result) {
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

} // namespace io
} // namespace vwrap

#endif  // VULKAWRAP_IO_H
