//---- include/visiosoft/vulkawrap/device/selector.h ------- -*- C++ -*- ----//
//---- Provides functionality for selecting vulkan devices.              ----//
//
//                           Visiosoft Vulkawrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file  selector.h
/// \brief Defines a class with functionality to select vulkan devices based on
///       their physical types and the Vulkan queues which they must support.
//
//---------------------------------------------------------------------------//

#ifndef VISIOSOFT_VULKAWRAP_DEVICE_SELECTOR_H
#define VISIOSOFT_VULKAWRAP_DEVICE_SELECTOR_H

#include "queue.h"
#include <vulkan/vulkan.h>

namespace vs    {
namespace vwrap {

//---- Forward declarations -------------------------------------------------//

struct      DeviceSpecifier;
struct      DeviceView;
struct      PhysicalDevice;
enum class  DeviceType : uint8_t;

//---- Aliases --------------------------------------------------------------//

/// Alias for a vector of VulkaWrap devices.
using PhysicalDeviceVec = std::vector<PhysicalDevice>;

/// Alias for a vector of VulkaWrap DeviceSpecifier's.
using DeviceSpecifierVec = std::vector<DeviceSpecifier>;

/// Alias for a vector of VulkaWrap DeviceViews, which are essentially VKDevice 
/// types but for the VulkaWrap physical device, rather than the Vulkan device.
using DeviceViewVec = std::vector<DeviceView>;

//---- Implementations ------------------------------------------------------//

/// The types of physical devices which are supported -- these are the
/// same values as defined by vulkan, but with the addition of the VW_ANY type,
/// to allow for any type of device to be sepecified.
enum class DeviceType : uint8_t {
  VW_OTHER          = 0,
  VW_INTEGRATED_GPU = 1,
  VW_DISCRETE_GPU   = 2,
  VW_VIRTUAL_GPU    = 3,
  VW_CPU            = 4,
  VW_ANY            = 5
};

/// Wrapper for a Vulkan Physical Device to include the queues assosciated
/// with the specific physical device.
struct PhysicalDevice {
  VkPhysicalDevice    device;       //!< The acrual physical device.
  QueueTypeVec        queueTypes;   //!< The types of queues for the device.
  QueueIdVec          queueIds;     //!< The id's of the queues for the device.

  /// Default constructor -- sets the vectors to empty.
  PhysicalDevice() 
  : queueTypes(0), queueIds(0) {};

  /// Constructor which takes a vulkan physical device.
  ///
  /// \param vkPhysicalDevice The vulkan physical device.
  PhysicalDevice(const VkPhysicalDevice& vkPhysicalDevice) 
  : device(vkPhysicalDevice), queueTypes(0), queueIds(0) {}

  /// Constructor which takes a Vulkan Physical Device, the queue types and 
  /// indexes of those queues.
  ///
  /// \param vkPhysicalDevice The vulkan physical device.
  /// \param qTypes           The types of queus which are supported.
  /// \param qIds             The indexes of each queue type.
  PhysicalDevice(const VkPhysicalDevice vkPhysicalDevice, 
    const QueueTypeVec& qTypes, const QueueIdVec qIds) 
  : device(vkPhysicalDevice), queueTypes(qTypes), queueIds(qIds) {}

  /// Checks for the requested queues, and adds those which are a match.
  ///
  /// \param requestedQueueTypes The type of queues that the device must
  ///        support.
  void addSupportedQueues(const QueueTypeVec& requestedQueueTypes) {
    uint32_t queueCount, queueId = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    if (queueCount < 1) return;

    QueueFamilyPropVec queueProperties;
    queueProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, 
      queueProperties.data());
    
    for (; queueId < queueCount; ++queueId) {
      for (const auto& queueType : requestedQueueTypes) {
        if ((!(static_cast<uint8_t>(queueProperties[queueId].queueFlags)  & 
               static_cast<uint8_t>(queueType))                         ) &&
            (queueType != QueueType::VW_ANY                            )) {
          continue;
        }

        queueIds.push_back(queueId);
        queueTypes.push_back(queueType);
      }
    }
  }
};

/// Struct for specifying a type of physical device and the type of queues 
/// which it needs to support.
struct DeviceSpecifier {
  DeviceType    deviceType; //!< The type of device to look for.
  QueueTypeVec  queueTypes; //!< The types of queues the device must support.
  bool          valid;      //!< If the device specifier is valid -- a device
                            //!< matching the specifiers was found.

  /// Constructor to specify the device type and the queues it needs to
  /// support.
  ///
  /// \note While only the first template parameter is checked for equivalence 
  /// wih a QueueType, if any of the parameter pack is not QueueType then
  /// construction of the vector will fail since the vector cannot be filled
  /// with differing types, which is what we want.
  ///
  /// \param  deviceType The type of device to look for.
  /// \param  qType      The type of the first queue which must be supported 
  ///         the device.
  /// \param  qTypes     The types of the rest of the queues which must be
  ///         supported.
  /// \tparam QType      The type of the fist queue. This must be of type 
  ///         QueueType.
  /// \tparam QTypes     The types of the rest of the queues. These must also
  ///         be of type QuueType.
  template <typename QType, typename... QTypes, typename = 
            std::enable_if_t<std::is_same<QueueType, QType>::value>>
  DeviceSpecifier(DeviceType device, QType qType, QTypes... qTypes)
  : deviceType(device), queueTypes{qType, qTypes...}, valid(false) {}
};

/// Class which allows vulkan physical devices to be selected based on their
/// physical type (GPU, CPU, etc) and what typw of vulkan queues they must
/// support. 
/// 
/// Example usage:
/// \code 
/// using namespace vs::vwrap;
/// DeviceSpecifier graphicsDevice(DeviceType::VW_ANY, 
///                                QueueType::VW_GRAPHICS_QUEUE);
/// DeviceSpecifier cpuComputeDevice(DeviceType::VW_CPU,
///                                  QueueType::VW_COMPUTE_QUEUE);
///
/// DeviceSelector deviceSelector(
///   DeviceSpecifierVec{ graphicsDevice, cpuCOmputeDevice }
/// );
///
/// // Check which devices were found 
/// if (!graphicsDevice.valid) 
///   // Error, can't present ...
/// /endcode
class DeviceSelector {
 public:
  /// Constructor which takes a single device specifier for the type of
  /// physical device which is wanted.
  ///
  /// \param deviceSpecifier            The specifier for the properties of the 
  ///        device.
  /// \param deviceMustSUpportAllQueues Only add the device if all the
  ///        requested queues are found.
  /// \param appName                    The name of the vulkan application.
  /// \param engineName                 The name of the engine for this 
  ///        application.
  /// \param extensions                 The vulkan extensions to use.
  DeviceSelector(DeviceSpecifier& deviceSpecifier                         ,
    bool deviceMustSupportAllQueues            = true                     ,
    const char* appName                        = ""                       , 
    const char* engineName                     = ""                       ,
    const std::vector<const char*>& extensions = std::vector<const char*>{});

  /// Constructor which takes a vector of specifiers for the physical devices
  /// which are wanted.
  ///
  /// \param deviceSpecifiers            The specifiers for the properties of 
  ///        each device.
  /// \param devicesMustSupportAllQueues Devices are only added if the device
  ///        supports all the requested queues, and not only some of them.
  /// \param appName                     The name of the vulkan application for 
  ///        this instance.
  /// \param engineName                  The name of the engine for this 
  ///        application.   
  /// \param extensions                  The vulkan extensions to use.
  DeviceSelector(DeviceSpecifierVec& deviceSpecifiers                      ,
    bool devicesMustSupportAllQueues           = true                      , 
    const char* appName                        = ""                        , 
    const char* engineName                     = ""                        ,
    const std::vector<const char*>& extensions = std::vector<const char*>{});

  /// Adds a vulkan physical device to the vector of physical devices, if the
  /// device has the requested queues, and returns true, otherwise returns
  /// false.
  ///
  /// \param physicalDevice       The physical device to add.
  /// \param queueTypes           The types of queues the device must support.
  /// \param mustSupportAllQueues If the device must find all the queus to be
  ///        added.
  bool addIfQueuesAreSupported(const VkPhysicalDevice& physicalDevice, 
    const QueueTypeVec& queueTypes, bool mustSupportAllQueues);

  /// Gets a vulkan physical device from the available physical devices.
  ///
  /// \param deviceIdx The index of the device to get.
  VkPhysicalDevice getVkPhysicalDevice(size_t deviceIdx) const {
    return PhysicalDevices[deviceIdx].device;
  }

  /// Gets a VulkaWrap physical device.
  ///
  /// \param deviceIdx The index of the VulkaWrap physical device to get.
  PhysicalDevice getVwPhysicalDevice(size_t deviceIdx) const {
    return PhysicalDevices[deviceIdx];
  }

 protected:
  VkInstance         Instance;         //!< Stores per-application state.
  PhysicalDeviceVec  PhysicalDevices;  //!< CPUs | GPUs for vulkan.

 private:
  /// Creates the vulkan instance.
  ///
  /// \param appName    The name of the vulkan application.
  /// \param engineName The name of the engine application.
  /// \param extensions The extensions to enable for the instance.
  VkResult createInstance(const char* appName, const char* engineName,
    const std::vector<const char*>& extensions);

  /// Gets all the physical devices available, and returns a vector of the
  /// devices.
  std::vector<VkPhysicalDevice> getPhysicalDevices() const;
};

} // namespace vwrap
} // namespace vs

#endif  // VISIOSOFT_VULKAWRAP_DEVICE_SELECTOR_H
