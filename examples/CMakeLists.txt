# -------------------    CmakeLists.txt for VulkaWrap    -------------------- #
# -------------------              Examples              -------------------- #

include_directories ( ${VulkaWrap_SOURCE_DIR}/include )

# --------------------     Set Example Bin Directory     -------------------- #

set (ExmplExeDir bin/)

# --------------------    Function to make an example    -------------------- #

function (MakeExample ExampleName ExampleFiles ExampleLibs ExampleBinDir)
  # Add all the example .cpp files for the examples
  add_executable (${${ExampleName}} ${${ExampleFiles}})

  # Link all the libraries which the Diagnostics module uses
  target_link_libraries ( ${${ExampleName}} ${${ExampleLibs}} )

  # Move the test binary into the TestsBin directory
  set_target_properties ( 
    ${${ExampleName}} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    ${VulkaWrap_SOURCE_DIR}/${${ExampleBinDir}}
  )

  # Add the test to the test execution
  #add_test ( 
  #  NAME ${${ExampleName}} WORKING_DIRECTORY
  #  ${VulkaWrap_SOURCE_DIR}/${${ExampleBinDir}} COMMAND
  #  ${VulkaWrap_SOURCE_DIR}/${${ExampleBinDir}}/${${ExampleName}}
  #)
endfunction()

# --------------------         Triangle Example          -------------------- #

set ( ExmplExe       triangle                                 )
set ( ExmplFiles     visiosoft/vulkawrap/triangle.cc          ) 
set ( ExmplLibs      VwDeviceFilter VwInstance ${VULKAN_LIB}  )

MakeExample (ExmplExe ExmplFiles ExmplLibs ExmplExeDir)

# --------------------------------------------------------------------------- #
