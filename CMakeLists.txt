# --------------------   CMakeLists.txt for VulkaWrap    -------------------- #

cmake_minimum_required ( VERSION 3.1 )
project ( VulkaWrap )

# --------------------        Define Subdirectories      -------------------- #

add_subdirectory ( src )
add_subdirectory ( examples )

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

include_directories ( 
  ${VulkaWrap_SOURCE_DIR}/include
  #${XCB_INCLUDE_DIRS}
)

message ( "CMAKE_CXX_FLAGS are ${CMAKE_CXX_FLAGS}"  )
message ( "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}" )

# --------------------            Install Rule           -------------------- #

# TODO: Add install rule ...

# --------------------------------------------------------------------------- #


