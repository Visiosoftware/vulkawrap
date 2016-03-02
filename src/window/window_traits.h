//---- src/window/window_traits. - Traits for window types ------------------//
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
// \file   window_traits.h
// \brief  Defines traits for a window class.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_WINDOW_WINDOW_TRAITS_H
#define VULKAN_WINDOW_WINDOW_TRAITS_H

#include <xcb/xcb.h>

static constexpr uint32_t defaultWidth  = 1280;
static constexpr uint32_t defaultHeight = 720;

struct WindowTraits {
#if defined(_WIN32)

#elif defined(__linux__)

  /// Alias for the type of window.
  using WindowType = xcb_window_t;

#endif
};

#endif  // VULKAN_WINDOW_WINDOW_TRAITS_H


