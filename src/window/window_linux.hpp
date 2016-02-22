//---- src/window/window_linux.hpp - Window class with linux functionality --//
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
// \file   window_linux.hpp
// \brief  Defines a class which inherits from the general window class but
// which implments the functionality which is specific to linux.
//
//---------------------------------------------------------------------------//

// This file is an implementation detail and should be included in the window
// base class if the development platform is linux -- therefore no header
// guards.

#include <xcb/xcb.h>

// Windows class which provides the implementation of the window functionality
// which is specific to linux. The class uses the XCB window library.
class LinuxWindow : public Window<LinuxWindow> {
 public:
  xcb_window_t setupWindow();

 private:
  xcb_connection_t*         Connection;
  xcb_screen_t              Screen;
  xcb_window_t              Window;
  xcb_intern_atom_reply_t*  AtomDeleteWindow;
};

xcb_window_t LinuxWindow::setupWindow() {
  uint32_t valueMask, valueList[32];
  
  // Continue ...

}


