//---- examples/visiosoft/vulkawrap/triangle.cpp ----------- -*- C++ -*- ----//
//---- Triangle example implementation.                                  ----//
//
//                           Visiosoft VulkaWrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   triangle.cpp
/// \brief  Implemenation of the triangle example for Visiosoft VulkaWrap.
//
//---------------------------------------------------------------------------//

#include <visiosoft/vulkawrap/device/selector.h>
#include <iostream>

int main() {
  using namespace vs::vwrap;

  // Define the DeviceSpecifier for the types of devices we want.
  DeviceSpecifier anyDevice(DeviceType::VW_ANY, QueueType::VW_ANY);
  DeviceSpecifier graphicsDevice(DeviceType::VW_ANY, 
                                 QueueType::VW_GRAPHICS_QUEUE);
  DeviceSpecifierVec specifiers{ anyDevice, graphicsDevice };
 
  // Create a device selector to find the devices.
  DeviceSelector deviceSelector(specifiers);

  // Check if a graphics device is found, otherwise we can't draw!
  if (!graphicsDevice.valid)
    std::cerr << "Can't present without graphics device!\n";
}
