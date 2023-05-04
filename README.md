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

### OpenCV - optional 

OpenCV is required to run the out-of-box example.
If OpenVINO is installed then no further action is required.

Otherwise install using one of the options below:

- Ubuntu 20.04 - `sudo apt install libopencv-dev`
- Fedora36+ - `sudo dnf install opencv-devel`
 - MacOS - `brew install opencv`
 - Or install from package / sources (Windows and other OSes)
https://docs.opencv.org/master/d0/d3d/tutorial_general_install.html


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
