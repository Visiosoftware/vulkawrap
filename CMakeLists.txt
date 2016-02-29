# --------------------       CMakeLists.txt for snap     -------------------- #

cmake_minimum_required (VERSION 3.1)
project (VulkanExamples)

# --------------------        Define Subdirectories      -------------------- #

add_subdirectory (src)

# --------------------          Compiler Flags           -------------------- #

IF(WIN32)
  set (CMAKE_CXX_FLAGS "/O2" CACHE STRING "compile flags" FORCE)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ELSE()
  set (CMAKE_CXX_FLAGS "-std=c++14 -g -pedantic -Wall" CACHE STRING 
       "compile flags" FORCE
      )
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR" CACHE
       STRING "compile flags" FORCE
      )
ENDIF()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

message ("CMAKE_CXX_FLAGS are ${CMAKE_CXX_FLAGS}")
message ("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# --------------------            Install Rule           -------------------- #

install (DIRECTORY include DESTINATION include PATTERN ".DS_Store" EXCLUDE)

# --------------------------------------------------------------------------- #


