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
- Assets/Plugins/Windows/x86_64/VorbisPlugin.dll
#### Android: 
- Assets/Plugins/VorbisPlugin/Android/libs/arm64-v8a/libVorbisPlugin.so 
- Assets/Plugins/VorbisPlugin/Android/libs/armeabi-v7a/libVorbisPlugin.so 
- Assets/Plugins/VorbisPlugin/Android/libs/x86/libVorbisPlugin.so 
- Assets/Plugins/VorbisPlugin/Android/libs/x86_64/libVorbisPlugin.so
#### Mac OSX:
- Assets/Plugins/VorbisPlugin/OSX/libVorbisPlugin.dylib
#### iOS
- Assets/Plugins/VorbisPlugin/iOS/libogg.a
- Assets/Plugins/VorbisPlugin/iOS/libvorbis.a
- Assets/Plugins/VorbisPlugin/iOS/libvorbisfile.a
- Assets/Plugins/VorbisPlugin/iOS/libVorbisPlugin.a
