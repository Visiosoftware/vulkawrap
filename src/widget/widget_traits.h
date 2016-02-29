//---- src/window/widget_traits. - Traits for widget types ------------------//
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
// \file   widget_traits.h
// \brief  Defines traits for the vulkan widget class.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_WIDGET_WIDGET_TRAITS_H
#define VULKAN_WIDGET_WIDGET_TRAITS_H

#if defined(_WIN32) 

#elif defined(__linux__)

#include <xcb/xcb.h>

#elif defined(__ANDROID__)

#endif

struct WidgetTraits {
#if defined(_WIN32)

  using HandleType = void*;
  using WindowType = void*;

#elif defined(__linux__)

  using HandleType = xcb_connection_t*;
  using WindowType = xcb_window_t;

#elif defined(__ANDROID__)
 
  using HandleType = void*;
  using WindowType = ANativeWindow*;

#endif
};

#endif  // VULKAN_WIDGET_WIDGET_TRAITS_H


