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

#include <string>

static constexpr uint32_t defaultWidth  = 1280;
static constexpr uint32_t defaultHeight = 720;

// Defines a class which provides the general features of a window which are
// not specific to the OS. The class can then be used as a base class of the
// more specific windows class for an OS.
//
// tparam: OsSpecificWindow Implements OS specific window functions.
template <typename OsSpecificWindow>
class WindowBase {
 public:  
  // Default constructor to set the window size.
  WindowBase() 
  : Width(defaultWidth), Height(defaultHeight), Title("Vulkan Example"), 
    Name("vulkanExample") {}

  // Constructor to set the size of the window.
  WindowBase(uint32_t width, uint32_t height) 
  : Width(width), Height(height), Title("Vulkan Example"), 
    Name("vulkanExample") {}

 protected:
  uint32_t Width;  
  uint32_t Height;

 private:
  std::string Title;
  std::string Name;

  // Gets non-const access to the OsSpecificWindow.
  OsSpecificWindow* osSpecificWindow() {
    return static_cast<OsSpecificWindow*>(this);
  }

  // Gets const access to the OsSpecificWindow.
  const OsSpecificWindow* osSpecificWindow() const {
    return static_cast<const OsSpecificWindow*>(this);
  }

  // Sets up the window.
  void setupWindow() const {
    osSpecificWindow()->setupWindow();
  }
};

#endif  // VULKAN_WINDOW_WINDOW_BASE_HPP