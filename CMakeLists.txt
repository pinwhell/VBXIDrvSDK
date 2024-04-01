cmake_minimum_required(VERSION 3.20)

project(VBXIDrvSDK)

option(BUILD_SAMPLES "Build Samples" FALSE)

add_subdirectory(vendor)
add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
