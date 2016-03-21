//---- tests/vulkawrap/util/util_tests.cc ------------------ -*- C++ -*- ----//
//
//                                Vulkawrap
//                          
//                      Copyright (c) 2016 Rob Clucas        
//                    Distributed under the MIT License
//                (See accompanying file LICENSE or copy at
//                    https://opensource.org/licenses/MIT)
//
// ========================================================================= //
//
/// \file  util_tests.cc
/// \brief Tests the the utility functionality for Vulkawrap.
//
//---------------------------------------------------------------------------//

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
    #define BOOST_TEST_MODULE VulkawrapUtilTests
#endif

#include "vulkawrap/util/assert.hpp"
#include "vulkawrap/util/error.hpp"
#include <boost/test/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_SUITE( VulkawrapUtilSuite )

// Gets the error buffer to check if messages have been written.
struct ErrorBufferRedirector {
  ErrorBufferRedirector(std::streambuf* newBuffer) 
  : oldBuffer(std::cerr.rdbuf(newBuffer)) {
  }

  ~ErrorBufferRedirector() {
    std::cerr.rdbuf(oldBuffer);
  }
   
 private:
  std::streambuf* oldBuffer;
};

BOOST_AUTO_TEST_CASE( VulkawrapAssertPrintsErrorForAssertationFailure ) {
  boost::test_tools::output_test_stream errorStream;

  {
    ErrorBufferRedirector redirectedBuffer(errorStream.rdbuf());

    // Call the custom assert for Vulkawrap.
    vwrap::util::Assert(false, "Some assertation message\n");
  }

  BOOST_CHECK( !errorStream.is_empty(false) );
}

BOOST_AUTO_TEST_CASE( VulkawrapAssertPrintsNoErrorForAssertationSuccess ) {
  boost::test_tools::output_test_stream errorStream;

  {
    ErrorBufferRedirector redirectedBuffer(errorStream.rdbuf());

    // Call the custom assert for Vulkawrap.
    vwrap::util::Assert(true, "Some assertation message\n");
  }

  BOOST_CHECK( errorStream.is_empty(false) );
}

BOOST_AUTO_TEST_SUITE_END()
