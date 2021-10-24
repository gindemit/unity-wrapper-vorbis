#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>

#include "VorbisPlugin.h"
#include "ErrorCodes.h"
#include "fmem.h"

static fmem fm;

static int32_t write_all_pcm_data_to_file_stream(
    FILE* file_stream,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samplesToRead) {

    if (file_stream == NULL) {
        return ERROR_INVALID_FILESTREAM_PARAMETER;
    }
    if (samples == NULL) {
        return ERROR_INVALID_SAMPLES_PARAMETER;
    }
    if (samples_length <= 0) {
        return ERROR_INVALID_SAMPLESLENGTH_PARAMETER;
    }
    if (channels != 1 && channels != 2) {
        return ERROR_INVALID_CHANNELS_PARAMETER;
    }
    if (frequency < 44100 || frequency > 192000) {
        return ERROR_INVALID_FREQUENCY_PARAMETER;
    }
    if (base_quality < 0 || base_quality > 1) {
        return ERROR_INVALID_BASE_QUALITY_PARAMETER;
    }

    ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
    ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet       op; /* one raw packet of data for decode */

    vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
    vorbis_comment   vc; /* struct that stores all the user comments */

    vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
    vorbis_block     vb; /* local working space for packet->PCM decode */

    vorbis_info_init(&vi);

    /* choose an encoding mode.  A few possibilities commented out, one
       actually used: */

       /*********************************************************************
        Encoding using a VBR quality mode.  The usable range is -.1
        (lowest quality, smallest file) to 1. (highest quality, largest file).
        Example quality mode .4: 44kHz stereo coupled, roughly 128kbps VBR

        ret = vorbis_encode_init_vbr(&vi,2,44100,.4);

        ---------------------------------------------------------------------

        Encoding using an average bitrate mode (ABR).
        example: 44kHz stereo coupled, average 128kbps VBR

        ret = vorbis_encode_init(&vi,2,44100,-1,128000,-1);

        ---------------------------------------------------------------------

        Encode using a quality mode, but select that quality mode by asking for
        an approximate bitrate.  This is not ABR, it is true VBR, but selected
        using the bitrate interface, and then turning bitrate management off:

        ret = ( vorbis_encode_setup_managed(&vi,2,44100,-1,128000,-1) ||
                vorbis_encode_ctl(&vi,OV_ECTL_RATEMANAGE2_SET,NULL) ||
                vorbis_encode_setup_init(&vi));

        *********************************************************************/

    int ret = vorbis_encode_init_vbr(&vi, channels, frequency, base_quality);

    /* do not continue if setup failed; this can happen if we ask for a
       mode that libVorbis does not support (eg, too low a bitrate, etc,
       will return 'OV_EIMPL') */

    if (ret) {
        return ret;
    }

    /* add a comment */
    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc, "ENCODER", "Ogg Vorbis Plugin For Unity by Gindemit Konstantin");

    /* set up the analysis state and auxiliary encoding storage */
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);

    /* set up our packet->stream encoder */
    /* pick a random serial number; that way we can more likely build
       chained streams just by concatenation */
    srand(time(NULL));
    ogg_stream_init(&os, rand());

    /* Vorbis streams begin with three headers; the initial header (with
       most of the codec setup parameters) which is mandated by the Ogg
       bitstream spec.  The second header holds any comment fields.  The
       third header holds the bitstream codebook.  We merely need to
       make the headers, then pass them to libvorbis one at a time;
       libvorbis handles the additional Ogg bitstream constraints */
    {
        ogg_packet header;
        ogg_packet header_comm;
        ogg_packet header_code;

        vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
        ogg_stream_packetin(&os, &header); /* automatically placed in its own
                                             page */
        ogg_stream_packetin(&os, &header_comm);
        ogg_stream_packetin(&os, &header_code);

        /* This ensures the actual
         * audio data will start on a new page, as per spec
         */
        while (1) {
            int result = ogg_stream_flush(&os, &og);
            if (result == 0) {
                break;
            }
            fwrite(og.header, 1, og.header_len, file_stream);
            fwrite(og.body, 1, og.body_len, file_stream);
        }

    }

    int eos = 0;
    long j = 0;
    while (!eos) {
        long toRead = samplesToRead;
        if (j + toRead > samples_length) {
            toRead = samples_length - j;
        }

        if (toRead == 0) {
            vorbis_analysis_wrote(&vd, 0);
        }
        else
        {
            float** buffer = vorbis_analysis_buffer(&vd, toRead);

            long i;
            for (i = 0; i < toRead; i++) {
                buffer[0][i] = samples[j++];
                if (channels == 2) {
                    buffer[1][i] = samples[j++];
                }
            }

            vorbis_analysis_wrote(&vd, i);
        }
        /* vorbis does some data preanalysis, then divvies up blocks for
           more involved (potentially parallel) processing.  Get a single
           block for encoding now */
        while (vorbis_analysis_blockout(&vd, &vb) == 1) {

            /* analysis, assume we want to use bitrate management */
            vorbis_analysis(&vb, NULL);
            vorbis_bitrate_addblock(&vb);

            while (vorbis_bitrate_flushpacket(&vd, &op)) {

                /* weld the packet into the bitstream */
                ogg_stream_packetin(&os, &op);

                /* write out pages (if any) */
                while (!eos) {
                    int result = ogg_stream_pageout(&os, &og);
                    if (result == 0)break;
                    fwrite(og.header, 1, og.header_len, file_stream);
                    fwrite(og.body, 1, og.body_len, file_stream);

                    /* this could be set above, but for illustrative purposes, I do
                       it here (to show that vorbis does know where the stream ends) */

                    if (ogg_page_eos(&og))eos = 1;
                }
            }
        }
    }

    /* clean up and exit.  vorbis_info_clear() must be called last */
    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
    return 0;
}

int32_t WriteAllPcmDataToFile(
    const char* file_path,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samplesToRead) {

    if (file_path == NULL) {
        return ERROR_INVALID_FILEPATH_PARAMETER;
    }
    /* Open file stream to write in */
    FILE* file_stream = fopen(file_path, "wb");
    if (file_stream == NULL) {
        return ERROR_CANNOT_OPEN_FILE_FOR_WRITE;
    }
    int32_t result = write_all_pcm_data_to_file_stream(
        file_stream,
        samples,
        samples_length,
        channels,
        frequency,
        base_quality,
        samplesToRead);
    fclose(file_stream);
    return result;
}
int32_t WriteAllPcmDataToMemory(
    char** memory_array,
    int32_t* memory_array_length,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samplesToRead) {

    /* Open file stream to write in */
    size_t size_of_final_buffer = 0;
    FILE* file_stream = fmem_open(&fm, "w+");
    if (file_stream == NULL) {
        return ERROR_CANNOT_OPEN_FILE_FOR_WRITE;
    }
    int32_t result = write_all_pcm_data_to_file_stream(
        file_stream,
        samples,
        samples_length,
        channels,
        frequency,
        base_quality,
        samplesToRead);
    fseek(file_stream, 0, SEEK_END);
    size_of_final_buffer = ftell(file_stream);
    fseek(file_stream, 0, SEEK_SET);
    char* in_memory_data = malloc(size_of_final_buffer + 1);
    if (in_memory_data == NULL) {
        return ERROR_MALLOC_RETURNED_NULL;
    }
    fread(in_memory_data, 1, size_of_final_buffer, file_stream);
    *memory_array = in_memory_data;
    *memory_array_length = size_of_final_buffer;
    fclose(file_stream);
    return result;
}