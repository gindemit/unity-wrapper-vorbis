using NUnit.Framework;
using System.IO;
using Unity.PerformanceTesting;
using UnityEngine;

namespace PlayModeTests
{
    public sealed class PerformanceTests
    {
        [Test, Performance]
        public void TestSaveToFilePerformance()
        {
            AudioClip sourceAudioClip = Resources.Load<AudioClip>(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_48000HZ);
            string filesFolder = Path.Combine(Application.persistentDataPath, Consts.TEST_FILE_DIR_NAME);
            string pathToFile = Path.Combine(filesFolder, sourceAudioClip.name) + ".ogg";


            Measure.Method(() => { OggVorbis.VorbisPlugin.Save(pathToFile, sourceAudioClip); })
                .WarmupCount(5)
                .MeasurementCount(10)
                .IterationsPerMeasurement(5)
                .GC()
                .Run();
        }
        [Test, Performance]
        public void TestLoadFromFilePerformance()
        {
            string filesFolder = Path.Combine(Application.persistentDataPath, Consts.TEST_FILE_DIR_NAME);
            string pathToFile = Path.Combine(filesFolder, Path.GetFileName(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_48000HZ) + ".ogg");

            Measure.Method(() => { OggVorbis.VorbisPlugin.Load(pathToFile); })
                .WarmupCount(5)
                .MeasurementCount(10)
                .IterationsPerMeasurement(5)
                .GC()
                .Run();
        }
        [Test, Performance]
        public void TestLoadFromBytesArrayPerformance()
        {
            byte[] sourceAudioBytes = Support.StreamingAssets.StreamingAssetsHelper.LoadFileFromStreamingAssets(
                Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_48000HZ + ".ogg");
            Measure.Method(() => { OggVorbis.VorbisPlugin.ToAudioClip(sourceAudioBytes, "AudioClip"); })
                .WarmupCount(5)
                .MeasurementCount(10)
                .IterationsPerMeasurement(5)
                .GC()
                .Run();
        }
        [Test, Performance]
        public void TestSerializeToBytesArrayPerformance()
        {
            AudioClip sourceAudioClip = Resources.Load<AudioClip>(Consts.SOURCE_AUDIO_CLIP_RESOURCES_PATH_STEREO_48000HZ);
            Measure.Method(() => { OggVorbis.VorbisPlugin.GetOggVorbis(sourceAudioClip, 0.4f); })
                .WarmupCount(5)
                .MeasurementCount(10)
                .IterationsPerMeasurement(5)
                .GC()
                .Run();
        }

    }
}
