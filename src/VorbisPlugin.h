#ifndef _VORBIS_PLUGIN_H_
#define _VORBIS_PLUGIN_H_

#include "ExportApi.h"
#include <stdio.h>
#include <stdlib.h>

EXPORT_API long WriteAllPcmDataToFile(
    const char* file_path,
    const float* samples,
    const long samples_length,
    const short channels,
    const long frequency,
    const float base_quality,
    const long samplesToRead);


EXPORT_API long ReadAllPcmDataFromFile(
    const char* file_path,
    float** samples,
    long* samples_length,
    short* channels,
    long* frequency,
    const long maxSamplesToRead);

EXPORT_API long FreeSamplesArrayNativeMemory(float** samples);


#endif // !_VORBIS_PLUGIN_H_