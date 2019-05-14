#!/usr/bin/env bash

export CMAKE_C_COMPILER=/usr/local/Cellar/gcc/8.3.0_2/bin/gcc-8
export CMAKE_CXX_COMPILER=/usr/local/Cellar/gcc/8.3.0_2/bin/g++-8

export CJSON_VERSION=1.7.11
export ZF_LOG_VERSION=0.4.0

export ROOT_WORKING_DIR=$(pwd)

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
