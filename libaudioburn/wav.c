/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
  Modified from original code included in XMMS-1.2.7 by
  Neill Miller.  Copyright (C) 2003.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "wav.h"

WaveFile *wav_file = NULL;

int read_le_long(FILE * file, long *ret)
{
    unsigned char buf[4];

    if (fread(buf, 1, 4, file) != 4)
        return 0;

    *ret = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
    return 0;
}

int read_le_short(FILE * file, short *ret)
{
    unsigned char buf[2];

    if (fread(buf, 1, 2, file) != 2)
        return 0;

    *ret = (buf[1] << 8) | buf[0];
    return 0;
}

int get_wav_length(char *filename, int *length)
{
    int ret = -1;
    char magic[4];
    unsigned long len;
    int rate;
    WaveFile *wav_file;

    wav_file = malloc(sizeof (WaveFile));
    assert(wav_file);
    memset(wav_file, 0, sizeof (WaveFile));
    if (!(wav_file->file = fopen(filename, "rb")))
    {
        free(wav_file);
        wav_file = NULL;
        return ret;
    }

    fread(magic, 1, 4, wav_file->file);
    if (strncmp(magic, "RIFF", 4))
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    read_le_long(wav_file->file, &len);
    fread(magic, 1, 4, wav_file->file);
    if (strncmp(magic, "WAVE", 4))
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    for (;;)
    {
        fread(magic, 1, 4, wav_file->file);
        read_le_long(wav_file->file, &len);

        if (!strncmp("fmt ", magic, 4))
            break;
        fseek(wav_file->file, len, SEEK_CUR);
    }
    if (len < 16)
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    read_le_short(wav_file->file, &wav_file->format_tag);
    switch (wav_file->format_tag)
    {
        case WAVE_FORMAT_UNKNOWN:
        case WAVE_FORMAT_ALAW:
        case WAVE_FORMAT_MULAW:
        case WAVE_FORMAT_ADPCM:
        case WAVE_FORMAT_OKI_ADPCM:
        case WAVE_FORMAT_DIGISTD:
        case WAVE_FORMAT_DIGIFIX:
        case IBM_FORMAT_MULAW:
        case IBM_FORMAT_ALAW:
        case IBM_FORMAT_ADPCM:
            fclose(wav_file->file);
            free(wav_file);
            wav_file = NULL;
            return ret;
    }
    read_le_short(wav_file->file, &wav_file->channels);
    read_le_long(wav_file->file, &wav_file->samples_per_sec);
    read_le_long(wav_file->file, &wav_file->avg_bytes_per_sec);
    read_le_short(wav_file->file, &wav_file->block_align);
    read_le_short(wav_file->file, &wav_file->bits_per_sample);
    if (wav_file->bits_per_sample != 8 && wav_file->bits_per_sample != 16)
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    len -= 16;
    if (len)
        fseek(wav_file->file, len, SEEK_CUR);

    for (;;)
    {
        fread(magic, 4, 1, wav_file->file);
        read_le_long(wav_file->file, &len);

        if (!strncmp("data", magic, 4))
            break;
        fseek(wav_file->file, len, SEEK_CUR);
    }
    rate = wav_file->samples_per_sec *
        wav_file->channels * (wav_file->bits_per_sample / 8);

    (*length) = (len / rate);
    ret = 0;

    fclose(wav_file->file);
    free(wav_file);
    wav_file = NULL;
    return ret;
}

int get_wav_format_information(char *filename, int *num_channels,
                               int *num_bits, int *sample_rate)
{
    int ret = -1;
    char magic[4];
    unsigned long len;
    WaveFile *wav_file;

    if (!filename || !num_channels || !num_bits || !sample_rate)
    {
        return ret;
    }

    wav_file = malloc(sizeof (WaveFile));
    assert(wav_file);
    memset(wav_file, 0, sizeof (WaveFile));
    if (!(wav_file->file = fopen(filename, "rb")))
    {
        free(wav_file);
        wav_file = NULL;
        return ret;
    }

    fread(magic, 1, 4, wav_file->file);
    if (strncmp(magic, "RIFF", 4))
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    read_le_long(wav_file->file, &len);
    fread(magic, 1, 4, wav_file->file);
    if (strncmp(magic, "WAVE", 4))
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    for (;;)
    {
        fread(magic, 1, 4, wav_file->file);
        read_le_long(wav_file->file, &len);

        if (!strncmp("fmt ", magic, 4))
            break;
        fseek(wav_file->file, len, SEEK_CUR);
    }
    if (len < 16)
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    read_le_short(wav_file->file, &wav_file->format_tag);
    switch (wav_file->format_tag)
    {
        case WAVE_FORMAT_UNKNOWN:
        case WAVE_FORMAT_ALAW:
        case WAVE_FORMAT_MULAW:
        case WAVE_FORMAT_ADPCM:
        case WAVE_FORMAT_OKI_ADPCM:
        case WAVE_FORMAT_DIGISTD:
        case WAVE_FORMAT_DIGIFIX:
        case IBM_FORMAT_MULAW:
        case IBM_FORMAT_ALAW:
        case IBM_FORMAT_ADPCM:
            fclose(wav_file->file);
            free(wav_file);
            wav_file = NULL;
            return ret;
    }
    read_le_short(wav_file->file, &wav_file->channels);
    read_le_long(wav_file->file, &wav_file->samples_per_sec);
    read_le_long(wav_file->file, &wav_file->avg_bytes_per_sec);
    read_le_short(wav_file->file, &wav_file->block_align);
    read_le_short(wav_file->file, &wav_file->bits_per_sample);
    if (wav_file->bits_per_sample != 8 && wav_file->bits_per_sample != 16)
    {
        fclose(wav_file->file);
        free(wav_file);
        wav_file = NULL;
        return ret;
    }
    len -= 16;
    if (len)
        fseek(wav_file->file, len, SEEK_CUR);

    for (;;)
    {
        fread(magic, 4, 1, wav_file->file);
        read_le_long(wav_file->file, &len);

        if (!strncmp("data", magic, 4))
            break;
        fseek(wav_file->file, len, SEEK_CUR);
    }

    *num_channels = wav_file->channels;
    *num_bits = wav_file->bits_per_sample;
    *sample_rate = wav_file->samples_per_sec;
    ret = 0;

    fclose(wav_file->file);
    free(wav_file);
    wav_file = NULL;
    return ret;
}
