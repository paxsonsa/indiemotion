//
// Created by Andrew Paxson on 2022-02-03.
//
#include <boost/python.hpp>

char const* greet()
{
	return "hello, world";
}

BOOST_PYTHON_MODULE(indiemotion)
{
	using namespace boost::python;
	def("greet", greet);
}