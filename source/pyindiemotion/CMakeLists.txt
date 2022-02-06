# Find python and Boost - both are required dependencies
message("PYTHON: ${Python_LIBRARIES} ${Python_INCLUDE_DIRS}")

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
add_library(pyindiemotion MODULE pyindiemotion.cpp)
set_target_properties(pyindiemotion PROPERTIES OUTPUT_NAME "indiemotion")

# Set up the libraries and header search paths for this target
target_link_libraries(pyindiemotion ${Boost_LIBRARIES} ${Python_LIBRARIES} indiemotion)
target_include_directories(pyindiemotion PRIVATE ${Python_INCLUDE_DIRS})