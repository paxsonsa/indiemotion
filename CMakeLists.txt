cmake_minimum_required(VERSION 3.20)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    if (NOT DEFINED WITH_IN_SOURCE_BUILD)
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
    endif ()
endif ()

project(
        "indiemotion"
        VERSION 0.1.0
        LANGUAGES C CXX
)
# -----------------------------------------------------------------------------
# Build Options
# -----------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Select C++17 as the standard for C++ projects.
set(CMAKE_CXX_STANDARD 17)
# If C++17 is not available, downgrading to an earlier standard is NOT OK.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Do not enable compiler specific language extensions.
set(CMAKE_CXX_EXTENSIONS OFF)

set(indiemotion_INSTALL_BINDIR "bin" CACHE STRING "Installation directory for executables")
set(indiemotion_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
set(indiemotion_INSTALL_INCLUDEDIR "include" CACHE STRING "Installation directory for headers")
set(indiemotion_INSTALL_CMAKEDIR "lib/cmake/${PACKAGE_NAME}" CACHE STRING "Installation directory for cmake config files")
set(indiemotion_INSTALL_SHAREDIR "share/indiemotion" CACHE STRING "Installation directory for root certificates")

# -----------------------------------------------------------------------------
#  Modules and scripts
# -----------------------------------------------------------------------------
# Standard CMake modules
#include(CTest)                          # Must be called before adding tests but after calling project(). This automatically calls enable_testing() and configures ctest targets when using Make/Ninja
include(CMakeDependentOption)           # This is a really useful scripts that creates options that depends on other options. It can even be used with generator expressions !
include(GNUInstallDirs)                 # This will define the default values for installation directories (all platforms even if named GNU)
include(InstallRequiredSystemLibraries) # Tell CMake that the `install` target needs to install required system libraries (eg: Windows SDK)
include(CMakePackageConfigHelpers)      # Helper to create relocatable packages

# -----------------------------------------------------------------------------
# Custom modules and scripts
# -----------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") # Make our cmake scripts available
include(LTO)
include(Warnings)
include(CopyDllsForDebug)

# -----------------------------------------------------------------------------
# Find Core Packages
# -----------------------------------------------------------------------------
add_subdirectory(vendor)
find_package(Boost 1.76.0 REQUIRED python program_options)

add_library(indiemotion
        include/indiemotion/server.hpp
        src/server/server.cpp
        include/indiemotion/impl/server_runtime.hpp
        include/beast.hpp include/net.hpp
        src/server/server_runtime.cpp
        src/server/http_session.cpp
        src/server/http_session.hpp
        src/server/websocket.cpp
        src/server/websocket.hpp
        )
add_library(indiemotion::indiemotion ALIAS indiemotion)
set_target_properties(indiemotion PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(indiemotion PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A # Version "A" is macOS convention
        MACOSX_FRAMEWORK_IDENTIFIER com.andrewpaxson.indiemotion
        )
target_compile_features(indiemotion PUBLIC cxx_std_17)
target_include_directories(indiemotion
        PUBLIC $<INSTALL_INTERFACE:${indiemotion_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
target_link_libraries(indiemotion
        PUBLIC
        general
        Boost::boost
        spdlog::spdlog
        fmt::fmt
        )

add_executable(imserver src/imserver/main.cpp)
target_link_libraries(imserver
        PUBLIC
        indiemotion
        fmt::fmt
        )
target_compile_features(imserver PUBLIC cxx_std_17)
