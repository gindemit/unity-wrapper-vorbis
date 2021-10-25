#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>

#include "VorbisPlugin.h"
#include "FloatArray.h"
#include "ErrorCodes.h"

extern void _VDBG_dump(void);

int32_t ReadAllPcmDataFromFile(
    const char* file_path,
    float** samples_to_fill,
    int32_t* samples_filled_length,
    int16_t* channels,
    int32_t* frequency,
    const int32_t max_samples_to_read) {

    VorbisFileReadStreamState* state = OpenReadFileStream(file_path, channels, frequency);

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

    CloseFileStream(state);
    return 0;
}
EXPORT_API int32_t ReadAllPcmDataFromMemory(
    const char* memory_array,
    const int32_t memory_array_length,
    float** samples,
    int32_t* samples_length,
    int16_t* channels,
    int32_t* frequency,
    const int32_t maxSamplesToRead) {


}
int32_t EXPORT_API FreeSamplesArrayNativeMemory(float** samples) {
    if (*samples != NULL) {
        free(*samples);
        *samples = NULL;
    }
    return 0;
}

VorbisFileReadStreamState* OpenReadFileStream(const char* file_path, int16_t* channels, int32_t* frequency) {
    if (file_path == NULL) {
        //TODO: add message to logger
        return NULL;
    }

    VorbisFileReadStreamState* state = malloc(sizeof(VorbisFileReadStreamState));
    if (state == NULL) {
        return NULL;
    }
    memset(state, 0, sizeof(VorbisFileReadStreamState));
    state->file_stream = fopen(file_path, "rb");
    if (state->file_stream == NULL) {
        return NULL;
    }

    if (ov_open_callbacks(state->file_stream, &(state->vf), NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
        fprintf(stderr, "Input does not appear to be an Ogg bitstream.\n");//TODO: use logger callback
        return NULL;
    }

    state->vi = ov_info(&(state->vf), -1);
    /*char** ptr = ov_comment(&(state->vf), -1)->user_comments;
    while (*ptr) {
        fprintf(stderr, "%s\n", *ptr);
        ++ptr;
    }*/
    fprintf(stderr, "\nBitstream is %d channel, %ldHz\n", state->vi->channels, state->vi->rate);
    //fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&(state->vf), -1)->vendor);
    *channels = state->vi->channels;
    *frequency = state->vi->rate;
    return state;
}
int32_t ReadFromFileStream(VorbisFileReadStreamState* state, float* samples_to_fill, const int32_t max_samples_to_read) {
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
int32_t CloseFileStream(VorbisFileReadStreamState* state) {
    ov_clear(&(state->vf));
    fclose(state->file_stream);
    free(state);
    return 0;
}
