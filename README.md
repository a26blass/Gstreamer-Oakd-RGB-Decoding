# RGB H264 Decoding for OAKD

Demo on decoding H264 RGB data from OAK-D

## Depthai library dependencies
- cmake >= 3.4
- C/C++14 compiler
## Submodules
Make sure submodules are initialized and updated 
```
git submodule update --init --recursive
```

## Prerequisites 
```
Ubuntu 20.04

Gstreamer - apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio

Depthai - sudo wget -qO- https://docs.luxonis.com/install_dependencies.sh | bash
```

## Building

Configure and build
```
cd build
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
-r [RESOLUTION] : specify resolution. Available resolutions: 
THE_1080_P

THE_1200_P

THE_12_MP

THE_13_MP

THE_1440X1080

THE_4000X3000

THE_48_MP

THE_4_K

THE_5312X6000

THE_5_MP

THE_720_P

THE_800_P
```
