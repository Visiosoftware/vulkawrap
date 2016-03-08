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

int main() {
  using namespace vs::vwrap;

  DeviceSelector deviceSelector(
      DeviceSpecifierVec{ { DeviceType::VW_ANY, QueueType::VW_ANY } }
  );
}
