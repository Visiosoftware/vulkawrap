//---- include/visiosoft/vulkawrap/device/filter.h --------- -*- C++ -*- ----//
//---- Provides functionality for filtering physical device.             ----//
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
/// \file  filter.h
/// \brief Defines a class with functionality to filter vulkan physical devices
///        based on their device type and the queue tyoes they support.
//
//---------------------------------------------------------------------------//

#ifndef VISIOSOFT_VULKAWRAP_DEVICE_FILTER_H 
#define VISIOSOFT_VULKAWRAP_DEVICE_FILTER_H

#include "queue.h"
#include "../instance/instance.h"
#include <vulkan/vulkan.h>
#include <memory>

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
  QueueTypeVec  queueTypes; //!< The types of queues the device must support.
  DeviceType    deviceType; //!< The type of device to look for.
  bool          valid;      //!< If the device specifier is valid -- a device
                            //!< matching the specifiers was found.
  bool          mustSupportAllQueues;  //!< If the physical device must 
                                       //!< support all queues.

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
  : queueTypes{qType, qTypes...}, deviceType(device), valid(false),
    mustSupportAllQueues(true) {}
  
  /// Constructor which allows for the specificatio of allowing the device to
  /// be found if only some of the queues are found.
  ///
  /// \param  deviceType       The type of device to look for.
  /// \param  supportAllQueues If the device must support all queues.
  /// \param  qType            The type of the first queue to look for.
  /// \param  qTypes           The type of the rest of the queues to look for.
  /// \tparam QType            The type of the fist queue. This must be of type 
  ///         QueueType.
  /// \tparam QTypes           The types of the rest of the queues. These must
  ///         be of type QuueType.
  template <typename QType, typename... QTypes, typename = 
    std::enable_if_t<std::is_same<QueueType, QType>::value>>
  DeviceSpecifier(DeviceType device, bool supportAllQueues,
    QType qType, QTypes... qTypes)
  : queueTypes{qType, qTypes...}, deviceType(device), valid(false),
    mustSupportAllQueues(supportAllQueues) {}
};

/// Class which allows vulkan physical devices to be filtered based on their
/// physical type (GPU, CPU, etc) and what type of vulkan queues they must
/// support. 
/// 
/// Example usage:
/// \code 
/// DeviceFilter deviceFilter(
///   instance         , // A UniqueInstance with application state
///   graphicsDevice   , // First queus
///   cpuComputeDevice   // Rest of the device specifiers ...
/// );
///
/// // The constructor changes the validity of the specifier if a device
/// // meeting the specifiers requirements is found ...
/// if (!graphicsQueue.valid)
///   // exit ...
///
/// /endcode
class DeviceFilter {
 public:
  /// Constructor which takes a single device specifier for the type of
  /// physical device which is wanted.
  ///
  /// \param  instance             The instance for vulkan state.
  /// \param  mustSUpportAllQueues If all the specifier queus must be found for
  ///         the device to be added.
  /// \param  deviceSpecifier      The first device specifier.
  /// \param  deviceSpecifiers     The rest of the device specifiers.
  /// \tparam SpecifierType        The type of the first specifier -- must be 
  ///         of type DeviceSpecifier.
  /// \tparam SpecifierTypes       The types of the rest of the specifiers --
  ///         be of types DeviceSpecifier.
  template <typename SpecifierType, typename... SpecifierTypes, typename = 
    std::enable_if_t<std::is_same<DeviceSpecifier, SpecifierType>::value>>
  DeviceFilter(
    UniqueInstance     instance        ,
    SpecifierType&     deviceSpecifier , 
    SpecifierTypes&... deviceSpecifiers
  );

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
  UniqueInstance     Instance;         //!< Stores per-application state.
  PhysicalDeviceVec  PhysicalDevices;  //!< CPUs | GPUs for vulkan.
    
 private:
  /// Gets all the physical devices available, and returns a vector of the
  /// devices.
  std::vector<VkPhysicalDevice> getPhysicalDevices() const;
};

namespace {

/// Checks if a Vulkan Physical Device matches a device specifier types. 
/// Returns true if the physical device meets the type requirements of the 
/// specifier.
///
/// \param vkPhysicalDevice The vulkan physical device to check.
/// \param deviceSpecifier  The specifier to try and find a match with.
bool physicalDeviceTypeIsCorrect(const VkPhysicalDevice& vkPhysicalDevice, 
                                 const DeviceSpecifier&  deviceSpecifier) {
  VkPhysicalDeviceProperties physicalDeviceProperties = {};
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &physicalDeviceProperties);

  if (static_cast<uint8_t>(physicalDeviceProperties.deviceType) ==
      static_cast<uint8_t>(deviceSpecifier.deviceType)          ) {
    return true;
  }
  if (deviceSpecifier.deviceType == DeviceType::VW_ANY)
    return true;

  return false;
}

} // annonymous namespace 

//---- Template Implementations ---------------------------------------------//

template <typename SpecifierType, typename... SpecifierTypes, typename>
DeviceFilter::DeviceFilter(UniqueInstance instance,
    SpecifierType& deviceSpecifier, SpecifierTypes&... deviceSpecifiers)
:   Instance(std::move(instance)), PhysicalDevices(0) { 
  auto physicalDevices = getPhysicalDevices();
  
  // Make a vector of the specifiers.
  auto specifierVec = 
    std::vector<SpecifierType>{deviceSpecifier, deviceSpecifiers...};

  // Go through the physical devices and add those which match the specifier
  for (auto& physicalDevice : physicalDevices) {
    for (auto& specifier : specifierVec) {
      if (!physicalDeviceTypeIsCorrect(physicalDevice, specifier))
        continue;  // Go to next iteration if the device type is incorrect.

      if (addIfQueuesAreSupported(physicalDevice, specifier.queueTypes, 
            specifier.mustSupportAllQueues)) {
        specifier.valid = true;
      } else {
        specifier.valid = false;
      }
    }
  }
}

} // namespace vwrap
} // namespace vs

#endif  // VISIOSOFT_VULKAWRAP_DEVICE_SELECTOR_H
