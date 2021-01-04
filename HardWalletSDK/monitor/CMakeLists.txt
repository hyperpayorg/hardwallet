cmake_minimum_required(VERSION 3.0.0)

add_executable(monitor "monitor.cpp")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")

#user lib
if(msvc)
    target_link_libraries(monitor PRIVATE JUB_SDK ${HID_API_LIBRARY} Setupapi)
elseif(APPLE)
    FIND_LIBRARY(HID_API_LIB_MAC_STATIC
        NAMES libhidapi.a
    )
    target_link_libraries(monitor PRIVATE JUB_SDK usb-1.0 ${HID_API_LIB_MAC_STATIC}  "-framework CoreFoundation" "-framework IOKit")
else()
    target_link_libraries(monitor PRIVATE JUB_SDK usb-1.0 hidapi-hidraw "-pthread")
endif(msvc)


