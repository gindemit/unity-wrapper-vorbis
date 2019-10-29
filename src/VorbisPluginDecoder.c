#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#include "VorbisPlugin.h"
#include "FloatArray.h"
#include "ErrorCodes.h"

extern void _VDBG_dump(void);

typedef struct VorbisFileReadStreamState {
    OggVorbis_File vf;
    int eof;
    int current_section;
    vorbis_info* vi;
} VorbisFileReadStreamState;


VorbisFileReadStreamState* ReadHeaderFromFileStream(
    FILE* file_stream,
    short* channels,
    long* frequency) {

    VorbisFileReadStreamState* state = malloc(sizeof(VorbisFileReadStreamState));
    if (state == NULL) {
        return NULL;
    }
    memset(state, 0, sizeof(VorbisFileReadStreamState));

    if (ov_open_callbacks(file_stream, &(state->vf), NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
        fprintf(stderr, "Input does not appear to be an Ogg bitstream.\n");//TODO: use logger callback
        return NULL;
    }

    char** ptr = ov_comment(&(state->vf), -1)->user_comments;
    state->vi = ov_info(&(state->vf), -1);
    while (*ptr) {
        fprintf(stderr, "%s\n", *ptr);
        ++ptr;
    }
    fprintf(stderr, "\nBitstream is %d channel, %ldHz\n", state->vi->channels, state->vi->rate);
    fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&(state->vf), -1)->vendor);
    *channels = state->vi->channels;
    *frequency = state->vi->rate;
    return state;
}

long ReadAllPcmDataFromFileStream(
    FILE* file_stream,
    float** samples_to_fill,
    long* samples_filled_length,
    short* channels,
    long* frequency,
    const long maxSamplesToRead) {

    VorbisFileReadStreamState* state = ReadHeaderFromFileStream(file_stream, channels, frequency);

    Array all_pcm;
    initArray(&all_pcm, state->vi->rate);

    while (!state->eof) {

        /*  pcm is actually an array of floating point arrays, one for each channel of audio.
            If you are decoding stereo, pcm[0] will be the array of left channel samples,
            and pcm[1] will be the right channel.
            As you might expect, pcm[0][0] will be the first sample in the left channel,
            and pcm[1][0] will be the first sample in the right channel.*/
        float** pcm;
        long ret = ov_read_float(&(state->vf), &pcm, maxSamplesToRead, &(state->current_section));
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
                    insertArray(&all_pcm, pcm[i][j]);
                }
            }
        }
    }

    *samples_to_fill = all_pcm.array;
    *samples_filled_length = all_pcm.used;

    ov_clear(&(state->vf));
    free(state);
    fclose(file_stream);

    fprintf(stderr, "Done.\n");
    return 0;
}

FILE* OpenFileToStreamFrom(const char* file_path) {
    if (file_path == NULL) {
        //TODO: add message to logger
        return NULL;
    }

    FILE* file_stream = fopen(file_path, "rb");
    if (file_stream == NULL) {
        //TODO: add message to logger
        return NULL;
    }
    return file_stream;
}

long ReadAllPcmDataFromFile(
    const char* file_path,
    float** samples_to_fill,
    long* samples_filled_length,
    short* channels,
    long* frequency,
    const long maxSamplesToRead) {

    if (file_path == NULL) {
        return ERROR_INVALID_FILEPATH_PARAMETER;
    }

    FILE* file_stream = OpenFileToStreamFrom(file_path);
    if (file_stream == NULL) {
        return ERROR_CANNOT_OPEN_FILE_FOR_READ;
    }
    return ReadAllPcmDataFromFileStream(
        file_stream,
        samples_to_fill,
        samples_filled_length,
        channels,
        frequency,
        maxSamplesToRead);
}

long EXPORT_API FreeSamplesArrayNativeMemory(float** samples) {
    if (*samples != NULL) {
        free(*samples);
        *samples = NULL;
    }
    return 0;
}
