FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

# Versions
ENV BINUTILS_VERSION=2.43
ENV GCC_VERSION=14.2.0

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    bison \
    flex \
    libgmp-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    wget \
    ca-certificates \
    xz-utils \
    nasm \
    make \
    qemu-system-x86 \
    gdb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp

#
# Build Binutils
#
RUN wget https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz && \
    tar -xf binutils-${BINUTILS_VERSION}.tar.xz && \
    mkdir binutils-build && \
    cd binutils-build && \
    ../binutils-${BINUTILS_VERSION}/configure \
        --target=i386-elf \
        --prefix=/opt/cross \
        --disable-nls \
        --disable-werror && \
    make -j$(nproc) && \
    make install

#
# Build GCC (C uniquement)
#
RUN wget https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz && \
    tar -xf gcc-${GCC_VERSION}.tar.xz && \
    cd gcc-${GCC_VERSION} && \
    ./contrib/download_prerequisites && \
    cd .. && \
    mkdir gcc-build && \
    cd gcc-build && \
    ../gcc-${GCC_VERSION}/configure \
        --target=i386-elf \
        --prefix=/opt/cross \
        --disable-nls \
        --enable-languages=c \
        --without-headers && \
    make all-gcc all-target-libgcc -j$(nproc) && \
    make install-gcc install-target-libgcc

ENV PATH="/opt/cross/bin:${PATH}"

#
# Alias GDB
#
RUN ln -s /usr/bin/gdb /opt/cross/bin/i386-elf-gdb

WORKDIR /workspace

COPY . .

RUN make clean || true
RUN make

CMD ["qemu-system-i386", "-fda", "os.bin"]