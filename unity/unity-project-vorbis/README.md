# Unity project Vorbis
This is a Unity sample project that uses the [C wrapper around Vorbis libraries](https://github.com/khindemit/unity-vorbis) to save and load audio data in Vorbis format.
You can checkout this repository. The latest released binaries are inside Assets/VorbisPlugin/Plugins/ folder

#### Supported platforms:
- Windows: x86, x86_64
- Android: arm64-v8a, armeabi-v7a, x86, x86_64
- iOS: aarch64
- Mac OSX: x86_64
#### Tested platforms:
- Windows: x86_64
- Android: arm64-v8a
- iOS: aarch64
- Mac OSX: x86_64
#### Tested with Unity versions:
- 2021.1.2f1
- 2019.1.12f1

## How to use the test project:
1. Checkout or download this repository
2. Open UnityClient project in Unity editor
3. Open Assets/_Presentation/Scene/Main.scene
4. Run the app
5. Press "Save Ogg Audio" to store the source Audio clip at runtime to local storage
6. Press "Load Ogg Audio" to load stored in local storage ogg audio file


## Build libraries for missing platforms:
In case you need some specific platform for the plugin, you can build the libraries yourself.
Detailed instructions for building the C wrapper native libraries you'll find [here](https://github.com/khindemit/unity-vorbis)
After you build the C libraries, you need to copy it to Assets/VorbisPlugin/Plugins folder:
#### Windows:
- Assets/VorbisPlugin/Plugins/Windows/x86/VorbisPlugin.dll
- Assets/VorbisPlugin/Plugins/Windows/x86_64/VorbisPlugin.dll
#### Android: 
- Assets/VorbisPlugin/Plugins/Android/libs/arm64-v8a/libVorbisPlugin.so 
- Assets/VorbisPlugin/Plugins/Android/libs/armeabi-v7a/libVorbisPlugin.so 
- Assets/VorbisPlugin/Plugins/Android/libs/x86/libVorbisPlugin.so 
- Assets/VorbisPlugin/Plugins/Android/libs/x86_64/libVorbisPlugin.so
#### Mac OSX:
- Assets/VorbisPlugin/Plugins/OSX/libVorbisPlugin.dylib
#### Linux:
- Assets/VorbisPlugin/Plugins/Linux/x86_64/libVorbisPlugin.so
#### iOS:
- Assets/VorbisPlugin/Plugins/iOS/libogg.a
- Assets/VorbisPlugin/Plugins/iOS/libvorbis.a
- Assets/VorbisPlugin/Plugins/iOS/libvorbisenc.a
- Assets/VorbisPlugin/Plugins/iOS/libvorbisfile.a
- Assets/VorbisPlugin/Plugins/iOS/libVorbisPlugin.a
#### WebGL (not tested):
- Assets/VorbisPlugin/Plugins/WebGL/libogg.a
- Assets/VorbisPlugin/Plugins/WebGL/libvorbis.a
- Assets/VorbisPlugin/Plugins/WebGL/libvorbisenc.a
- Assets/VorbisPlugin/Plugins/WebGL/libvorbisfile.a
- Assets/VorbisPlugin/Plugins/WebGL/libVorbisPlugin.a
