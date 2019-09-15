#!/usr/bin/env bash

echo "CMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
echo "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"

export CJSON_VERSION=1.7.11
export LWS_VERSION=3.1.0
export CMOCKA_VERSION=1.1.5

if [ -z "${ROOT_WORKING_DIR}" ]
then
  export ROOT_WORKING_DIR=/build
fi

echo "ROOT_WORKING_DIR=${ROOT_WORKING_DIR}"
if [ ! -d "${ROOT_WORKING_DIR}" ]
then
  mkdir -p ${ROOT_WORKING_DIR}
fi

rm -rf ${ROOT_WORKING_DIR}/lws && \
    wget https://github.com/warmcat/libwebsockets/archive/v${LWS_VERSION}.tar.gz -O /tmp/libwebsockets.tar.gz && \
    rm -rf ${ROOT_WORKING_DIR}/libwebsockets && \
    mkdir -p ${ROOT_WORKING_DIR}/libwebsockets && \
    tar --strip=1 -xf /tmp/libwebsockets.tar.gz -C ${ROOT_WORKING_DIR}/libwebsockets && \
    rm /tmp/libwebsockets.tar.gz && \
    cd ${ROOT_WORKING_DIR}/libwebsockets && \
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
        -DCMAKE_BUILD_TYPE=DEBUG \
        -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl \
        -DCMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE=/usr/local/opt/openssl \
        -DLWS_SSL_SERVER_WITH_ECDH_CERT=1 && \
    make install

cd ${ROOT_WORKING_DIR}
wget https://github.com/DaveGamble/cJSON/archive/v${CJSON_VERSION}.tar.gz -O /tmp/cjson.tar.gz && \
    rm -rf ${ROOT_WORKING_DIR}/cjson && \
    mkdir -p ${ROOT_WORKING_DIR}/cjson && \
    tar --strip=1 -xf /tmp/cjson.tar.gz -C ${ROOT_WORKING_DIR}/cjson && \
    rm /tmp/cjson.tar.gz && \
    cd ${ROOT_WORKING_DIR}/cjson && \
    cmake . \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DENABLE_CJSON_UTILS=Off \
        -DENABLE_CJSON_TEST=Off \
        -DBUILD_SHARED_AND_STATIC_LIBS=On \
        -DCMAKE_INSTALL_PREFIX=${ROOT_WORKING_DIR}/install/cJSON/${CJSON_VERSION} && \
    make install

cd ${ROOT_WORKING_DIR}
wget https://gitlab.com/cmocka/cmocka/-/archive/cmocka-${CMOCKA_VERSION}/cmocka-cmocka-${CMOCKA_VERSION}.tar.gz -O /tmp/cmocka.tar.gz && \
    rm -rf ${ROOT_WORKING_DIR}/cmocka && \
    mkdir -p ${ROOT_WORKING_DIR}/cmocka && \
    tar --strip=1 -xf /tmp/cmocka.tar.gz -C ${ROOT_WORKING_DIR}/cmocka && \
    rm /tmp/cmocka.tar.gz && \
    cd ${ROOT_WORKING_DIR}/cmocka && \
    mkdir -p build && \
    cd build && \
    cmake .. \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${ROOT_WORKING_DIR}/install/cmocka/${CMOCKA_VERSION} \
        -DCMAKE_BUILD_TYPE=Debug && \
    make install

