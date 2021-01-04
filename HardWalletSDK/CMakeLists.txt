cmake_minimum_required(VERSION 3.14.0)

project(JubSDK VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

if ("$ENV{PREFIX}" STREQUAL "")
    set(PREFIX "${CMAKE_SOURCE_DIR}/build/local")
else()
    set(PREFIX "$ENV{PREFIX}")
endif()

add_subdirectory(src)
add_subdirectory(trezor-crypto)
add_subdirectory(JubSDKCore)
#add_subdirectory(test)
