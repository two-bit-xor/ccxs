cmake_minimum_required(VERSION 3.14)
project(cJSON_example C)

set(CMAKE_C_STANDARD 11)

find_package(openssl REQUIRED)

if (APPLE)
    message(STATUS "APPLE=${APPLE}")
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    set(OPENSSL_INCLUDE_DIRS "/usr/local/opt/openssl/include")
    set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE "/usr/local/opt/openssl")
    set(OPENSSL_LIBRARIES "/usr/local/opt/openssl/lib/libcrypto.dylib;/usr/local/opt/openssl/lib/libssl.dylib")
    message(STATUS "OPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}")
    include_directories(${OPENSSL_INCLUDE_DIRS})
endif ()

add_library(zf_log_lib STATIC IMPORTED)
set_target_properties(zf_log_lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/install/zf_log/0.4.0/lib/libzf_log.a)

add_library(cjson_lib STATIC IMPORTED)
set_target_properties(cjson_lib PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/install/cJSON/1.7.11/lib/libcjson.a)
include_directories(install/zf_log/0.4.0/include install/cJSON/1.7.11/include)

add_executable(cJSON_example main.c src/binance-depth.c)
target_link_libraries(cJSON_example cjson_lib zf_log_lib)


