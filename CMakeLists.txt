cmake_minimum_required(VERSION 3.20)

project(VBXIDrvSDK)

option(BUILD_SAMPLES "Build Samples" FALSE)

add_compile_options(-mno-sse -fno-inline -U_FORTIFY_SOURCE -fno-threadsafe-statics -fno-exceptions -fno-stack-protector)

add_subdirectory(vendor)
add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
