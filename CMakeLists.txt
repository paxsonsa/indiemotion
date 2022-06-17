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
find_package(Boost 1.76.0 REQUIRED program_options)
include(FindProtobuf)
find_package(Protobuf REQUIRED)

add_library(indiemotion SHARED
        include/beast.hpp
        include/net.hpp
        src/context.cpp
        src/server/server.cpp
        src/server/message.cpp
        src/server/connection.cpp
        src/server/http_session.cpp
        src/server/http_session.hpp
        src/server/websocket.cpp
        src/server/websocket.hpp
        include/indiemotion/error.hpp
        include/indiemotion/server.hpp
        include/indiemotion/message.hpp
        include/indiemotion/runtime.hpp
        include/indiemotion/context.hpp
        include/indiemotion/callbacks.hpp
        include/indiemotion/connection.hpp
        include/indiemotion/message_gen.hpp
        include/indiemotion/proto/messages.pb.h
        include/indiemotion/proto/messages.pb.cc
        src/server/message_gen.cpp include/indiemotion/engine.hpp src/engine.cpp include/indiemotion/controllers/controller.hpp include/indiemotion/controllers/debug.hpp src/controllers/debug.cpp include/indiemotion/controllers/session.hpp src/controllers/session.cpp)
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
        protobuf::libprotobuf
        )

add_executable(imserver src/imserver/main.cpp)
target_compile_features(imserver PUBLIC cxx_std_17)
target_link_libraries(imserver
        PUBLIC
        indiemotion
        fmt::fmt
        )

add_executable(mgen src/mgen/main.cpp)
target_compile_features(mgen PUBLIC cxx_std_17)
target_link_libraries(mgen
        PUBLIC
        indiemotion
        fmt::fmt
        )

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
# Enable Testing before generating sources to ensure the tests in the submodules are found
include(GoogleTest)
set(TEST_RUNNER_PARAMS "--force-colors=false" CACHE STRING "Options to add to our test runners commands")
enable_testing()

add_executable(test_context src/test_context.cpp)
target_compile_features(test_context PUBLIC cxx_std_17)
target_link_libraries(test_context
        PUBLIC
        indiemotion
        gtest
        gtest_main
)
gtest_discover_tests(test_context)
