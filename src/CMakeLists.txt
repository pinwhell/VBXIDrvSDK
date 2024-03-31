file(GLOB VBXIDrvSDK_SRC *.cpp)

add_library(VBXIDrvSDK STATIC ${VBXIDrvSDK_SRC})
target_include_directories(VBXIDrvSDK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(VBXIDrvSDK etl fslc)

# Forcing Linking No Discard of Entry/Exit Points
add_library(VBXIDrvSDKEntry STATIC Entry.cxx)
target_link_libraries(VBXIDrvSDK $<LINK_LIBRARY:WHOLE_ARCHIVE,VBXIDrvSDKEntry>) 