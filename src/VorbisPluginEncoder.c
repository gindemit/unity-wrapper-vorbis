#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <vorbis/vorbisenc.h>

#include "VorbisPlugin.h"
#include "ErrorCodes.h"
#include "UnsignedCharArray.h"

#define MEMORY_ARRAY_INITIAL_SIZE 4096

static int32_t write_all_pcm_data_using_on_write_callback(
    void (*on_write_callback)(
        const void* write_to_impl,
        const unsigned char* buffer,
        const size_t element_size,
        const size_t element_count),
    const void *write_to_impl,
    const float *samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samples_to_write) {

    if (on_write_callback == NULL) {
        return ERROR_INVALID_WRITE_CALLBACK_PARAMETER;
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
    if (frequency < 8000 || frequency > 192000) {
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

    ogg_stream_init(&os, 0x04030201);

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
            on_write_callback(write_to_impl, og.header, 1, og.header_len);
            on_write_callback(write_to_impl, og.body, 1, og.body_len);
        }

    }

    int eos = 0;
    long j = 0;
    while (!eos) {
        long to_read = samples_to_write;
        if (j + to_read > samples_length) {
            to_read = samples_length - j;
        }

        if (to_read == 0) {
            vorbis_analysis_wrote(&vd, 0);
        }
        else
        {
            float** buffer = vorbis_analysis_buffer(&vd, to_read);

            long i;
            for (i = 0; i < to_read; i++) {
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
                    on_write_callback(write_to_impl, og.header, 1, og.header_len);
                    on_write_callback(write_to_impl, og.body, 1, og.body_len);

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

static void on_write_to_file_stream(
    const void* file_stream_void,
    const unsigned char* buffer,
    const size_t element_size,
    const size_t element_count) {

    FILE* file_stream = (FILE*)file_stream_void;
    fwrite(buffer, element_size, element_count, file_stream);
}
int32_t write_all_pcm_data_to_file(
    const char* file_path,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samples_to_read) {

    if (file_path == NULL) {
        return ERROR_INVALID_FILEPATH_PARAMETER;
    }
    /* Open file stream to write in */
    FILE *file_stream = fopen(file_path, "wb");
    if (file_stream == NULL) {
        return ERROR_CANNOT_OPEN_FILE_FOR_WRITE;
    }
    int32_t result = write_all_pcm_data_using_on_write_callback(
        on_write_to_file_stream,
        file_stream,
        samples,
        samples_length,
        channels,
        frequency,
        base_quality,
        samples_to_read);
    fflush(file_stream);
    fclose(file_stream);
    return result;
}
static void on_write_to_unsigned_char_array(
    const void* unsigned_char_buffer_void,
    const unsigned char* buffer,
    const size_t element_size,
    const size_t element_count) {

    if (element_size != 1) {
        return;
    }
    UnsignedCharArray* unsigned_char_array = (UnsignedCharArray*)unsigned_char_buffer_void;
    for (size_t i = 0; i < element_count; ++i) {
        insertUnsignedCharArray(unsigned_char_array, buffer[i]);
    }
}
int32_t write_all_pcm_data_to_memory(
    char** memory_array,
    int32_t* memory_array_length,
    const float* samples,
    const int32_t samples_length,
    const int16_t channels,
    const int32_t frequency,
    const float base_quality,
    const int32_t samples_to_read) {

    UnsignedCharArray char_array;
    initUnsignedCharArray(&char_array, MEMORY_ARRAY_INITIAL_SIZE);
    int32_t result = write_all_pcm_data_using_on_write_callback(
        on_write_to_unsigned_char_array,
        &char_array,
        samples,
        samples_length,
        channels,
        frequency,
        base_quality,
        samples_to_read);
    *memory_array = char_array.array;
    *memory_array_length = char_array.used;
    return result;
}
int32_t free_memory_array_for_write_all_pcm_data(char* memory_array) {
    if (memory_array == NULL) {
        return ERROR_BYTES_MEMORY_ARRAY_NULL;
    }
    free(memory_array);
    return 0;
}