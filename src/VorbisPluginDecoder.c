#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#include "VorbisPlugin.h"
#include "FloatArray.h"

extern void _VDBG_dump(void);

long DecodePcmDataFromFile(
    const char* filePath,
    float** samples_to_fill,
    long* samples_filled_length,
    short* channels,
    long* frequency) {

    if (filePath == NULL) {
        return 1;
    }

    FILE* file_stream = fopen(filePath, "rb");
    if (file_stream == NULL) {
        return 2;
    }

    OggVorbis_File vf;
    int eof = 0;
    int current_section;

    if (ov_open_callbacks(file_stream, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
        fprintf(stderr, "Input does not appear to be an Ogg bitstream.\n");
        return 1;
    }


    char** ptr = ov_comment(&vf, -1)->user_comments;
    vorbis_info* vi = ov_info(&vf, -1);
    while (*ptr) {
        fprintf(stderr, "%s\n", *ptr);
        ++ptr;
    }
    fprintf(stderr, "\nBitstream is %d channel, %ldHz\n", vi->channels, vi->rate);
    fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);
    *channels = vi->channels;
    *frequency = vi->rate;

    Array all_pcm;
    initArray(&all_pcm, vi->rate);

    while (!eof) {

        /*  pcm is actually an array of floating point arrays, one for each channel of audio.
            If you are decoding stereo, pcm[0] will be the array of left channel samples,
            and pcm[1] will be the right channel.
            As you might expect, pcm[0][0] will be the first sample in the left channel,
            and pcm[1][0] will be the first sample in the right channel.*/
        float** pcm;
        long ret = ov_read_float(&vf, &pcm, 1024, &current_section);
        if (ret == 0) {
            /* EOF */
            eof = 1;
        }
        else if (ret < 0) {
            return 2;
        }
        else {
            for (int j = 0; j < ret; ++j) {
                for (int i = 0; i < vi->channels; ++i) {
                    insertArray(&all_pcm, pcm[i][j]);
                }
            }
        }
    }

    *samples_to_fill = all_pcm.array;
    *samples_filled_length = all_pcm.used;
    ov_clear(&vf);
    fclose(file_stream);

    fprintf(stderr, "Done.\n");
    return 0;
}

long EXPORT_API FreeSamplesArrayNativeMemory(float** samples)
{
    if (*samples != NULL)
    {
        free(*samples);
        *samples = NULL;
    }
    return 0;
}
