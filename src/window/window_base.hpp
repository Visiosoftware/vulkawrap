//---- src/window/window_base.hpp - CRTP Window base class ------------------//
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
// \file   window_base.hpp
// \brief  Defines a class which provides platform agnostic features of a
// window, the platform specific features of the window are then implemented in
// the derived class, using CRTP to call the functions from the Window class.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_WINDOW_WINDOW_BASE_HPP
#define VULKAN_WINDOW_WINDOW_BASE_HPP

#if defined(_WIN32)
#else 
#include "window_traits_linux.h"
#endif

#include <string>
#include <glm/glm.hpp>

static constexpr uint32_t defaultWidth  = 1280;
static constexpr uint32_t defaultHeight = 720;

struct Mouse {
  Mouse() 
  : position(0.0f, 0.0f), leftButton(false), rightButton(false) {}

  glm::vec2 position;
  bool      leftButton;
  bool      rightButton;
};

// Defines a class which defines the interface for windows, and then by using
// CRTP, the more specific functionality for the platform is implemented in the
// derived class.
//
// tparam: OsSpecificWindow Implements OS specific window functions.
// tparam: WinTraits The traits specific to the window
template <typename OsSpecificWindow, typename WinTraits>
class WindowBase {
 public: 
  // Alias for the type of window which is being used.
  using WindowType = typename WinTraits::WindowType;

  // Default constructor to set the window size.
  WindowBase() 
  : Width(defaultWidth), Height(defaultHeight), Title("Vulkan Example"), 
    Name("vulkanExample") {}

  // Constructor to set the size of the window.
  WindowBase(uint32_t width, uint32_t height) 
  : Width(width), Height(height), Title("Vulkan Example"), 
    Name("vulkanExample") {}

  // Creates the window.
  WindowType create() {
    return osSpecificWindow()->setupWindow();
  };

  // Draws the window to the screen.
  void draw() {
    return osSpecificWindow()->render();
  };

 protected:
  uint32_t    Width;  
  uint32_t    Height;
  std::string Title;
  std::string Name;
   
  struct {
    glm::vec2 position      = glm::vec2(0.0f, 0.0f);
    bool      leftButton    = false;
    bool      rightButton   = false;
  } Mouse;

  // Gets non-const access to the OsSpecificWindow.
  OsSpecificWindow* osSpecificWindow() {
    return static_cast<OsSpecificWindow*>(this);
  }

  // Gets const access to the OsSpecificWindow.
  const OsSpecificWindow* osSpecificWindow() const {
    return static_cast<const OsSpecificWindow*>(this);
  }
};

// Aliases for a window based on the platform which is being used, since 
// we can be sure that only one of these can be valid for a single program.
#if defined(_WIN32)
  //using Window = Window<WindowsWindow>;
#else 
  #include "window_linux.h"
  using Window = LinuxWindow;
#endif

#endif  // VULKAN_WINDOW_WINDOW_BASE_HPP