sudo apt install qemu
wget http://releases.linaro.org/components/toolchain/binaries/7.3-2018.05/arm-linux-gnueabi/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabi.tar.xz
tar xf gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabi.tar.xz
mv gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabi gcc-linaro
wget http://releases.linaro.org/components/toolchain/binaries/7.3-2018.05/arm-linux-gnueabi/sysroot-glibc-linaro-2.25-2018.05-arm-linux-gnueabi.tar.xz
tar xf sysroot-glibc-linaro-2.25-2018.05-arm-linux-gnueabi.tar.xz
mv sysroot-glibc-linaro-2.25-2018.05-arm-linux-gnueabi sysroot-glibc-linaro
SYSROOT=${PWD}/"/sysroot-glibc-linaro"
CC_PATH=${PWD}/"/gcc-linaro/bin/arm-linux-gnueabi-gcc"
CXX_PATH=${PWD}/"/gcc-linaro/bin/arm-linux-gnueabi-g++"
mkdir build
cd ./build
CC=${CC_PATH} CXX=${CXX_PATH} cmake ../
make
qemu-arm -L $SYSROOT run_tests

