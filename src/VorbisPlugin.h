#ifndef _VORBIS_PLUGIN_H_
#define _VORBIS_PLUGIN_H_

#include "ExportApi.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vorbis/vorbisfile.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vorbis_file_read_stream_state {
    FILE* file_stream;
    OggVorbis_File vf;
    int eof;
    int current_section;
    vorbis_info* vi;
} vorbis_file_read_stream_state;

EXPORT_API int32_t write_all_pcm_data_to_file(
    const char* file_path,
    const float* samples,
    int32_t samples_length,
    int16_t channels,
    int32_t frequency,
    float base_quality,
    int32_t samples_to_read);
EXPORT_API int32_t write_all_pcm_data_to_memory(
    char** memory_array,
    int32_t* memory_array_length,
    const float* samples,
    int32_t samples_length,
    int16_t channels,
    int32_t frequency,
    float base_quality,
    int32_t samples_to_read);
EXPORT_API int32_t free_memory_array_for_write_all_pcm_data(char* memory_array);

EXPORT_API int32_t read_all_pcm_data_from_file(
    const char* file_path,
    float** samples,
    int32_t* samples_length,
    int16_t* channels,
    int32_t* frequency,
    int32_t max_samples_to_read);
EXPORT_API int32_t read_all_pcm_data_from_memory(
    const char* memory_array,
    int32_t memory_array_length,
    float** samples,
    int32_t* samples_length,
    int16_t* channels,
    int32_t* frequency,
    int32_t max_samples_to_read);
EXPORT_API int32_t free_samples_array_native_memory(float** samples);

EXPORT_API int32_t open_read_file_stream(
    vorbis_file_read_stream_state** state,
    const char* file_path,
    int16_t* channels,
    int32_t* frequency);
EXPORT_API int32_t read_from_file_stream(
    vorbis_file_read_stream_state* state,
    float* samples_to_fill,
    int32_t max_samples_to_read);
EXPORT_API int32_t close_file_stream(vorbis_file_read_stream_state* state);

#ifdef __cplusplus
}
#endif

#endif // !_VORBIS_PLUGIN_H_
