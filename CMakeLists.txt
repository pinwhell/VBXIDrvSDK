cmake_minimum_required(VERSION 3.20)

project(VBXIDrvSDK)

function(DriverCompilerFlagsSet)
    # Enforce the -fno-exceptions flag globally for C++ compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-stack-protector")
endfunction()

DriverCompilerFlagsSet()

option(BUILD_SAMPLES "Build Samples" ON)

add_subdirectory(vendor)
add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
