#!/usr/bin/env bash

export CMAKE_C_COMPILER=/usr/local/Cellar/gcc/8.3.0_2/bin/gcc-8
export CMAKE_CXX_COMPILER=/usr/local/Cellar/gcc/8.3.0_2/bin/g++-8

export CJSON_VERSION=1.7.11
export ZF_LOG_VERSION=0.4.0
export LWS_VERSION=3.1-stable

export ROOT_WORKING_DIR=$(pwd)

export ROOT_WORKING_DIR=$(pwd)
rm -rf build/lws && \
    mkdir -p build && cd build && \
    git clone --depth 1 --branch v${LWS_VERSION} --single-branch git@github.com:warmcat/libwebsockets.git lws && \
    cd lws && \
    cmake . \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${ROOT_WORKING_DIR}/install/libwebsockets/${LWS_VERSION} \
        -DLWS_IPV6=ON \
        -DLWS_WITHOUT_BUILTIN_GETIFADDRS=ON \
        -DLWS_WITHOUT_EXTENSIONS=ON \
        -DLWS_WITHOUT_TESTAPPS=ON \
        -DLWS_WITH_STATIC=ON \
        -DLWS_WITH_ZIP_FOPS=OFF \
        -DLWS_WITH_ZLIB=OFF \
        -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl \
        -DCMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE=/usr/local/opt/openssl \
        -DOPENSSL_LIBRARIES="/usr/local/opt/openssl/lib/libcrypto.dylib;/usr/local/opt/openssl/lib/libssl.dylib" \
        -DOPENSSL_INCLUDE_DIRS=/usr/local/opt/openssl/include && \
    make install

cd ${ROOT_WORKING_DIR}
wget https://github.com/DaveGamble/cJSON/archive/v${CJSON_VERSION}.tar.gz -O /tmp/cjson.tar.gz && \
    rm -rf build/cjson && \
    mkdir -p build/cjson && \
    tar --strip=1 -xf /tmp/cjson.tar.gz -C build/cjson && \
    rm /tmp/cjson.tar.gz && \
    cd build/cjson && \
    cmake . \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DENABLE_CJSON_UTILS=Off \
        -DENABLE_CJSON_TEST=Off \
        -DBUILD_SHARED_AND_STATIC_LIBS=On \
        -DCMAKE_INSTALL_PREFIX=${ROOT_WORKING_DIR}/install/cJSON/${CJSON_VERSION} && \
    make install

cd ${ROOT_WORKING_DIR}
wget https://github.com/wonder-mice/zf_log/archive/v${ZF_LOG_VERSION}.tar.gz -O /tmp/zf_log.tar.gz && \
    rm -rf build/zf_log && \
    mkdir -p build/zf_log && \
    tar --strip=1 -xf /tmp/zf_log.tar.gz -C build/zf_log && \
    rm /tmp/zf_log.tar.gz && \
    cd build/zf_log && \
    cmake . \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${ROOT_WORKING_DIR}/install/zf_log/${ZF_LOG_VERSION} && \
    make install
