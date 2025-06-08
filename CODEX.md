# Project Overview

This repository contains a Unity plugin for loading and saving Vorbis `.ogg` audio
files at runtime. The plugin consists of native C code that links against the
open source *libvorbis* and *libogg* libraries. A C# wrapper in the Unity project
exposes the functionality in a convenient API.

## Repository Structure

- `src/` – C sources and headers that implement the plugin. The entry points are
  `VorbisPluginEncoder.c` and `VorbisPluginDecoder.c` along with the public
  header `VorbisPlugin.h`.
- `dependency/` – git submodules for external libraries.
  - `ogg/` – [xiph/ogg](https://github.com/xiph/ogg) (empty unless submodules are
    fetched).
  - `vorbis/` – [xiph/vorbis](https://github.com/xiph/vorbis).
  - `cmake-ios-toolchain/` – toolchain files for building on iOS.
- `projects/` – build projects.
  - `CMake/` – contains the main `CMakeLists.txt` used to compile the native
    libraries for different platforms.
  - `Android/` – Gradle project used to build the Android plugin binaries.
- `unity/` – Unity example project stored as a submodule.
- `.github/workflows/` – CI configuration which builds the plugin for Windows,
  Android, macOS, iOS and Linux using GitHub Actions.

## Building Native Libraries

The `projects/CMake/CMakeLists.txt` script orchestrates compilation of `libvorbis`,
`libogg` and the plugin sources into `VorbisPlugin` for each target platform. The
workflow builds static or shared libraries depending on the target (for iOS the
output is a static library, while other platforms use shared libraries).

## Usage

The Unity-side API (in the Unity submodule) lets you load an `.ogg` file into an
`AudioClip`, encode an `AudioClip` back to Vorbis, or work directly with byte
arrays. See the main `README.md` for sample code and feature descriptions.

