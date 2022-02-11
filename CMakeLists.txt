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
    "BUILD_TESTING" OFF # Stay coherent with CTest variables
)

option(${PROJECT_NAME}_BUILD_PYTHON
    "Build the python shared modules" ON # By default we want tests if CTest is enabled
)

option(${PROJECT_NAME}_BUILD_CLI "Enable/Disable the debugging CLI tools" ON)

# Enable Testing before generating sources to ensure the tests in the submodules are found
set(TEST_RUNNER_PARAMS "--force-colors=false" CACHE STRING "Options to add to our test runners commands")
enable_testing()

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(Boost 1.76.0 REQUIRED program_options)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include(FindProtobuf)
find_package(Protobuf REQUIRED)

# -----------------------------------------------------------------------------
# Libraries and Executables
# -----------------------------------------------------------------------------
add_subdirectory(indiemotionpb)
add_subdirectory(indiemotion)

if (${PROJECT_NAME}_BUILD_PYTHON)
    add_subdirectory(indiemotion-python)
endif()

if(${${PROJECT_NAME}_BUILD_CLI})
    message("Enabled CLI")
    add_subdirectory(idmserver)
endif()



if (EXISTS "${CMAKE_SOURCE_DIR}/private")
  add_subdirectory(private)
endif()
add_subdirectory(external EXCLUDE_FROM_ALL)