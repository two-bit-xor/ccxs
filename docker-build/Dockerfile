FROM gcc:9.2.0

RUN apt-get -qq update
RUN apt-get -qq upgrade
RUN apt-get -qq -y install
RUN apt-get -qq -y install cmake openssl libssl-dev zlib1g zlib1g-dev ca-certificates lcov valgrind ruby
RUN gem install coveralls-lcov

ENV CMAKE_C_COMPILER=/usr/local/bin/gcc
ENV CMAKE_CXX_COMPILER=/usr/local/bin/g++
ENV DEPENDENCY_BASE_DIR=/usr/src

WORKDIR /usr/src/app
ADD pre-build.sh /usr/src/app/pre-build.sh
RUN chmod +x /usr/src/app/pre-build.sh
RUN /usr/src/app/pre-build.sh ${DEPENDENCY_BASE_DIR}
