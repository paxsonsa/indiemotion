# Always list the source files explicitly, including headers so that they are listed in the IDE
# If you need to use files based on a variable value, use target_sources
find_package(Boost 1.76.0 REQUIRED)
add_library(indiemotion INTERFACE)
add_library(indiemotion::indiemotion ALIAS indiemotion)
target_compile_features(indiemotion INTERFACE cxx_std_17)
target_include_directories(indiemotion INTERFACE
    "$<BUILD_INTERFACE:${indiemotionpb_SOURCE_INCLUDE}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(indiemotion PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION A # Version "A" is macOS convention
  MACOSX_FRAMEWORK_IDENTIFIER com.andrewpaxson.indiemotion
)

target_link_libraries(indiemotion
    PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
        #libraries/targets to link when linking this library
        #this will automatically setup the needed flags and dependencies when linking against this target
    INTERFACE # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
        general
        fmt::fmt
        spdlog::spdlog
        Boost::boost
        indiemotionpb
)

install(TARGETS indiemotion EXPORT IndieMotionTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

#install(EXPORT IndieMotionTargets
#        FILE IndieMotionTargets.cmake
#        NAMESPACE indiemotion::
#        DESTINATION lib/cmake/indiemotion
#)

function(MakeTestWithPath NAME)
    add_executable(${NAME} tests/${NAME}.cpp)
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