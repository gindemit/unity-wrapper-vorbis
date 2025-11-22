using NUnit.Framework;
using System.IO;
using UnityEngine;

namespace PlayModeTests
{
    public class SourceFilesPropertiesTests
    {
        private AudioClip _stereoFile3Minutes48000Hz;
        private AudioClip _monoFile3Minutes48000Hz;
        private AudioClip _stereoFile2Minutes44100Hz;
        private AudioClip _monoFile2Minutes44100Hz;

        private string _filesFolder;

        [SetUp]
        public void SetUp()
        {
            _stereoFile3Minutes48000Hz = Resources.Load<AudioClip>(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_48000HZ);
            _monoFile3Minutes48000Hz = Resources.Load<AudioClip>(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_MONO_48000HZ);
            _stereoFile2Minutes44100Hz = Resources.Load<AudioClip>(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_44100HZ);
            _monoFile2Minutes44100Hz = Resources.Load<AudioClip>(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_MONO_44100HZ);

            _filesFolder = Path.Combine(Application.persistentDataPath, Consts.TEST_FILE_DIR_NAME);
            Directory.CreateDirectory(_filesFolder);
        }

        [Test]
        public void TheAudioClipsAreLoaded()
        {
            Assert.IsNotNull(_stereoFile3Minutes48000Hz);
            Assert.IsNotNull(_monoFile3Minutes48000Hz);
            Assert.IsNotNull(_stereoFile2Minutes44100Hz);
            Assert.IsNotNull(_monoFile2Minutes44100Hz);
        }
        [Test]
        public void TheAudioClipsHaveTheRightChannelCount()
        {
            Assert.AreEqual(2, _stereoFile3Minutes48000Hz.channels);
            Assert.AreEqual(1, _monoFile3Minutes48000Hz.channels);
            Assert.AreEqual(2, _stereoFile2Minutes44100Hz.channels);
            Assert.AreEqual(1, _monoFile2Minutes44100Hz.channels);
        }
        [Test]
        public void TheAudioClipsHaveTheRightFrequencySampleRate()
        {
            Assert.AreEqual(48000, _stereoFile3Minutes48000Hz.frequency);
            Assert.AreEqual(48000, _monoFile3Minutes48000Hz.frequency);
            Assert.AreEqual(44100, _stereoFile2Minutes44100Hz.frequency);
            Assert.AreEqual(44100, _monoFile2Minutes44100Hz.frequency);
        }
        [Test]
        public void TheVorbisPluginSavesStereo48000HzFile()
        {
            string pathToFile = Path.Combine(_filesFolder, _stereoFile3Minutes48000Hz.name) + ".ogg";
            OggVorbis.VorbisPlugin.Save(pathToFile, _stereoFile3Minutes48000Hz);
            Assert.IsTrue(File.Exists(pathToFile));
            var fileInfo = new FileInfo(pathToFile);
            Assert.IsTrue(fileInfo.Length > 1000);
        }
    }
}