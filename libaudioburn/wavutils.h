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
#ifndef __WAVUTILS_H
#define __WAVUTILS_H


/*
  must be called before using any wavutils methods.
  returns 0 on success; 1 otherwise
*/
int wav_initialize(settings_t *settings);

/*
  verifies that the wav file specified exists and
  that it is in 16 bit, stereo 44100 Hz format.

  returns -1 if the file does not exist;
  returns 0 on success;
  returns 1 on error;
  returns 2 if the file exists but is in the wrong format
*/
int wav_verify_format(char *filename);


/*
  retrieves whether the input file 'filename' is stereo
  or mono, and which sampling rate it is encoded as (if
  available).

  the information is passed back through the outgoing
  arguments included:
  'num_channels', 'num_bits', and 'sampling_rate'.

  returns 0 on success; returns 1 otherwise 
*/
int wav_get_file_information(char *filename,
                             int *num_channels,
                             int *num_bits,
                             int *sample_rate);

/*
  returns the length of the specified wav file
  'filename' on success;  returns -1 otherwise.
*/
int wav_get_song_length(char *filename);

#endif /* __WAVUTILS_H */
