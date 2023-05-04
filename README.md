# RGB H265 Decoding for OAKD

CMake example project which serves as a template on how to quickly get started with C++ and depthai library

## Depthai library dependencies
- cmake >= 3.4
- C/C++14 compiler
## Submodules
Make sure submodules are initialized and updated 
```
git submodule update --init --recursive
```

## Prerequisites 

Gstreamer
Depthai


## Building

Configure and build
```
mkdir -p build && cd build
cmake ..
cmake --build . --parallel
```

## Running

To run the application 'rgb_decoder', navigate to build directory and run 'rgb_decoder' executable
```
./rgb decoder
[Flags] 
-v : verbose
-d : debug
```
