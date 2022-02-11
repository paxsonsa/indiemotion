#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <boost/python.hpp>
#include <indiemotion-py/delegate.hpp>

namespace python = boost::python;
// TODO Sketch out testing and workflow for server and delegate testing.
// https://stackoverflow.com/questions/36049533/sending-python-function-as-boost-function-argument

TEST_CASE("PyDelegate calls PyObject functions as expected")
{

}