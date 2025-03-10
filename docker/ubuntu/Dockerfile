FROM ubuntu:20.04

# Set environment variables to avoid tzdata interactive dialog
ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

ENV CLANG_VERSION=20
#Max supported version of CMake by CLion
ENV CMAKE_VERSION=3.28.6
ENV NINJA_VERSION=v1.12.1
#Max supported version of gdb by CLion
ENV GDB_VERSION=14.1
ENV CCACHE_VERSION=4.10.2

RUN addgroup --gid 1000 docker && \
    adduser --uid 1000 --ingroup docker --home /home/docker --shell /bin/sh --disabled-password --gecos "" docker

# Update the system and install necessary packages
RUN apt update
RUN apt update --fix-missing
#install clang and lld-10 to speed up the build over default ld
#curl and ca-certificates to be able to use vcpkg
#texinfo required for GDB 14 (not anymore for 16.x I think)
RUN apt install -y --no-install-recommends --no-install-suggests \
    build-essential \
    wget \
    git \
    python3 python3-dev \
    python-is-python3 \
    pkg-config \
    zip \
    curl \
    libmpfr-dev libgmp3-dev libmpc-dev \
    libssl-dev \
    unzip \
    ca-certificates \
    texinfo \
    libwxgtk3.0-gtk3-dev \
    lsb-release wget software-properties-common gnupg \
    libexpat1-dev libipt-dev file \
    python3-distutils libstdc++-10-dev \
    valgrind

#Cmake
RUN wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz \
    && tar xvf cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz -C /usr/local --strip-components=1\
    && rm cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz

#Ninja
RUN wget --no-check-certificate https://github.com/ninja-build/ninja/releases/download/${NINJA_VERSION}/ninja-linux.zip && \
    unzip ninja-linux.zip -d /usr/local/bin/ && \
    rm ninja-linux.zip

# Clang
RUN wget https://apt.llvm.org/llvm.sh \
    && chmod +x llvm.sh \
    && ./llvm.sh ${CLANG_VERSION} \
    && apt-get install -y clang-tools-${CLANG_VERSION} \
                          clang-format-${CLANG_VERSION} \
                          clangd-${CLANG_VERSION} \
                          clang-tidy-${CLANG_VERSION} \
                          libc++-${CLANG_VERSION}-dev \
                          libc++abi-${CLANG_VERSION}-dev

RUN wget --no-check-certificate https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/ccache-${CCACHE_VERSION}-linux-x86_64.tar.xz \
    && tar -xvf ccache-${CCACHE_VERSION}-linux-x86_64.tar.xz -C /usr/local/bin --strip-components=1 \
    && update-alternatives --install /usr/bin/ccache ccache /usr/local/bin/ccache 100

#gdb
RUN update-alternatives --install /usr/local/bin/cc cc /usr/bin/clang-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/clang clang /usr/bin/clang-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/cpp ccp /usr/bin/clang++-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/c++ c++ /usr/bin/clang++-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/clang++ clang++ /usr/bin/clang++-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/ld ld /usr/bin/ld.lld-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/clang-format clang-format /usr/bin/clang-format-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/local/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 100 \
    && update-alternatives --install /usr/bin/ctest ctest /usr/local/bin/ctest 100 \
    && update-alternatives --install /usr/bin/ninja ninja /usr/local/bin/ninja 100

#gdb
RUN CXX=/usr/bin/clang && CC=/usr/bin/clang && \
    wget "http://ftp.gnu.org/gnu/gdb/gdb-${GDB_VERSION}.tar.gz" && \
    tar -xvf gdb-${GDB_VERSION}.tar.gz && \
    pushd gdb-${GDB_VERSION} && \
    mkdir build && \
    pushd build && \
    ../configure --prefix=/usr/local --with-python && \
    make -j$(nproc) && \
    make install-strip && \
    popd && \
    popd && \
    rm gdb-${GDB_VERSION}.tar.gz && \
    rm -rf gdb-${GDB_VERSION} && \
    update-alternatives --install /usr/bin/gdb gdb /usr/local/bin/gdb 100 \
    && apt remove texinfo -y

RUN wget https://bootstrap.pypa.io/get-pip.py \
    && python3 get-pip.py \
    && python3 -m pip install --upgrade pip \
    && python3 -m pip install jsonschema pandas six numpy pytest

#https://github.com/boxboat/fixuid
#Prevent messing permissions especially on mounted volumes
RUN USER=docker && \
    GROUP=docker && \
    curl -SsL https://github.com/boxboat/fixuid/releases/download/v0.6.0/fixuid-0.6.0-linux-amd64.tar.gz | tar -C /usr/local/bin -xzf - && \
    chown root:root /usr/local/bin/fixuid && \
    chmod 4755 /usr/local/bin/fixuid && \
    mkdir -p /etc/fixuid && \
    printf "user: $USER\ngroup: $GROUP\n" > /etc/fixuid/config.yml

RUN rm -rf /var/lib/apt/lists/* \
    && rm /etc/ld.so.cache \
    && ldconfig -C /etc/ld.so.cache

# Set the working directory
USER root

RUN mkdir /work
WORKDIR /work


ENTRYPOINT ["fixuid"]