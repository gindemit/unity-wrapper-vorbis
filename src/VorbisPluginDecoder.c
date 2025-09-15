#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>

#include "VorbisPlugin.h"
#include "FloatArray.h"
#include "ErrorCodes.h"
#include "OggFileCallbacks.h"


int32_t read_all_pcm_data_from_file(
    const char* file_path,
    float** samples_to_fill,
    int32_t* samples_filled_length,
    int16_t* channels,
    int32_t* frequency,
    const int32_t max_samples_to_read) {

    vorbis_file_read_stream_state* state = NULL;
    open_read_file_stream(&state, file_path, channels, frequency);

    FloatArray all_pcm;
    initFloatArray(&all_pcm, state->vi->rate);

    while (!state->eof) {
        /*  pcm is actually an array of floating point arrays, one for each channel of audio.
            If you are decoding stereo, pcm[0] will be the array of left channel samples,
            and pcm[1] will be the right channel.
            As you might expect, pcm[0][0] will be the first sample in the left channel,
            and pcm[1][0] will be the first sample in the right channel.*/
        float** pcm;
        long ret = ov_read_float(&(state->vf), &pcm, max_samples_to_read, &(state->current_section));
        if (ret == 0) {
            /* EOF */
            state->eof = 1;
        }
        else if (ret < 0) {
            return ERROR_READING_OGG_STREAM;
        }
        else {
            for (int j = 0; j < ret; ++j) {
                for (int i = 0; i < state->vi->channels; ++i) {
                    insertFloatArray(&all_pcm, pcm[i][j]);
                }
            }
        }
    }
    *samples_to_fill = all_pcm.array;
    *samples_filled_length = all_pcm.used;

    close_file_stream(state);
    return 0;
}

int32_t read_all_pcm_data_from_memory(
    const char* memory_array,
    const int32_t memory_array_length,
    float** samples,
    int32_t* samples_length,
    int16_t* channels,
    int32_t* frequency,
    const int32_t max_samples_to_read) {

    ov_callbacks callbacks;
    ogg_file t;
    t.curPtr = t.filePtr = memory_array;
    t.fileSize = memory_array_length;

    OggVorbis_File ov;
    memset(&ov, 0, sizeof(OggVorbis_File));

    callbacks.read_func = ogg_file_callback_read_ogg;
    callbacks.seek_func = ogg_file_callback_seek_ogg;
    callbacks.close_func = ogg_file_callback_close_ogg;
    callbacks.tell_func = ogg_file_callback_tell_ogg;

    int ret = ov_open_callbacks((void*)&t, &ov, NULL, -1, callbacks);

    vorbis_info* vi = ov_info(&ov, -1);
    assert(vi);

    FloatArray all_pcm;
    initFloatArray(&all_pcm, max_samples_to_read);

    int current_section;
    float** pcm;
    do
    {
        long ret_read_float = ov_read_float(&ov, &pcm, max_samples_to_read, &current_section);
        if (ret_read_float == 0) {
            break;
        }
        else if (ret_read_float < 0) {
            return ERROR_READING_OGG_STREAM;
        }
        else {
            const int channels = vi->channels;
            for (int j = 0; j < ret_read_float; ++j) {
                for (int i = 0; i < channels; ++i) {
                    insertFloatArray(&all_pcm, pcm[i][j]);
                }
            }
        }
    } while (1);

    *channels = vi->channels;
    *frequency = vi->rate;
    *samples = all_pcm.array;
    *samples_length = all_pcm.used;

    ov_clear(&ov);
    return 0;
}
int32_t EXPORT_API free_samples_array_native_memory(float** samples) {
    if (*samples != NULL) {
        free(*samples);
        *samples = NULL;
    }
    return 0;
}

int32_t open_read_file_stream(vorbis_file_read_stream_state** state, const char* file_path, int16_t* channels, int32_t* frequency) {
    if (file_path == NULL) {
        return ERROR_INVALID_FILEPATH_PARAMETER;
    }

    vorbis_file_read_stream_state* state_to_return = malloc(sizeof(vorbis_file_read_stream_state));
    if (state_to_return == NULL) {
        return ERROR_MALLOC_RETURNED_NULL;
    }
    memset(state_to_return, 0, sizeof(vorbis_file_read_stream_state));
    state_to_return->file_stream = fopen(file_path, "rb");
    if (state_to_return->file_stream == NULL) {
        return ERROR_CANNOT_OPEN_FILE_FOR_READ;
    }

    if (ov_open_callbacks(state_to_return->file_stream, &(state_to_return->vf), NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
        return ERROR_INPUT_FILESTREAM_IS_NOT_OGG_STREAM;
    }

    state_to_return->vi = ov_info(&(state_to_return->vf), -1);
    /*char** ptr = ov_comment(&(state->vf), -1)->user_comments;
    while (*ptr) {
        fprintf(stderr, "%s\n", *ptr);
        ++ptr;
    }*/
    fprintf(stderr, "\nBitstream is %d channel, %ldHz\n", state_to_return->vi->channels, state_to_return->vi->rate);
    //fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&(state->vf), -1)->vendor);
    *channels = state_to_return->vi->channels;
    *frequency = state_to_return->vi->rate;
    *state = state_to_return;
}
int32_t read_from_file_stream(vorbis_file_read_stream_state* state, float* samples_to_fill, const int32_t max_samples_to_read) {
    /*  pcm is actually an array of floating point arrays, one for each channel of audio.
    If you are decoding stereo, pcm[0] will be the array of left channel samples,
    and pcm[1] will be the right channel.
    As you might expect, pcm[0][0] will be the first sample in the left channel,
    and pcm[1][0] will be the first sample in the right channel.*/
    long samples_actually_read = 0;
    do
    {
        float** pcm;
        long samples_to_ask = max_samples_to_read - samples_actually_read;
        long ret = ov_read_float(&(state->vf), &pcm, samples_to_ask, &(state->current_section));
        if (ret == 0) {
            /* EOF */
            state->eof = 1;
        }
        else if (ret < 0) {
            return ERROR_READING_OGG_STREAM;
        }
        else {
            long counter = samples_actually_read;
            const int channels = state->vi->channels;
            for (int j = 0; j < ret; ++j) {
                for (int i = 0; i < channels; ++i) {
                    samples_to_fill[counter++] = pcm[i][j];
                }
            }
        }
        samples_actually_read += ret;
    } while (samples_actually_read < max_samples_to_read && (!state->eof));
    return samples_actually_read;
}
int32_t close_file_stream(vorbis_file_read_stream_state* state) {
    ov_clear(&(state->vf));
    fclose(state->file_stream);
    free(state);
    return 0;
}
