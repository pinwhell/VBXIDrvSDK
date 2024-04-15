file(GLOB VBXIDrvSDK_SRC *.cpp)

add_library(VBXIDrvSDK STATIC ${VBXIDrvSDK_SRC})
target_include_directories(VBXIDrvSDK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(VBXIDrvSDK etl fslc nmd TBS)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_compile_definitions(VBXIDrvSDK PUBLIC BITS32)
else()
    target_compile_definitions(VBXIDrvSDK PUBLIC BITS64)
endif()

target_compile_options(VBXIDrvSDK INTERFACE -mno-sse -fno-inline -U_FORTIFY_SOURCE -fno-threadsafe-statics -fno-exceptions -fno-stack-protector)

# Forcing Linking No Discard of Entry/Exit Points
add_library(VBXIDrvSDKEntry STATIC Entry.cxx)
target_link_libraries(VBXIDrvSDK $<LINK_LIBRARY:WHOLE_ARCHIVE,VBXIDrvSDKEntry>) 
target_compile_options(VBXIDrvSDKEntry INTERFACE -fno-exceptions -fno-stack-protector)
