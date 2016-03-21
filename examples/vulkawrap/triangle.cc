//---- examples/vulkawrap/triangle.cpp --------------------- -*- C++ -*- ----//
//
//                                Vulkawrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                   https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file   triangle.cc
/// \brief  Implementation of the triangle example for the Vulkawrap library.
//
//---------------------------------------------------------------------------//

#include <vulkawrap/device/filter.h>
#include <iostream>

int main() {
  using namespace vwrap;

  UniqueInstance instance = makeUniqueInstance();
  NonConcurrentSharedInstance  ncSharedInstance;
  ConcurrentSharedInstance     cSharedInstance;


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
