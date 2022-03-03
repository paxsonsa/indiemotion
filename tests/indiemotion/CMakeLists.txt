function(MakeTestWithPath NAME)
    message("adding test: ${NAME}")
    add_executable(${NAME} ${NAME}.cpp)
    target_include_directories(${NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
    target_link_libraries(${NAME}
        #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
        #libraries/targets to link when linking this library
        #this will automatically setup the needed flags and dependencies when linking against this target
        PRIVATE # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
            indiemotion
            doctest::doctest
    )
    add_test(
            NAME IndieMotion.${NAME}
            COMMAND ${NAME} ${TEST_RUNNER_PARAMS}
    )
    target_compile_features(${NAME}  PRIVATE cxx_std_17)
    target_set_warnings(${NAME} DISABLE deprecated-declarations)
    target_set_warnings(${NAME} ENABLE pedantic unused DISABLE deprecated-declarations)
endfunction()

MakeTestWithPath(test_logging)
MakeTestWithPath(test_motion_xform)
MakeTestWithPath(test_service)