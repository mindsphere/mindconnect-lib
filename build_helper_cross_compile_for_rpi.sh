#!/bin/bash
openssl_version=1.1.1d
curl_version=7.65.3
main_directory=../mcl_sandbox_for_rpi
mcl_directory=$(pwd)

mkdir $main_directory
cd $main_directory

#Change relative path to absolute.
main_directory=$(pwd)

git clone --depth 1 https://github.com/raspberrypi/tools.git raspberrypi_tools
rpi_tool_path=$main_directory/raspberrypi_tools/arm-bcm2708/arm-linux-gnueabihf

wget -c https://www.openssl.org/source/openssl-$openssl_version.tar.gz
tar -xvzf openssl-$openssl_version.tar.gz

wget https://curl.haxx.se/download/curl-$curl_version.tar.gz
tar -xvzf curl-$curl_version.tar.gz

cd openssl-$openssl_version
./Configure --cross-compile-prefix=$rpi_tool_path/bin/arm-linux-gnueabihf- --prefix=$main_directory/install -Wl,-rpath=$main_directory/install/lib linux-generic32 shared -fPIC && make install_sw
cd ..

cd curl-$curl_version
export PATH=$PATH:$rpi_tool_path/bin
export CPPFLAGS="-I$rpi_tool_path/include"
export AR=arm-linux-gnueabihf-ar
export AS=arm-linux-gnueabihf-as
export LD=arm-linux-gnueabihf-ld
export RANLIB=arm-linux-gnueabihf-ranlib
export CC=arm-linux-gnueabihf-cc
export NM=arm-linux-gnueabihf-nm

LDFLAGS="-Wl,-R$main_directory/install/lib" ./configure --enable-http --with-ssl=$main_directory/install --prefix=$main_directory/install --without-libssh2 --disable-ftp --disable-tftp --disable-file --disable-ldap --disable-rtsp --disable-dict --disable-telnet --disable-pop3 --disable-imap --disable-smb --disable-scp --disable-sftp --disable-smtp --disable-gopher --disable-manual --host arm-linux-gnueabihf && make install
cd ..

echo "SET(CMAKE_SYSTEM_NAME Linux)" > toolchain_rpi.cmake
echo "SET(CMAKE_C_COMPILER $rpi_tool_path/bin/arm-linux-gnueabihf-gcc)" >> toolchain_rpi.cmake
echo "SET(CMAKE_CXX_COMPILER $rpi_tool_path/bin/arm-linux-gnueabihf-g++)" >> toolchain_rpi.cmake
echo "SET(CMAKE_FIND_ROOT_PATH $rpi_tool_path/arm-linux-gnueabihf/sysroot/)" >> toolchain_rpi.cmake
echo "SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)" >> toolchain_rpi.cmake
echo "SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)" >> toolchain_rpi.cmake
echo "SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)" >> toolchain_rpi.cmake

mkdir mcl_build
cd mcl_build
cmake -DCMAKE_PREFIX_PATH="$main_directory/install" -DCMAKE_TOOLCHAIN_FILE="$main_directory/toolchain_rpi.cmake" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX:PATH=$main_directory/install -DMCL_STATICLIB=OFF -DMCL_LOG_LEVEL=MCL_LOG_LEVEL_NONE $mcl_directory
cmake --build . --clean-first --target install
