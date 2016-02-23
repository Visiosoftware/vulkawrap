//---- src/window/window_traits_linux.g - Traits for linux windows ----------//
//
//                        Vulkan Examples/Tutorials
//                          
//  This code is modified from Sascha Willems Vulkan examples whic can be found 
//                  at https://github.com/SaschaWillems/Vulkan 
//
//                        Copyright (c) 2016 Rob Clucas        
//                      Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
// \file   window_traits_linux.h
// \brief  Defines traits for a window class which are specific to linux.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_WINDOW_WINDOW_TRAITS_LINUX_H 
#define VULKAN_WINDOW_WINDOW_TRAITS_LINUX_H 

#include <xcb/xcb.h>

struct WindowTraits {
  /// Alias for the type of window.
  using WindowType = xcb_window_t;
};

#endif  // VULKAN_WINDOW_WINDOW_TRAITS_LINUX_H 


