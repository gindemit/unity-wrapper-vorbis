# Unity Vorbis Plugin
![GitHub release](https://img.shields.io/github/release/gindemit/unity-vorbis.svg)
![Unity version](https://img.shields.io/badge/unity-2019.4%2B-green.svg)
![License](https://img.shields.io/github/license/gindemit/unity-vorbis.svg)

Unity Vorbis plugin is a powerful tool for the Unity game engine that allows you to work with OGG Vorbis audio files. OGG Vorbis is a free, open-source audio coding format that is well-suited for streaming and manipulation in games.

## Features
The plugin provides you with the following functionalities:
1. **Load Vorbis OGG from file at runtime**: Allows you to load OGG files from specified file paths.
2. **Save Vorbis OGG to file at runtime**: Allows you to save AudioClip objects as OGG files at specified paths.
3. **Get Vorbis OGG from bytes array at runtime**: Allows you to convert byte arrays to AudioClip objects.
4. **Serialize Vorbis OGG to bytes array at runtime**: Allows you to convert AudioClip objects to byte arrays.
5. **C# API to read from file as from stream**: This is an experimental feature and is not fully tested.

For detailed information about these features, visit the [API Reference](https://github.com/gindemit/unity-vorbis/wiki/API-Reference) page on our wiki.

## Supported Platforms
* Android
* iOS
* Windows
* OSX

## Installation

### Unity Package Manager

To install the Unity Vorbis Plugin using the Unity Package Manager, follow these steps:

1. Open your Unity project and navigate to `Window` > `Package Manager`.
2. Click the `+` button in the top-left corner and select `Add package from git URL...`.
3. Enter the following URL and click `Add`:
   ```
   https://github.com/gindemit/unity-vorbis.git?path=/unity/unity-project-vorbis/UnityClient/Assets/VorbisPlugin/#0.3.1
   ```

For more detailed instructions, please visit the [Installation Guide](https://github.com/gindemit/unity-vorbis/wiki/Installation-Guide) on the wiki.

## Quick Start

Here's a quick rundown on how to use some of the most essential functions provided by this plugin:
```csharp
// Loading Vorbis OGG from file at runtime:
string pathToFile = ...; // path to your file
UnityEngine.AudioClip audioClip = OggVorbis.VorbisPlugin.Load(pathToFile);

// Saving Vorbis OGG to file at runtime:
string pathToFile = ...; // path where you want to save the file
UnityEngine.AudioClip audioClip = ...; // audio clip you want to save
float quality = 0.4f; // optional parameter, values from 0 to 1
OggVorbis.VorbisPlugin.Save(pathToFile, audioClip, quality);

// Getting Vorbis OGG from bytes array at runtime:
byte[] sourceAudioBytes = ...; // byte array of your audio
UnityEngine.AudioClip sourceAudioClip = OggVorbis.VorbisPlugin.ToAudioClip(sourceAudioBytes, "NameOfAudioClip");

// Serializing Vorbis OGG to bytes array at runtime:
UnityEngine.AudioClip sourceAudioClip = ...; // audio clip you want to serialize
float quality = 0.4f; // optional parameter, values from 0 to 1
byte[] bytes = OggVorbis.VorbisPlugin.GetOggVorbis(sourceAudioClip, quality);
```

## Usage
Visit the [Usage Guide](https://github.com/gindemit/unity-vorbis/wiki/Usage-Guide) on the wiki for step-by-step instructions on how to use the plugin in your Unity project.

## Building from Source
If you want to build the plugin from source, we've provided detailed instructions for each supported platform in our [Building from Source](https://github.com/gindemit/unity-vorbis/wiki/Building-from-Source) page on the wiki.

## Contributing
We welcome contributions from the community. To contribute, please read the [Contribution Guidelines](https://github.com/gindemit/unity-vorbis/wiki/Contributions) on the wiki.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
