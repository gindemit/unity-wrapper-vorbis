#include "VorbisPlugin.h"
#include "TestSamplesArray.h"
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

const char* OGG_TEST_FILE_NAME = "1_ogg_plugin_test.ogg";

static int nearlyEqual(float a, float b, float epsilon)
{
    float diff = fabsf(a - b);
    return diff < epsilon;
}

static void TestEncodeToFileDecodeFromFile() {
    WriteAllPcmDataToFile(OGG_TEST_FILE_NAME, testData, testDataLength, 1, 44100, 0.2, 1024);
    float* samples;
    long samples_filled_length;
    short channels;
    long frequency;
    ReadAllPcmDataFromFile(OGG_TEST_FILE_NAME, &samples, &samples_filled_length, &channels, &frequency, 1024);

    assert(44101 == samples_filled_length);
    assert(1 == channels);
    assert(44100 == frequency);
    for (int i = 0; i < samples_filled_length; ++i) {
        int result = nearlyEqual(testData[i], samples[i], 0.05);
        assert(result);
    }
    printf("Encode Decode Success\n");
}
static void TestReadFromFileStream() {
    short channels;
    long frequency;
    VorbisFileReadStreamState* state = OpenReadFileStream(OGG_TEST_FILE_NAME, &channels, &frequency);
    assert(1 == channels);
    assert(44100 == frequency);

    long checked_samples = 0;
    long maxSamplesToLoad = 1024;
    float* samples = (float*)malloc(sizeof(float) * maxSamplesToLoad);
    if (samples == NULL) {
        assert(0);
    }
    while (!state->eof) {
        int32_t read_samples = ReadFromFileStream(state, samples, maxSamplesToLoad);
        assert(read_samples <= maxSamplesToLoad);
        for (int32_t i = 0; i < read_samples; ++i) {
            float original = testData[checked_samples++];
            float sample = samples[i];
            int result = nearlyEqual(original, sample, 0.05);
            assert(result);
        }
    }
    free(samples);
    CloseFileStream(state);
    printf("Read From FileStream Success\n");
}

int main() {
    TestEncodeToFileDecodeFromFile();
    TestReadFromFileStream();
}