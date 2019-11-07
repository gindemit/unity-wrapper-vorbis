# C++ source code for Unity Vorbis plugin

This repository contain only C/C++ code. The Unity project is added as git submodule.

#### Following functions are supported:
- Write all PCM data to ogg file at path
- Read all ogg data from file to float[] samples array
- Stream read ogg from file

#### Build process

**Windows**: 

The easiest way to build the plugin on Windows is to open it in VisualStudio (make sure the CMake addon is installed) and run build all.
The CMakeLists.txt file is in unity-vorbis/projects/CMake/ directory.