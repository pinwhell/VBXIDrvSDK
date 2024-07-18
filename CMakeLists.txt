cmake_minimum_required(VERSION 3.15)

project(VBXIDrvSDK)

option(VBXIDRVSDK_BUILD_SAMPLES "Build Samples" FALSE)
option(VBXIDRVSDK_LOGFILE_ENABLE "Output Log File" FALSE)
option(VBXIDRVSDK_SYSLOG_ENABLE "Output Log to Syslog" FALSE)
set(VBXIDRVSDK_LOGFOLDER "/data/local/tmp" CACHE STRING "Folder to output the log file")
set(VBXIDRVSDK_LOGFILENAME "VBXILog.log" CACHE STRING "Filename of the log file")
    
set(BUILD_FLAGS -mno-sse -fno-inline -U_FORTIFY_SOURCE -fno-threadsafe-statics -fno-exceptions -fno-stack-protector)

add_compile_options(${BUILD_FLAGS})

add_subdirectory(vendor)
add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
