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

static void test_encode_to_file_decode_from_file() {
    WriteAllPcmDataToFile(OGG_TEST_FILE_NAME, test_data, test_data_length, 1, 44100, 0.2, 1024);
    float* samples;
    long samples_filled_length;
    short channels;
    long frequency;
    ReadAllPcmDataFromFile(OGG_TEST_FILE_NAME, &samples, &samples_filled_length, &channels, &frequency, 1024);

    assert(44101 == samples_filled_length);
    assert(1 == channels);
    assert(44100 == frequency);
    for (int i = 0; i < samples_filled_length; ++i) {
        int result = nearlyEqual(test_data[i], samples[i], 0.05);
        assert(result);
    }
    printf("Encode Decode Success\n");
}
static void test_read_from_file_stream() {
    short channels;
    long frequency;
    VorbisFileReadStreamState* state = OpenReadFileStream(OGG_TEST_FILE_NAME, &channels, &frequency);
    assert(1 == channels);
    assert(44100 == frequency);

    long checked_samples = 0;
    long MAX_SAMPLES_TO_LOAD = 1024;
    float* samples = (float*)malloc(sizeof(float) * MAX_SAMPLES_TO_LOAD);
    if (samples == NULL) {
        assert(0);
    }
    while (!state->eof) {
        int32_t read_samples = ReadFromFileStream(state, samples, MAX_SAMPLES_TO_LOAD);
        assert(read_samples <= MAX_SAMPLES_TO_LOAD);
        for (int32_t i = 0; i < read_samples; ++i) {
            float original = test_data[checked_samples++];
            float sample = samples[i];
            int result = nearlyEqual(original, sample, 0.05);
            assert(result);
        }
    }
    free(samples);
    CloseFileStream(state);
    printf("Read From FileStream Success\n");
}
static void test_encode_to_memory()
{
    char* memory_buffer = NULL;
    int32_t memory_buffer_length = 0;
    WriteAllPcmDataToMemory(&memory_buffer, &memory_buffer_length, test_data, test_data_length, 1, 44100, 0.2, 1024);
    assert(memory_buffer != NULL);
    assert(memory_buffer_length > 0);
    FILE* file_stream = fopen(OGG_TEST_FILE_NAME, "rb");
    if (file_stream == NULL) {
        assert(0);
    }
    size_t compared_bytes_count = 0;
    char buffer[1024];
    while (!feof(file_stream)) {
        size_t actually_read = fread(buffer, sizeof(char), 1024, file_stream);
        for (size_t i = 0; i < actually_read; ++i) {
            assert(memory_buffer[compared_bytes_count++] == buffer[i]);
        }
    }
    fclose(file_stream);
    free(memory_buffer);
    printf("Write to memory buffer success\n");
}

int main() {
    test_encode_to_file_decode_from_file();
    test_read_from_file_stream();
    test_encode_to_memory();
}