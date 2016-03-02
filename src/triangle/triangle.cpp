//---- src/triangle/triangle.cpp - Main file for triangle example ------ ----//
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
// \file   triangle.cpp
// \brief  This is the main implementation file for the triangle exmaple.
//
//---------------------------------------------------------------------------//

#include "../window/window_base.hpp"

int main() {

  // Create a new window using the default size.
  Window window;
  window.initialize();

  // Add a vulkan widget to the window.
  window.addWidget(
    std::make_unique<VulkanWidget>(window.size().x, window.size().y)
  );

  // Draw the window to the screen.
  window.draw();
}
