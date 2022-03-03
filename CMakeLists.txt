cmake_minimum_required(VERSION 3.20)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  if(NOT DEFINED WITH_IN_SOURCE_BUILD)
    message(FATAL_ERROR
      "CMake generation for indiemotion is not allowed within the source directory!"
      "\n Remove \"${CMAKE_SOURCE_DIR}/CMakeCache.txt\" and try again from another folder, e.g.:"
      "\n "
      "\n rm CMakeCache.txt"
      "\n mkdir build"
      "\n cd build"
      "\n cmake .."
      "\n "
    )
  endif()
endif()

project(
    "indiemotion"
    VERSION 1.0.0
    LANGUAGES C CXX
)

# -----------------------------------------------------------------------------
# Build Options
# -----------------------------------------------------------------------------
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
# Select C++17 as the standard for C++ projects.
set(CMAKE_CXX_STANDARD 17)
# If C++17 is not available, downgrading to an earlier standard is NOT OK.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Do not enable compiler specific language extensions.
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------------------------
#  Modules and scripts
# -----------------------------------------------------------------------------
# Standard CMake modules
#include(CTest)                          # Must be called before adding tests but after calling project(). This automatically calls enable_testing() and configures ctest targets when using Make/Ninja
include(CMakeDependentOption)           # This is a really useful scripts that creates options that depends on other options. It can even be used with generator expressions !
include(GNUInstallDirs)                 # This will define the default values for installation directories (all platforms even if named GNU)
include(InstallRequiredSystemLibraries) # Tell CMake that the `install` target needs to install required system libraries (eg: Windows SDK)
include(CMakePackageConfigHelpers)      # Helper to create relocatable packages

# Custom modules and scripts

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") # Make our cmake scripts available

include(LTO)
include(Warnings)
include(CopyDllsForDebug)
# -----------------------------------------------------------------------------
# Project Options
# -----------------------------------------------------------------------------
cmake_dependent_option(${PROJECT_NAME}_BUILD_TESTS
    "Enable ${PROJECT_NAME} project tests targets" ON # By default we want tests if CTest is enabled
    "BUILD_TESTING" ON # Stay coherent with CTest variables
)

option(${PROJECT_NAME}_BUILD_PYTHON
    "Build the python shared modules" ON # By default we want tests if CTest is enabled
)

# Enable Testing before generating sources to ensure the tests in the submodules are found
set(TEST_RUNNER_PARAMS "--force-colors=false" CACHE STRING "Options to add to our test runners commands")
enable_testing()

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
#find_package(Boost 1.76.0 REQUIRED program_options)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#endif()

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost 1.76.0 REQUIRED python)
find_package(Python 3.9 COMPONENTS Development Interpreter)
add_subdirectory(external EXCLUDE_FROM_ALL)

# -----------------------------------------------------------------------------
# Libraries and Executables
# -----------------------------------------------------------------------------
add_library(indiemotion-protobuf
        include/indiemotion-protobuf/messages.pb.h
        include/indiemotion-protobuf/messages.pb.cc
        include/indiemotion-protobuf/payloads.pb.h
        include/indiemotion-protobuf/payloads.pb.cc
        )
set_target_properties(indiemotion-protobuf PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A # Version "A" is macOS convention
        MACOSX_FRAMEWORK_IDENTIFIER com.andrewpaxson.indiemotionpb
        )
target_link_libraries(indiemotion-protobuf
        #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
        #libraries/targets to link when linking this library
        #this will automatically setup the needed flags and dependencies when linking against this target
        PUBLIC # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
        protobuf::libprotobuf
        )


add_library(indiemotion source/indiemotion/indiemotion.cpp)
add_library(indiemotion::indiemotion ALIAS indiemotion)
target_compile_features(indiemotion PUBLIC cxx_std_17)
target_include_directories(indiemotion PUBLIC include)
target_include_directories(indiemotion PUBLIC include/indiemotion-protobuf)
set_target_properties(indiemotion PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A # Version "A" is macOS convention
        MACOSX_FRAMEWORK_IDENTIFIER com.andrewpaxson.indiemotion
        )

target_link_libraries(indiemotion
        PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
        general
        Boost::boost
        indiemotion-protobuf
        spdlog::spdlog
        fmt::fmt
        #libraries/targets to link when linking this library
        #this will automatically setup the needed flags and dependencies when linking against this target
#        INTERFACE # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
        )

install(TARGETS indiemotion EXPORT IndieMotionTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        FRAMEWORK DESTINATION lib
        INCLUDES DESTINATION include
        )
if (${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests/indiemotion)
endif()


if (${PROJECT_NAME}_BUILD_PYTHON)
  add_library(indiemotion-python MODULE
              include/indiemotion-protobuf/messages.pb.cc
              include/indiemotion-protobuf/payloads.pb.cc
              source/indiemotion-python/python.cpp)
  target_include_directories(indiemotion-python PUBLIC include)
  add_library(indiemotion::python ALIAS indiemotion-python)

  set_target_properties(indiemotion-python PROPERTIES OUTPUT_NAME "indiemotion")
  set_target_properties(indiemotion-python PROPERTIES PREFIX "")
  target_link_libraries(indiemotion-python PUBLIC
          general
          indiemotion
          Boost::python
          Python::Module
          Python::Python
  )

  # Make sure .so files are copied into python package
  set_target_properties(indiemotion-python
          PROPERTIES
          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
          )
  if (${PROJECT_NAME}_BUILD_TESTS)
    add_subdirectory(tests/indiemotion-python)
  endif()

endif()

if(${${PROJECT_NAME}_BUILD_CLI})
  add_executable(indiemotion-debug-server
          source/indiemotion-debug-server/main.cpp
)
  target_link_libraries(indiemotion-debug-server
          #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
          #libraries/targets to link when linking this library
          #this will automatically setup the needed flags and dependencies when linking against this target
          PRIVATE # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
          indiemotion
          general fmt::fmt
          spdlog::spdlog
          Boost::program_options
          #debug DEBUGLIBS
          #optimized RELEASELIBS
          )
  target_compile_features(indiemotion-debug-server  PRIVATE cxx_std_17)
  target_set_warnings(indiemotion-debug-server ENABLE pedantic unused)
endif()