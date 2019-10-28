#include "VorbisPlugin.h"
#include "TestSamplesArray.h"
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <math.h>


static int nearlyEqual(float a, float b, float epsilon)
{
    float diff = fabsf(a - b);
    return diff < epsilon;
}

static void TestEncodeToFileDecodeFromFile() {
    EncodePcmDataToFile("1_plugin_test_out_text.ogg", testData, testDataLength, 1, 44100, 0.2, 1024);
    float* samples;
    long samples_filled_length;
    short channels;
    long frequency;
    DecodePcmDataFromFile("1_plugin_test_out_text.ogg", &samples, &samples_filled_length, &channels, &frequency, 1024);

    assert(44101 == samples_filled_length);
    assert(1 == channels);
    assert(44100 == frequency);
    for (int i = 0; i < samples_filled_length; ++i) {
        int result = nearlyEqual(testData[i], samples[i], 0.05);
        assert(result);
    }
    printf("Encode Decode success");
}

int main() {
    TestEncodeToFileDecodeFromFile();

}