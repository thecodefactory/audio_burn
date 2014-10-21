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
#ifndef __NORMUTILS_H
#define __NORMUTILS_H

/*
  must be called before using any normutil methods.
  returns 0 on success; 1 otherwise
*/
int normalize_initialize(settings_t *settings);

/*
  normalizes a specified input .wav file ('input_file')
  to 'output_file'.  The resulting 'output_file' will be
  in .wav format on success.

  the provided callback function 'fnptr' will be
  called with output from the normdecoding session each
  time new data is available for reading. the data
  passed to the callback will never be larger than
  MAX_BUF_LEN for any given call.

  returns 0 on success; 1 otherwise
*/
int normalize_file(settings_t *settings,
                   char *input_file,
                   fn_callback_t fnptr);

/*
  the same as normalize_file, except this causes the
  normalization to be done in batch mode over all files
  specified in the null-terminated 'input_files' string
  array.

  the 'input_files' array MUST have a NULL value as
  the last element of the array.

  returns 0 on success; 1 otherwise
*/
int normalize_file_list(settings_t *settings,
                        char *input_files[],
                        fn_callback_t fnptr);


/*
  given a line of output from the normalizing callback
  buffer, this method finds the percent complete in it
  and returns that as a float if it's present.
  if no percent is present in the input buffer,
  a value of -1.0 is returned.

  the buffer data passed to this function is no longer
  valid after this function returns. (i.e. it's modified)
*/
float normalize_get_percent_complete(char *buf, int buflen);

#endif /* __NORMUTILS_H */
