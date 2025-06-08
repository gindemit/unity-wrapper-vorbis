using NUnit.Framework;
using System.IO;
using UnityEngine;

namespace PlayModeTests
{
    public class OggFileWriteReadTest
    {
        private string _filesFolder;

        [SetUp]
        public void SetUp()
        {
            _filesFolder = Path.Combine(Application.persistentDataPath, Consts.TEST_FILE_DIR_NAME);
            Directory.CreateDirectory(_filesFolder);
        }

        [TestCase(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_48000HZ)]
        [TestCase(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_MONO_48000HZ)]
        [TestCase(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_44100HZ)]
        [TestCase(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_MONO_44100HZ)]
        public void SaveLoadAndSaveWork(string sourceAudioClipPath)
        {
            AudioClip sourceAudioClip = Resources.Load<AudioClip>(sourceAudioClipPath);
            string pathToFile = Path.Combine(_filesFolder, sourceAudioClip.name) + ".ogg";
            OggVorbis.VorbisPlugin.Save(pathToFile, sourceAudioClip);
            AudioClip audioClip = OggVorbis.VorbisPlugin.Load(pathToFile);
            OggVorbis.VorbisPlugin.Save(Path.Combine(_filesFolder, sourceAudioClip.name) + "_1.ogg", audioClip);

            Assert.AreEqual(sourceAudioClip.channels, audioClip.channels);
            Assert.AreEqual(sourceAudioClip.frequency, audioClip.frequency);

            // Tested on Win 10 Unity 2021.1.2f1 version:
            // 384 delta is added because Unity SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_44100HZ.samples returns
            // an array that is 384 samples smaller in size that the original file contains
            // The sourceAudioClip source file in Unity project is an mp3 file
            // The Audacity returns:
            //      6 325 632 samples for the original mp3 file from Unity Assets folder
            //      6 326 016 samples for the first stored ogg file
            //      6 326 272 samples for the ogg file that is stored from the loaded first ogg file
            Assert.AreEqual(sourceAudioClip.samples, audioClip.samples, 384);
            // And the length of the files differs little bit
            Assert.AreEqual(sourceAudioClip.length, audioClip.length, 0.1);

            float[] samplesFromResourcesFile = new float[sourceAudioClip.samples];
            Assert.IsTrue(sourceAudioClip.GetData(samplesFromResourcesFile, 0));
            float[] samplesFromLoadedOggFile = new float[audioClip.samples];
            Assert.IsTrue(audioClip.GetData(samplesFromLoadedOggFile, 0));

            int length = Mathf.Min(sourceAudioClip.samples, audioClip.samples);
            for (int i = 0; i < length; ++i)
            {
                Assert.AreEqual(samplesFromResourcesFile[i], samplesFromLoadedOggFile[i], 0.2, "Wrong sample value at " + i);
            }
        }
    }
}