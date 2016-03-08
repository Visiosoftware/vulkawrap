# --------------------   CmakeLists.txt for VulkaWrap    -------------------- #
# --------------------            Libraries              -------------------- #

# --------------------      Try and find Modules         -------------------- #

# Make sure that the required boost libraries are installed.
find_package (Boost COMPONENTS 
                system 
                filesystem 
                unit_test_framework
              REQUIRED)

# Make sure that Vulkan is installed.
IF(WIN32)
ELSE(WIN32)
  find_library(VULKAN_LIB NAMES libvulkan.so PATHS /usr/lib) 
ENDIF(WIN32)

message(STATUS ${VULKAN_LIB})

include_directories (${VulkaWrap_SOURCE_DIR}/include)

# --------------------     Make libraries in subdirs     -------------------- # 

add_library (VwDeviceSelector visiosoft/vulkawrap/device/selector.cc)

# --------------------------------------------------------------------------- #
