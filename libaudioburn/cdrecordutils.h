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
#ifndef __CDRECORDUTILS_H
#define __CDRECORDUTILS_H

typedef struct
{
    int num_devices;

    int scsibuses[MAX_NUM_DEVICES];
    int targets[MAX_NUM_DEVICES];
    int luns[MAX_NUM_DEVICES];
    int labels[MAX_NUM_DEVICES];
    char devices[MAX_NUM_DEVICES][MAX_DEVICENAME_LEN];
} cdrecorder_t;

/*
  must be called before using any cdrecordutil methods.
  returns 0 on success; returns 1 otherwise
*/
int cdrecord_initialize(settings_t *settings);

/*
  fills in information about the cdrecorder(s) installed
  in the output struct 'recorders' and returns 0 on success;
  returns 1 otherwise
*/
int cdrecord_get_recorder_info(settings_t *settings,
                               cdrecorder_t *recorders);

/*
  prints the contents of the specified cdrecoder_t struct
  to stdout (if it's valid) in a nice readable format
*/
void cdrecord_print_recorder_info(cdrecorder_t *recorders);

/*
  attempts to burn an audio cd given the settings object and
  a valid cdrecorder object. the settings->output_files array
  is the list used for burning to CD.

  if the settings->dummy_run variable is set, the cdrecorder
  will pretend to burn the disc without actually writing data.

  the provider function pointer 'fnptr' will be called
  when there is output from cdrecord available.

  returns 0 on success; returns 1 otherwise
*/
int cdrecord_burn_audio_cd(settings_t *settings,
                           cdrecorder_t *recorders,
                           fn_callback_t fnptr);

#endif /* __CDRECORDUTILS_H */
