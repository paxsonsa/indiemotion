# Find python and Boost - both are required dependencies
message("PYTHON: ${Python_LIBRARIES} ${Python_INCLUDE_DIRS}")

# Find Python
find_package(Python 3.9 COMPONENTS Development Interpreter)
find_package(Boost 1.76 COMPONENTS python)

# Build Package Skeleton
add_custom_target(pythondir ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python/indiemotion)
add_custom_target(pythonpkg ALL COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/python/indiemotion/__init__.py DEPENDS pythondir)

add_library(indiemotion-py SHARED
            indiemotion-py/base.hpp
            indiemotion-py/context.hpp
            indiemotion-py/context.cpp
            indiemotion-py/delegate.cpp
            indiemotion-py/delegate.hpp)
add_library(indiemotion::python ALIAS indiemotion-py)

set_target_properties(indiemotion-py PROPERTIES OUTPUT_NAME "indiemotion")
set_target_properties(indiemotion-py PROPERTIES PREFIX "")
set_target_properties(indiemotion-py PROPERTIES SUFFIX ".pyd")
target_link_libraries(indiemotion-py PUBLIC Boost::python Python::Module Python::Python indiemotion)
add_dependencies(indiemotion-py pythonpkg)

# Make sure .so files are copied into python package
set_target_properties(indiemotion-py
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add Python Module Testing
add_test(NAME test_indiemotion_python
         COMMAND ${Python_EXECUTABLE} -m unittest
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set_tests_properties(test_indiemotion_python
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/python:$ENV{PYTHONPATH}")

# Add c++ tests
add_executable(test_pydelegate tests/test_pydelegate.cpp)
message("Current Source Dir: ${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(test_pydelegate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_pydelegate
        #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
        #libraries/targets to link when linking this library
        #this will automatically setup the needed flags and dependencies when linking against this target
        PUBLIC # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
        doctest::doctest
        indiemotion
        indiemotion::python
        )
message("adding test_pydelegate")
add_test(
        NAME IndieMotion.test_pydelegate
        COMMAND test_pydelegate ${TEST_RUNNER_PARAMS}
)
target_compile_features(test_pydelegate  PRIVATE cxx_std_17)
target_set_warnings(test_pydelegate ENABLE pedantic unused DISABLE deprecated-declarations)