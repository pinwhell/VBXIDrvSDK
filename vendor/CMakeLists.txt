set(NO_STL ON)

add_subdirectory(etl)
add_subdirectory(FsLibc)

target_compile_definitions(etl INTERFACE ETL_NO_STL ETL_DISABLE_STRING_CLEAR_AFTER_USE ETL_DISABLE_STRING_TRUNCATION_CHECKS)

add_library(nmd STATIC nmd.cpp)
target_include_directories(nmd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nmd)