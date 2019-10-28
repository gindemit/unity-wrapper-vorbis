#ifndef _VORBIS_PLUGIN_H_
#define _VORBIS_PLUGIN_H_

#include "ExportApi.h"
#include <stdio.h>

long EXPORT_API WriteAllPcmDataToFile(
    const char* file_path,
    const float* samples,
    const long samples_length,
    const short channels,
    const long frequency,
    const float base_quality,
    const long samplesToRead);

long EXPORT_API ReadAllPcmDataFromFile(
    const char* file_path,
    float** samples,
    long* samples_length,
    short* channels,
    long* frequency,
    const long maxSamplesToRead);

long EXPORT_API FreeSamplesArrayNativeMemory(float** samples);

#endif // !_VORBIS_PLUGIN_H_