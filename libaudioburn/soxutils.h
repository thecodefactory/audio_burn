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
#ifndef __SOXUTILS_H
#define __SOXUTILS_H

/*
  must be called before using any soxutil methods.
  returns 0 on success; 1 otherwise
*/
int sox_initialize(settings_t *settings);

/*
  attempts to resample the wav file 'input_file'
  to be valid for cd-burning into the specified
  output file 'output_file'.

  NOTE: the callback will only be called twice for
  this operation.  It may be wise to poll inside
  of the callback until the child process has
  terminated, rather than returning immediately.
  You can use the child_still_running util method
  (found in processutils) to poll the process and
  report completion.  the last time it is called
  can be distinguished by checking if 'buf' and
  'buflen' are NULL in your callback.

  returns 0 on success; returns 1 otherwise
*/
int sox_resample_wav(settings_t *settings,
                     char *input_file,
                     char *output_file,
                     fn_callback_t fnptr);

#endif /* __SOXUTILS_H */
