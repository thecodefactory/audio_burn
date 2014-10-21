/*
  libaudioburn - an audio cd burning library
  Copyright (C) 2003  Neill Miller
  This file is part of libaudioburn.

  libaudioburn is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  libaudioburn is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libaudioburn; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.
*/
#include "libaudioburn.h"

int wav_verify_format(char *filename)
{
    int ret = 1;
    int num_channels = 0, num_bits = 0, sample_rate = 0;

    if (filename)
    {
        if (wav_get_file_information(
                filename, &num_channels, &num_bits, &sample_rate) == 0)
        {
            ret = (((num_bits == 16) && (sample_rate == 44100) &&
                    (num_channels == 2)) ? 0 : 2);
        }
    }
    return ret;
}

int wav_get_file_information(char *filename,
                             int *num_channels,
                             int *num_bits,
                             int *sample_rate)
{
    return (get_wav_format_information(
                filename, num_channels, num_bits, sample_rate) ? 1 : 0);
}

int wav_get_song_length(char *filename)
{
    int ret = -1;
    int length = 0;

    if (filename)
    {
        if (get_wav_length(filename,&length) == 0)
        {
            ret = length;
        }
    }
    return ret;
}
