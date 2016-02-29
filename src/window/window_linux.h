//---- src/window/window_linux.h - Window class with linux functionality ----//
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
// \file   window_linux.h
// \brief  Defines a class which inherits from the general window class but
// which implments the functionality which is specific to linux.
//
//---------------------------------------------------------------------------//

// This file is an implementation detail and should be included in the window
// base class if the development platform is linux -- therefore no header
// guards.

#include "window_traits.h"
#include <iostream>
#include <stdlib.h>

// Windows class which provides the implementation of the window functionality
// which is specific to linux. The class uses the XCB window library.
class LinuxWindow : public WindowBase<LinuxWindow, WindowTraits> {
 public:
  // Default constructor -- creates the XCB connection
  LinuxWindow();

  // Constructor which allows the size of the window to be set.
  LinuxWindow(uint32_t width, uint32_t height);

  // Destructor -- cleans up all window features.
  ~LinuxWindow();

  // Alias for the window type so that the base 
  // Window<LinuxWindow> class can access it
  using WindowType = typename WindowTraits::WindowType;

  // Sets up a window using the XCB window library.
  WindowType setupWindow();

  // Renders the window to the screen.
  void render();

 private:
  // Alias for the window base type.
  using WindowBaseType = WindowBase<LinuxWindow, WindowTraits>;

  bool                      Quit;
  xcb_connection_t*         Connection;
  xcb_screen_t*             Screen;
  WindowType                Window;
  xcb_intern_atom_reply_t*  AtomDeleteWindow;

  // Handles window events.
  void handleEvent(const xcb_generic_event_t* motion);

  // handles mouse events.
  void handleMouseEvent(const xcb_motion_notify_event_t* motion);

  // Initializes the XCB connection.
  void initializeXcbConnection();
};


//---- Public ---------------------------------------------------------------//

LinuxWindow::LinuxWindow() 
: Quit(false) {
  initializeXcbConnection();
}

LinuxWindow::LinuxWindow(uint32_t width, uint32_t height) 
: WindowBaseType(width, height), Quit(false) {
  initializeXcbConnection();
}

LinuxWindow::~LinuxWindow() {
  xcb_destroy_window(Connection, Window);
  xcb_disconnect(Connection);
}

typename WindowTraits::WindowType LinuxWindow::setupWindow() {
  uint32_t valueMask, valueList[32];

  Window = xcb_generate_id(Connection);

  // Set some of the properties of the window.
  valueMask     = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  valueList[0]  = Screen->black_pixel;
  valueList[1]  = XCB_EVENT_MASK_KEY_RELEASE      |
                  XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                  XCB_EVENT_MASK_POINTER_MOTION   |
                  XCB_EVENT_MASK_BUTTON_PRESS     |
                  XCB_EVENT_MASK_BUTTON_RELEASE   ;

  // Create the window using the set values.  
  xcb_create_window(Connection, XCB_COPY_FROM_PARENT, Window, Screen->root,
                    0, 0, this->Width, this->Height, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, Screen->root_visual, 
                    valueMask, valueList);

  // Code that will send a notification when the window is destroyed.
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(Connection, 1, 12,
    "WM_PROTOCOLS");
  xcb_intern_atom_reply_t*  reply = xcb_intern_atom_reply(Connection, cookie,
    0);

  xcb_intern_atom_cookie_t otherCookie = xcb_intern_atom(Connection, 0, 16,
    "WM_DELETE_WINDOW");
  AtomDeleteWindow = xcb_intern_atom_reply(Connection, otherCookie, 0);

  xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, Window, reply->atom, 
    4, 32, 1, &(AtomDeleteWindow->atom));
  xcb_change_property(Connection, XCB_PROP_MODE_REPLACE, Window,
    XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, Title.size(), Title.c_str());

  // Map the the window and the connection
  xcb_map_window(Connection, Window);

  // Clean up
  free(reply);

  return(Window);
}

//---- Private --------------------------------------------------------------//

void LinuxWindow::handleEvent(const xcb_generic_event_t* event) {
  switch (event->response_type & 0x7F) {
    case XCB_CLIENT_MESSAGE: {
      auto messageEvent = 
        reinterpret_cast<const xcb_client_message_event_t*>(event);
      if (messageEvent->data.data32[0] == AtomDeleteWindow->atom) 
        Quit = true; 
    }  break;
    case XCB_MOTION_NOTIFY: {
      auto  motion = 
        reinterpret_cast<const xcb_motion_notify_event_t*>(event);
      handleMouseEvent(motion);
    } break;
    case XCB_BUTTON_PRESS: {
      auto buttonPress = 
        reinterpret_cast<const xcb_button_press_event_t*>(event);
      Mouse.leftButton  = (buttonPress->detail & XCB_BUTTON_INDEX_1);
      Mouse.rightButton = (buttonPress->detail & XCB_BUTTON_INDEX_3);
    } break;
    case XCB_BUTTON_RELEASE: {
      auto buttonPress = 
        reinterpret_cast<const xcb_button_press_event_t*>(event);
      if (buttonPress->detail & XCB_BUTTON_INDEX_1) 
        Mouse.leftButton = false;
      if (buttonPress->detail & XCB_BUTTON_INDEX_3)
        Mouse.rightButton = false;
    } break;
    case XCB_KEY_RELEASE: {
      auto key = reinterpret_cast<const xcb_key_release_event_t*>(event);
      if (key->detail == 0x90) 
        Quit = true;
    } break;
    default: break;
  }
}

void LinuxWindow::handleMouseEvent(const xcb_motion_notify_event_t* motion) {
  if (Mouse.leftButton) {
    std::cout << Mouse.position.x << " : " << Mouse.position.y << "\n";
  }
  if (Mouse.rightButton) {
    std::cout << "Options :";
  }
  Mouse.position = glm::vec2(static_cast<float>(motion->event_x),
                             static_cast<float>(motion->event_y));
}
 
void LinuxWindow::initializeXcbConnection() {
  const xcb_setup_t*    setup;
  xcb_screen_iterator_t screenIterator;
  int                   screen;

  // Create the connection
  Connection = xcb_connect(nullptr, &screen);
  if (Connection == nullptr) {
//    std::cerr << "Could not create window, terminating\n" << std::flush;
    exit(EXIT_FAILURE);
  }

  setup           = xcb_get_setup(Connection);
  screenIterator  = xcb_setup_roots_iterator(setup);
  while (screen-- > 0) 
    xcb_screen_next(&screenIterator);
  Screen = screenIterator.data;
}

void LinuxWindow::render() {
  xcb_flush(Connection);
  while (!Quit) {
    auto event = xcb_poll_for_event(Connection);
    if (event) {
      handleEvent(event);
      free(event);
    }
  }
}
