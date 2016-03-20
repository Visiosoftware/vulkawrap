// Main page for Vulkawrap Developer Documentation

/*!
 
@mainpage Vulkawrap

@tableofcontents

@section vulkawrap-overview Overview

===============================================================================
-------------------------------------------------------------------------------
Vulkawrap is a C++ "wrapper" for Vulkan which aims to present the API through a
more modern C++ interface. In addition, it aims to simplify the use of vulkan,
allowing users to simply and quickly get Vulkan graphics and compute
applications running, but also providing the control Vulkan gives the user when
this is what's desired.

@section vulkawrap-compatibility Compatibility 

===============================================================================
-------------------------------------------------------------------------------
Vulkawrap is designed to work on all platforms, except for OSX and iOS, since
Vulkan is not suppored on these platforms. If Vulkan is not supported by iOS
and OSX shortly, then some attempt may be made to allow the wrapper to work
through some sort of Metal converter.

The current (just under development) version will support the following 
operating systems:

- Windows 10
- Linux (Numerous distros -- primarily tested on Arch and Ubuntu)
- Android

@note If you are using OSX then you can use Vulkan through a docker container.
Such a container will be provided in the future to make the setup process
simple for anyonw in this situation.

The current version of Visiosoft uses the following language and API versions:

- C++14 
- Vulkan 1.0.3

@section vulkawrap-test Tests

===============================================================================
-------------------------------------------------------------------------------
Tests are provided which verify teh functionality of the wrapper library, as
well as to demonstrate the *intended* use of the wrapper. Before using the
wrapper, run the tests to make sure that everything is working correctly with
your system. The testing of each module is done with Boost.Unit.

@section vulkawrap-examples Documentation

===============================================================================
-------------------------------------------------------------------------------
The wrapper is well documented, with examples being provided to demonstrate
the functionality the wrapper provides. If there is any functionality which is
not well documented, please contact me as per the 
[contact info](@ref vulkawrap-contact) section.

@section vulkawrap-install Pre-requisits and Installation

===============================================================================
-------------------------------------------------------------------------------

Add installation instructions ...

@section vulkawrap-contact Contact and Contributing

===============================================================================
-------------------------------------------------------------------------------
For all information regarding Vulkawrap, contact 
[Rob](mailto:robjclucas@gmail.com).

@section visiosoft-license License

===============================================================================
-------------------------------------------------------------------------------
Vulkawrap is licensed under the very permissive MIT license, which supports both 
commercial and non-commercial use. You can find a copy of the license in the
root of the Vulkawrap repository.

@section vulkawrap-qstart Qucik Start

===============================================================================
-------------------------------------------------------------------------------
This section provides a very high-level overview of the Vulkan wrapper and
shows how simple is it to get a Vulkan application up and running using the
wrapper. It also gives *some* introduction to Vulakn itself, so as to allow
anyone unfamiliar with Vulkan to get a small application up and running.

*/
