# C wrapper source code for Unity Vorbis plugin

This repository contain only C code. [The Unity project](https://github.com/khindemit/unity-project-vorbis) that use this wrapper is added as git submodule.
The ogg and vorbis projects are added also as submodules. When you checkout this repository you need to initialize and update submudules.

# Following functions are supported:
### 1. Load vorbis ogg from file at runtime:
```
string pathToFile = ...;
UnityEngine.AudioClip audioClip = OggVorbis.VorbisPlugin.Load(pathToFile);
```
### 2. Save vorbis ogg to file at runtime:
```
string pathToFile = ...;
UnityEngine.AudioClip audioClip = ...;
float quality = 0.4f; // optional parameter, values from 0 to 1 (https://xiph.org/vorbis/doc/vorbisenc/overview.html)
OggVorbis.VorbisPlugin.Save(pathToFile, audioClip, quality);
```
### 3. Get vorbis ogg from bytes array at runtime:
```
byte[] sourceAudioBytes = ...;
UnityEngine.AudioClip sourceAudioClip = OggVorbis.VorbisPlugin.ToAudioClip(sourceAudioBytes, "NameOfAudioClip");
```
### 4. Serialize vorbis ogg to bytes array at runtime:
```
UnityEngine.AudioClip sourceAudioClip = ...;
float quality = 0.4f; // optional parameter, values from 0 to 1 (https://xiph.org/vorbis/doc/vorbisenc/overview.html)
byte[] bytes = OggVorbis.VorbisPlugin.GetOggVorbis(sourceAudioClip, quality);
```
### 5. There is an C# API to read from file as from stream, but it is not well tested

# Installation:
1. Download from [latest releases](https://github.com/khindemit/unity-wrapper-vorbis/releases) the VorbisPlugin_vX.X.X.unitypackage file
2. In your project in Unity editor click on menu item Assets->Import Package->Custom Package...
3. If you use [assembly definition files](https://docs.unity3d.com/Manual/ScriptCompilationAssemblyDefinitionFiles.html) add the "VorbisPluginImpl" as a reference to your project

# Notes:
- The **.unitypackage** contains native libraries **build with Release** configuration. For debugging download the *.zip* file with libraries and replace the libraries with debug version for platforms you want to debug.
- The functions to load/save from/to file and bytes array was tested with Unity runtime tests on Win 10 PC, Mac Book Pro, iPhone 6 and Google Pixel 3a. All IL2CPP builds. If you want to test it yourself, please review readme for [Unity project](https://github.com/khindemit/unity-project-vorbis).
- If you experience problems, have suggestions to improve the API, or found missing functionality, please submit an issue.


## Build process (optional, the releases contain the native libraries):

**Build on Windows for Windows operating system (x64-Debug, x64-Release, x86-Debug, x86-Release)**: 

The easiest way to build the plugin on Windows is to open it in VisualStudio (make sure the CMake addon is installed) and run build all.
The CMakeLists.txt file is in "unity-vorbis/projects/CMake/" directory.
Depending on the selected in VisualStudio configuration the VorbisPlugin.dll will be in "unity-vorbis/projects/CMake/out/build/[selected_configuration]"
For example if the selected configuration is x64-Debug then the VorbisPlugin.dll are stored in "unity-vorbis/projects/CMake/out/build/x64-Debug" folder

**Build on Windows for Android operating system**:

Open the project in AndroidStudio ("unity-vorbis/projects/Android/" folder)
Make sure that the Adroid SDK and Android NDK are installed.
If you want to build for multiple ABIs, you need to add it to the build.gradle file:
```abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86'```

Run Build->Make Project menu item.
1. If you get a fatal error: 'ogg/ogg.h' file not found, you can workaround it by copying the ogg.h and os_types.h files from 
unity-vorbis\dependency\ogg\include\ogg\
to
unity-vorbis\projects\Android\app\.cxx\cmake\debug\[TARGET_ABI]\ogg_build\include\ogg\
2. If you receive linker error that vorbis_encode_init_vbr could not be resolved, then open the
"unity-vorbis\dependency\vorbis\lib\CMakeLists.txt" file and comment out the if(WIN32) and endif() lines, it sould look like this:
```
#if(WIN32)
    list(APPEND VORBIS_SOURCES vorbisenc.c)
#endif()
```
I have no idea why vorbis plugin developers include the vorbisenc.c file only for Windows.
The code above will include the vorbisenc.c file to the Vorbis library.

By default you'll get the debug versions of the libraries. To build the release versions you need to run the gradle task "compileReleaseSources". To do so you need to
click gradle menu on top right side of Android Studio and go to "Android->app->Tasks->build->compileReleaseSources"

The output libraries you'll find in following folders:
```unity-vorbis\projects\Android\app\build\intermediates\cmake\[debug OR release]\obj\[TARGET ABI]\```

**Build on MacOS for Mac OSX operating system**:

Install CMake. Open the "unity-vorbis/projects/CMake" folder and generate xCode project.
From terminal:
if cmake is not installed (to check type "cmake" in terminal) then install it: 
```
brew install cmake
```
Then go to "unity-vorbis/projects/CMake" directory and run:
```
cmake -S . -B ./buildOSX -G Xcode -DVORBIS_OSX=1
```
To build the project from terminal you can run:
```
cmake --build ./buildOSX --config Release
```
After successful build you can find the static libraries for iOS in VorbisPluginBuild folder.


**Build on MacOS for iOS operating system**:

To build for iOS you need to initialize the iOS Toolchain submodule under dependency folder.
Install CMake. Open the "unity-vorbis/projects/CMake" folder and generate xCode project.
From terminal:
if cmake is not installed (to check type "cmake" in terminal) then install it: 
```
brew install cmake
```
Then go to "unity-vorbis/projects/CMake" directory and run:
```
cmake -S . -B ./buildIOS -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../dependency/cmake-ios-toolchain/ios.toolchain.cmake -DPLATFORM=OS64 -DVORBIS_IOS=1
```
To build the project from terminal you can run:
```
cmake --build ./buildIOS --config Release --target VorbisPlugin
```
Or you can open the xCode project located in buildIOS directory. In xCode select VorbisPlugin->Any iOS Device (arm64). Build it. After successful build you can find the static libraries for iOS in VorbisPluginBuild folder. You can open the directory in finder and copy the libraries to your Unity project.
Notice that the libraries are build with arm64 architecture only (iPhone 6+). If you need to build also for armv7, you need to select in xCode standart libraries under Build Settings Architectures for VorbisPlugin build target.
By default the build will create Debug libraries. To build Release ones you need in xCode modify the current scheme: Product->Scheme-Edit Scheme... and change the build configuration.

## Project structure:

- **unity-vorbis**
    - **dependency** this folder contain the git submodules for *ogg* and *vorbis* repositories
        - **ogg** ogg repository
        - **vorbis** vorbis repository
        - **cmake-ios-toolchain** repository with the CMake iOS toolchain file
    - **projects** this folder contain the Android and CMake project
    - **src** this folder contain the source code of the VorbisPlugin and also PluginTest.c file (simple program to test the Save/Load function)
    - **unity** this folder contain the Unity part of the VorbisPlugin (sample project and source C# files)
