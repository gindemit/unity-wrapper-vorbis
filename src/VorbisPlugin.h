#ifndef _VORBIS_PLUGIN_H_
#define _VORBIS_PLUGIN_H_

#include "ExportApi.h"
#include <stdio.h>
#include <stdlib.h>
#include <vorbis/vorbisfile.h>


typedef struct VorbisFileReadStreamState {
    FILE* file_stream;
    OggVorbis_File vf;
    int eof;
    int current_section;
    vorbis_info* vi;
} VorbisFileReadStreamState;


EXPORT_API int32_t WriteAllPcmDataToFile(
    const char* file_path,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samplesToRead);

    
EXPORT_API int32_t WriteAllPcmDataToMemory(
    char** memory_array,
    int32_t* memory_array_length,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samplesToRead);


EXPORT_API int32_t ReadAllPcmDataFromFile(
    const char* file_path,
    float** samples,
    int32_t* samples_length,
    int16_t* channels,
    int32_t* frequency,
    const int32_t maxSamplesToRead);
EXPORT_API int32_t FreeSamplesArrayNativeMemory(float** samples);

EXPORT_API VorbisFileReadStreamState* OpenReadFileStream(const char* file_path, int16_t* channels, int32_t* frequency);
EXPORT_API int32_t ReadFromFileStream(VorbisFileReadStreamState* state, float* samples_to_fill, const int32_t max_samples_to_read);
EXPORT_API int32_t CloseFileStream(VorbisFileReadStreamState* state);

#endif // !_VORBIS_PLUGIN_H_