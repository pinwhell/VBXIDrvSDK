cmake_minimum_required(VERSION 3.20)

project(VBXIDrvSDK)

option(BUILD_SAMPLES "Build Samples" FALSE)

add_compile_options(-fno-exceptions -fno-stack-protector -mregparm=3 -fvisibility=hidden)

add_subdirectory(vendor)
add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
