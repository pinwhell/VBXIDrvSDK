set(NO_STL ON)

add_subdirectory(etl)
add_subdirectory(FsLibc)

add_library(nmd STATIC nmd.cpp)
target_include_directories(nmd STATIC ${CMAKE_CURRENT_SOURCE_DIR}/nmd)