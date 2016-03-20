# --------------------   CMakeLists.txt for VulkaWrap    -------------------- #

cmake_minimum_required ( VERSION 3.1 )
project ( Vulkawrap )


# --------------------              Versioning           -------------------- #

set ( VULKAWRAP_MAJOR_VERSION 0 )
set ( VULKAWRAP_MINOR_VERSION 1 )
set ( VULKAWRAP_PATCH_VERSION 0 )
set ( VULKAWRAP_VERSION 
  ${VULKAWRAP_MAJOR_VERSION}.${VULKAWRAP_MINOR_VERSION}.${VULKAWRAP_PATCH_VERSION}
)

# --------------------      Installation Directories     -------------------- #

set ( INSTALL_LIB_DIR     lib               CACHE PATH 
  "Installation directory for libraries"  
)
set ( INSTALL_BIN_DIR     bin               CACHE PATH 
  "Installation directory for binaries"   
)
set ( INSTALL_INCLUDE_DIR include/visiosoft CACHE PATH                                
 "Installation firectory for header files"
)

IF(WIN32 AND NOT CYGWIN)
  set ( DEF_INSTALL_CMAKE_DIR CMake )
ELSE()
  set ( DEF_INSTALL_CMAKE_DIR lib/CMake/Visiosoft )
ENDIF()
set ( INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH 
  "Installation directory for CMake files"
)

FOREACH( pth in LIB BIN INCLUDE CMAKE )
  set (var INSTALL_${pth}_DIR )
  IF( NOT IS_ABSOLUTE "${${var}}")
    set ( ${var} "${CMAKE_INSTALL_PREFIX}/${${var}}" )
  ENDIF()
ENDFOREACH()

# --------------------        Header Only libraries      -------------------- #

install ( DIRECTORY ${Vulkawrap_SOURCE_DIR}/include/vulkawrap/util/
  DESTINATION ${INSTALL_INCLUDE_DIR}/util COMPONENT util
)

# --------------------        Include Directories        -------------------- #

include_directories ( 
  ${Vulkawrap_SOURCE_DIR}/include
)


# --------------------        Define Subdirectories      -------------------- #

add_subdirectory ( src )
add_subdirectory ( examples )
add_subdirectory ( doc      )

# --------------------          Compiler Flags           -------------------- #

IF(WIN32)
  set ( CMAKE_CXX_FLAGS "/O2" CACHE STRING "compile flags" FORCE         )
  set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR" )
ELSE()
  set ( CMAKE_CXX_FLAGS "-std=c++14 -g -pedantic -Wall" CACHE STRING 
        "compile flags" FORCE
  )
  set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR" CACHE
        STRING "compile flags" FORCE
  )
ENDIF()

set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES" )

IF(APPLE)
  set ( XCB_INCLUDE_DIRS /opt/X11/include )
ENDIF(APPLE)

message ( "CMAKE_CXX_FLAGS are ${CMAKE_CXX_FLAGS}"  )
message ( "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}" )

# --------------------           Uinstall Rule           -------------------- #

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY 
)

add_custom_target (
  uninstall 
    COMMAND 
  ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# --------------------------------------------------------------------------- #


