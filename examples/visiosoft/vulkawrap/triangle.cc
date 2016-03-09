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

#include <visiosoft/vulkawrap/device/filter.h>
#include <iostream>

int main() {
  using namespace vs::vwrap;

  UniqueInstance instance = makeUniqueInstance();

  // Define the DeviceSpecifier for any device and ay queue,
  // and another which must have a graphics queue. By default
  // all the requested queues must be found, but by passing 
  // false as the second parameter, a device will be created 
  // if any of the specified queues are found.
  DeviceSpecifier anyDevice(DeviceType::VW_ANY, QueueType::VW_ANY);
  DeviceSpecifier graphicsDevice(DeviceType::VW_ANY, 
                                 QueueType::VW_GRAPHICS_QUEUE);
 
  // Create a device filter -- moves ownershp of the instance to the filter.
  DeviceFilter deviceFilter(std::move(instance), anyDevice, graphicsDevice);

  // Check if a graphics device is found, otherwise we can't draw!
  if (!graphicsDevice.valid)
    std::cerr << "Can't present without graphics device!\n";
}
