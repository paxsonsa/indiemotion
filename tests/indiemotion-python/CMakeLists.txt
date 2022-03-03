# Add Python Module Testing
add_test(NAME test_indiemotion_python
         COMMAND ${Python_EXECUTABLE} -m unittest
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set_tests_properties(test_indiemotion_python
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/python:$ENV{PYTHONPATH}")

# Add c++ tests
function(MakeTestFromName NAME)
    add_executable(${NAME} tests/${NAME}.cpp)
    message("Current Source Dir: ${CMAKE_CURRENT_BINARY_DIR}")
    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${NAME}
            #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
            #libraries/targets to link when linking this library
            #this will automatically setup the needed flags and dependencies when linking against this target
            PUBLIC # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
            doctest::doctest
            indiemotion
            indiemotion::python
            )
    message("adding ${NAME}")
    add_test(
            NAME IndieMotion.${NAME}
            COMMAND ${NAME} ${TEST_RUNNER_PARAMS}
    )
    target_compile_features(${NAME}  PRIVATE cxx_std_17)
    target_set_warnings(${NAME} ENABLE pedantic unused DISABLE deprecated-declarations)
endfunction()


#MakeTestFromName(test_pydelegate)
#MakeTestFromName(test_context)
