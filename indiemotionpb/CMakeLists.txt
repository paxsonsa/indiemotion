find_package(Protobuf REQUIRED)

add_library(indiemotionpb
    include/indiemotionpb/messages.pb.h
    include/indiemotionpb/messages.pb.cc
    include/indiemotionpb/payloads.pb.h
    include/indiemotionpb/payloads.pb.cc
)
set_target_properties(indiemotionpb PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION A # Version "A" is macOS convention
    MACOSX_FRAMEWORK_IDENTIFIER com.andrewpaxson.indiemotionpb
)

target_link_libraries(indiemotionpb
        #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
        #libraries/targets to link when linking this library
        #this will automatically setup the needed flags and dependencies when linking against this target
        PUBLIC # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
            protobuf::libprotobuf
        )
SET(indiemotionpb_SOURCE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)