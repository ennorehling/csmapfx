# Building CSMapFX


## Linux

apt pkgconf cmake make g++ install libpng-dev libboost-dev libfox-1.6-dev

## Windows
Currently only the Win32 Release build works, only via project file, not with CMake.

1. Using vcpkg, install the following libraries:
    * boost-core, boost-function, boost-tuple 
    * libpng
2. Download the Fox Toolkit version 1.6 and install it in C:\usrlocal\fox-1.6.57
    The Fox Toolkit has no good packages for Windows, and the CMakeLists.txt rules
    to find it are a terrible hack.
