set(NO_STL ON)

add_subdirectory(etl)

set(ENABLE_STANDARD_SYMNAMES OFF)

add_subdirectory(FsLibc)

target_compile_definitions(etl INTERFACE ETL_NO_STL ETL_DISABLE_STRING_CLEAR_AFTER_USE ETL_DISABLE_STRING_TRUNCATION_CHECKS)

add_library(nmd STATIC nmd.cpp)
target_include_directories(nmd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nmd)

set (TBS_MT OFF)		# No Multi-thread.
set (TBS_USE_ETL ON)	# Use ETL

add_subdirectory(TBS)