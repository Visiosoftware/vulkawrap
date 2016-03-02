# --------------------   CmakeLists.txt for snap tests   -------------------- #

IF(WIN32)
	set (BOOST_ROOT            C:/Program Files/boost)
	set (BOOST_LIBRARYDIR      C:/Program Files/boost/lib)
	set (Boost_USE_STATIC_LIBS ON)
ELSEIF(APPLE)
	set (BOOST_ROOT            /opt/boost_1_60)
	set (BOOST_LIBRARYDIR      /opt/boost_1_60/lib)
  set (Boost_USE_STATIC_LIBS OFF)
ENDIF()

# --------------------      Try and find Modules         -------------------- #

# Make sure that the required boost libraries are installed
find_package (Boost COMPONENTS 
                system 
                filesystem 
                unit_test_framework
              REQUIRED)

IF(APPLE)
  set (XCB_INCLUDE_DIRS /opt/X11/include)
ENDIF(APPLE)

IF(WIN32)
ELSE(WIN32)
  find_library(VULKAN_LIB NAMES libvulkan.so PATHS 
               /usr/lib) 
ENDIF(WIN32)

message(STATUS ${VULKAN_LIB})

include_directories ( ${VulkanExamples_SOURCE_DIR}/src
                      ${VulkanExamples_SOURCE_DIR}/src/vulkan
                      ${VulkanExamples_SOURCE_DIR}/src/utilities
                      ${VulkanExamples_SOURCE_DIR}/src/widget
                      ${XCB_INCLUDE_DIRS}
                    )
# --------------------     Make libraries in subdirs     -------------------- # 

add_library(VkBasic SHARED vulkan/vulkan_basic.cpp)
add_library(VkUtilities SHARED utilities/vulkan_utils.cpp)
add_library(VkBase SHARED vulkan/vulkan_base.cpp)
link_libraries(xcb ${VULKAN_LIB})
target_link_libraries (VkBasic VkUtilities VkBase ${VULKAN_LIB})

# --------------------   Set Example Bin Directory       -------------------- #

set (example_exe_dir bin/)

# --------------------      Function to make a test      -------------------- #

function (MakeExample ExampleName ExampleFiles ExampleLibs ExampleBinDir)
  # Add all the example .cpp files for the examples
  add_executable (${${ExampleName}} ${${ExampleFiles}})

  # Link all the libraries which the Diagnostics module uses
  target_link_libraries ( ${${ExampleName}}
                          ${${ExampleLibs}}
                        )

  # Move the test binary into the TestsBin directory
  set_target_properties ( ${${ExampleName}} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                          ${VulkanExamples_SOURCE_DIR}/${${ExampleBinDir}}
                        )

  # Add the test to the test execution
  add_test (  NAME ${${ExampleName}} WORKING_DIRECTORY
              ${VulkanExamples_SOURCE_DIR}/${${ExampleBinDir}} COMMAND
              ${VulkanExamples_SOURCE_DIR}/${${ExampleBinDir}}/${${ExampleName}}
          )
endfunction()

# --------------------        Diagnostics Tests          -------------------- #

set (example_exe       triangle             )
set (example_files     triangle/triangle.cpp) 
set (example_libs      VkBasic VkUtilities ${VULKAN_LIB})

MakeExample (example_exe example_files example_libs example_exe_dir)

# --------------------------------------------------------------------------- #
