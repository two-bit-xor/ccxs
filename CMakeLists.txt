cmake_minimum_required(VERSION 3.13)
project(ccxs C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -std=c11")

find_package(ZLIB REQUIRED)


if (APPLE)
    message(STATUS "APPLE=${APPLE}")
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    set(OPENSSL_INCLUDE_DIRS "/usr/local/opt/openssl/include")
    set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE "/usr/local/opt/openssl")
    set(OPENSSL_LIBRARIES "/usr/local/opt/openssl/lib/libcrypto.dylib;/usr/local/opt/openssl/lib/libssl.dylib")
    message(STATUS "OPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}")
    include_directories(${OPENSSL_INCLUDE_DIRS})
else ()
    find_package(OpenSSL REQUIRED)
    if (OpenSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIRS})
        link_directories(${OPENSSL_LIBRARIES})
        message(STATUS "Using OpenSSL include ${OPENSSL_INCLUDE_DIRS}")
        message(STATUS "Using OpenSSL libraries ${OPENSSL_LIBRARIES}")
        message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
    endif ()
endif ()

if (DEFINED ENV{ROOT_WORKING_DIR})
    set(DEPENDENCY_BASE_DIR $ENV{ROOT_WORKING_DIR})
else ()
    set(DEPENDENCY_BASE_DIR /build)
endif ()

message(STATUS "Using DEPENDENCY_BASE_DIR ${DEPENDENCY_BASE_DIR}")
add_library(cjson_lib SHARED IMPORTED)
set_target_properties(cjson_lib PROPERTIES IMPORTED_LOCATION ${DEPENDENCY_BASE_DIR}/install/cJSON/1.7.11/lib/libcjson.a)

add_library(lws_lib SHARED IMPORTED)

if (APPLE)
    set_target_properties(lws_lib PROPERTIES IMPORTED_LOCATION ${DEPENDENCY_BASE_DIR}/install/libwebsockets/3.1.0/lib/libwebsockets.dylib)
else ()
    set_target_properties(lws_lib PROPERTIES IMPORTED_LOCATION ${DEPENDENCY_BASE_DIR}/install/libwebsockets/3.1.0/lib/libwebsockets.so)
endif ()

include_directories(
        ${DEPENDENCY_BASE_DIR}/install/cJSON/1.7.11/include
        ${DEPENDENCY_BASE_DIR}/install/libwebsockets/3.1.0/include
)

add_subdirectory(src)

add_subdirectory(test)
