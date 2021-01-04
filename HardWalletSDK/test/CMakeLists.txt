cmake_minimum_required(VERSION 3.0.0)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/test/JUB_SDK_test)

include_directories(${CMAKE_SOURCE_DIR}/deps/jsoncpp/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/deps/jsoncpp/src/lib_json SRC_LIST)

if(MSVC)   # msvc

    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")   #x86

        FIND_LIBRARY(HID_API_LIBRARY_RELEASE
        NAMES hidapi
        PATHS ${CMAKE_SOURCE_DIR}/deps/hidapi/libhidapi
        )

        FIND_LIBRARY(HID_API_LIBRARY_DEBUG
        NAMES hidapi_d
        PATHS ${CMAKE_SOURCE_DIR}/deps/hidapi/libhidapi
        )

    elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8") #x64

        FIND_LIBRARY(HID_API_LIBRARY_RELEASE
        NAMES hidapi
        PATHS ${CMAKE_SOURCE_DIR}/deps/hidapi/libhidapi/x64
        )

        FIND_LIBRARY(HID_API_LIBRARY_DEBUG
        NAMES hidapi_d
        PATHS ${CMAKE_SOURCE_DIR}/deps/hidapi/libhidapi/x64
        )

    endif()

    SET(HID_API_LIBRARY debug ${HID_API_LIBRARY_DEBUG} optimized ${HID_API_LIBRARY_RELEASE})

endif()

add_executable(JUB_test "JUB_SDK_test/JUB_SDK_test.cpp" ${SRC_LIST})

#user lib
if(msvc)
    target_link_libraries(JUB_test PRIVATE JUB_SDK TrezorCrypto ${HID_API_LIBRARY} Setupapi)
elseif(APPLE)
    FIND_LIBRARY(HID_API_LIB_MAC_STATIC
        NAMES libhidapi.a
    )
    target_link_libraries(JUB_test PRIVATE JUB_SDK TrezorCrypto ${HID_API_LIB_MAC_STATIC}  "-framework CoreFoundation" "-framework IOKit")
else()
    target_link_libraries(JUB_test PRIVATE JUB_SDK TrezorCrypto hidapi-hidraw)  
endif(msvc)



