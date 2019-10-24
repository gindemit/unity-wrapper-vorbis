#ifndef _VORBIS_PLUGIN_H_
#define _VORBIS_PLUGIN_H_

#include "ExportApi.h"
#include <stdio.h>

long EXPORT_API EncodePcmDataToFile(
    const char* file_path,
    const float* samples,
    const long samples_length,
    const short channels,
    const long frequency,
    const float base_quality);

long EXPORT_API EncodePcmDataToFileStream(
    FILE* file_stream,
    const float* samples,
    const long samples_length,
    const short channels,
    const long frequency,
    const float base_quality);

long EXPORT_API DecodePcmDataFromFile(
    const char* file_path,
    float** samples,
    long* samples_length,
    short* channels,
    long* frequency);

long EXPORT_API FreeSamplesArrayNativeMemory(float** samples);

#endif // !_VORBIS_PLUGIN_H_