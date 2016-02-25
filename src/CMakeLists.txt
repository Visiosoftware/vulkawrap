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

# --------------------     Make DYLIBS easy to find      -------------------- #

IF(APPLE)
  # Don't skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # When building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # Add the automatically determined parts of the RPATH which 
  # point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


  # The RPATH to be used when installing, 
  # but only if it's not a system directory
  LIST( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES 
        "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir
      )
  IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF("${isSystemDir}" STREQUAL "-1")
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

include_directories ( ${VulkanExamples_SOURCE_DIR}/src
                      ${VulkanExamples_SOURCE_DIR}/src/vulkan
                      ${XCB_INCLUDE_DIRS}
                    )
# --------------------     Make libraries in subdirs     -------------------- # 

add_library(VkBasic SHARED vulkan/vulkan_basic.cpp)
target_link_libraries ( VkBasic -lvulkan)

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
set (example_libs      -L/opt/X11/lib -lxcb VkBasic )

MakeExample (example_exe example_files example_libs example_exe_dir)

# --------------------------------------------------------------------------- #
