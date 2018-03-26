# nac
neural-network acclerator

*under development*

## build PC
```
mkdir build
cd build
cmake ../
make -j$(nproc)
make install
```
will result in build/install


## cross-compile for arm
tested under linaro gcc-linaro-7.1.1
```
export PATH=/path/to/toolchain/:$PATH
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-linux-gnueabihf.toolchain.cmake   ../
make -j$(nproc)
make install
```
will result in build/install
