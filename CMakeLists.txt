cmake_minimum_required(VERSION 3.13)
project(ccxs C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra  -std=c11")
#-Werror

find_package(ZLIB REQUIRED)

find_package(OpenSSL REQUIRED)
if (OpenSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_directories(${OPENSSL_LIBRARIES})
    message(STATUS "Using OpenSSL include ${OPENSSL_INCLUDE_DIRS}")
    message(STATUS "Using OpenSSL libraries ${OPENSSL_LIBRARIES}")
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif ()

if (APPLE)
    message(STATUS "APPLE=${APPLE}")
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    set(OPENSSL_INCLUDE_DIRS "/usr/local/opt/openssl/include")
    set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE "/usr/local/opt/openssl")
    set(OPENSSL_LIBRARIES "/usr/local/opt/openssl/lib/libcrypto.dylib;/usr/local/opt/openssl/lib/libssl.dylib")
    message(STATUS "OPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}")
    include_directories(${OPENSSL_INCLUDE_DIRS})
endif ()

add_library(cjson_lib SHARED IMPORTED)
set_target_properties(cjson_lib PROPERTIES IMPORTED_LOCATION /build/install/cJSON/1.7.11/lib/libcjson.a)

add_library(lws_lib SHARED IMPORTED)
set_target_properties(lws_lib PROPERTIES IMPORTED_LOCATION /build/install/libwebsockets/3.1.0/lib/libwebsockets.so)

include_directories(
        /build/install/cJSON/1.7.11/include
        /build/install/libwebsockets/3.1.0/include
)

add_subdirectory(src)

add_subdirectory(test)
