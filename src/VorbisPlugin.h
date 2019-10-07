#ifndef _VORBIS_PLUGIN_H_
#define _VORBIS_PLUGIN_H_

#include "ExportApi.h"

long EXPORT_API EncodePcmDataToFile(
    const char* filePath,
    const float* samples,
    const long samplesLength,
    const short channels,
    const long frequency,
    const float base_quality);

long EXPORT_API DecodePcmDataFromFile(
    const char* filePath,
    float** samples,
    long* samplesLength,
    short* channels,
    long* frequency);

long EXPORT_API FreeSamplesArrayNativeMemory(float** samples);

#endif // !_VORBIS_PLUGIN_H_