//---- src/widget/vulkan_widget_linux.h - Vulkan widget for linux -----------//
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
// \file   vulkan_widget_linux.h
// \brief  Implementation of a vulkan widget for linux.
//
//---------------------------------------------------------------------------//

#ifndef VULKAN_WIDGET_VULKAN_WIDGET_LINUX_H
#define VULKAN_WIDGET_VULKAN_WIDGET_LINUX_H

#include <vulkan/vulkan.h>
#include "widget_traits.h"

// Implementation of the vulkan widget class which is specific to linux. This
// implementation uses the XCB library.
class VulkanWidgetLinux : 
    public VulkanWidgetBase<VulkanWidgetLinux, WidgetTraits> {
 public:
  // Constructor -- initializes the widget.
  //
  // \param width The width of the widget in pixels.
  // \param height The height of the widget in pixels.
  explicit VulkanWidgetLinux(uint32_t width, uint32_t height)
  : BaseWidget(width, height) {};

  // Initializes the vulkan linux widget implemenetation.
  //
  // \param window The window to use for the widget.
  // \param connection The xcb connection for the window.
  void initialize(WindowType window, HandleType connection) {   
    VkResult error;

    // Define the creation information for and xcb surface.
    VkXcbSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.connection = connection;
    surfaceInfo.window     = window;
    error = vkCreateXcbSurfaceKHR(this->instance(), &surfaceInfo, nullptr, 
              &this->Surface);
    assert(!error && "Failed to create an XCB surface : Fatal Error\n");
  }

 private:
    using BaseWidget = VulkanWidgetBase<VulkanWidgetLinux, WidgetTraits>;
    using HandleType = typename WidgetTraits::HandleType;
    using WindowType = typename WidgetTraits::WindowType;
};

#endif  // VULKAN_WIDGET_VULKAN_WIDGET_LINUX_H
