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

#include "window_traits.h"
#include "../widget/vulkan_widget_base.hpp"

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

// TODO: Change width and heght to a size

struct Mouse {
  Mouse() 
  : position(0.0f, 0.0f), leftButton(false), rightButton(false) {}

  glm::vec2 position;
  bool      leftButton;
  bool      rightButton;
};

struct Size {
  // Constructor for creating a Size object.
  Size(uint32_t sizeX, uint32_t sizeY) 
  : x(sizeX), y(sizeY) {}

  uint32_t x;
  uint32_t y;
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

  // Constructor to set the size of the window.
  explicit WindowBase(uint32_t width, uint32_t height) 
  : Width(width), Height(height), Title("Vulkan Example"), 
    Name("vulkanExample") {}

  // Adds a widget to the widow.
  //
  // \param The widget to add to the window.
  void addWidget(std::unique_ptr<Widget> widget) {
    Widgets.push_back(std::move(widget));
  }

  // Creates the window.
  WindowType initialize() {
    return osSpecificWindow()->setupWindow();
  };

  // Draws the window to the screen.
  void draw() {
    return osSpecificWindow()->render();
  };

  // Gets the size of the window.
  Size size() const { return Size(Width, Height); }

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

 private:
  // Alias for a vector of widgets.
  using WidgetVec  = std::vector<std::unique_ptr<Widget>>;
  
  // All the widgets for the widow.
  WidgetVec Widgets;
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