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
#ifndef __MPGUTILS_H
#define __MPGUTILS_H

/*
  must be called before using any mpgutil methods.
  returns 0 on success; 1 otherwise
*/
int mpg_initialize(settings_t *settings);

/*
  decodes a specified input mp3 file ('input_file') to
  'output_file'.  The resulting 'output_file' will be
  in .wav format on success.

  the provided callback function 'fnptr' will be
  called with output from the mpgdecoding session each
  time new data is available for reading. the data
  passed to the callback will never be larger than
  MAX_BUF_LEN for any given call.

  returns 0 on success; 1 otherwise
*/
int mpg_decode(settings_t *settings,
               char *input_file,
              char *output_file,
              fn_callback_t fnptr);

/*
  given a line of output from the mpgdecode callback
  buffer, this method finds the percent complete in it
  and returns that as a float if it's present.
  if no percent is present in the input buffer,
  a value of -1.0 is returned.

  the buffer data passed to this function is no longer
  valid after this function returns. (i.e. it's modified)
*/
float mpg_get_percent_complete(char *buf, int buflen);

/*
  returns the song length of the mp3 file in seconds
  on success; returns -1 otherwise
*/
int mpg_get_song_length(char *filename);


#endif /* __MPGUTILS_H */
