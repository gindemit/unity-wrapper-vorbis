# C wrapper source code for Unity Vorbis plugin

This repository contain only C code. [The Unity project](https://github.com/khindemit/unity-project-vorbis) that use this wrapper is added as git submodule.

## Following functions are supported:
- Write all PCM data to ogg file at path
- Read all ogg data from file to float[] samples array
- Stream read ogg from file

## Build process:

**Build on Windows for Windows operating system (x64-Debug, x64-Release, x86-Debug, x86-Release)**: 

The easiest way to build the plugin on Windows is to open it in VisualStudio (make sure the CMake addon is installed) and run build all.
The CMakeLists.txt file is in "unity-vorbis/projects/CMake/" directory.
Depending on the selected in VisualStudio configuration the VorbisPlugin.dll will be in "unity-vorbis/projects/CMake/out/build/[selected_configuration]"
For example if the selected configuration is x64-Debug then the VorbisPlugin.dll are stored in "unity-vorbis/projects/CMake/out/build/x64-Debug" folder

**Build on Windows for Android operating system**:

Open the project in AndroidStudio ("unity-vorbis/projects/Android/" folder)
Make sure that the Adroid SDK and Android NDK are installed.
Run Build->Make Project menu item.
If you receive linker error that vorbis_encode_init_vbr could not be resolved, then open the
"unity-vorbis\dependency\vorbis\lib\CMakeLists.txt" file and comment out the if(WIN32) and endif() lines, it sould look like this:
```
#if(WIN32)
    list(APPEND VORBIS_SOURCES vorbisenc.c)
#endif()
```
I have no idea why vorbis plugin developers include the vorbisenc.c file only for Windows.
The code above will include the vorbisenc.c file to the Vorbis library.

**Build on MacOS for iOS operating system**:

Install CMake (better GUI version). Open the "unity-vorbis/projects/CMake" folder and generate xCode project.

## Project structure:

- **unity-vorbis**
    - **dependency** this folder contain the git submodules for *ogg* and *vorbis* repositories
        - **ogg** ogg repository
        - **vorbis** vorbis repository
    - **projects** this folder contain the Android and CMake project
    - **src** this folder contain the source code of the VorbisPlugin and also PluginTest.c file (simple program to test the Save/Load function)
    - **unity** this folder contain the Unity part of the VorbisPlugin (sample project and source C# files)
