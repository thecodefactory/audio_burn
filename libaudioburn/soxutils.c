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

int sox_initialize(settings_t *settings)
{
    int ret = 1;
    struct stat statbuf;

    if (settings)
    {
        if (which("sox",settings->sox_path,
                  MAX_PATH_LEN) == 0)
        {
            ret = 0;
        }
        else
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(SOX_PATH,&statbuf) ||
                (!(statbuf.st_mode & S_IXUSR)))
            {
                fprintf(stderr,"Cannot find required program "
                        "sox.\nPlease install this program "
                        "either from your vendor, or visit:\n\n%s\n",
                        SOX_DOWNLOAD_SITE);
            }
            else
            {
                memcpy(settings->sox_path,SOX_PATH,
                       MIN(strlen(SOX_PATH),MAX_PATH_LEN-1));
                ret = 0;
            }
        }
    }
    return ret;
}

int sox_resample_wav(settings_t *settings,
                     char *input_file,
                     char *output_file,
                     fn_callback_t fnptr)
{
    int ret = 1;
    int arg_index = 0;
    int num_channels = 0;
    int num_bits = 0;
    int sample_rate = 0;
    char *args[10] = {(char *)0,(char *)0,(char *)0,(char *)0,
                      (char *)0,(char *)0,(char *)0,(char *)0,
                      (char *)0,(char *)0};

    if (settings && input_file && output_file &&
        (wav_get_file_information(input_file,&num_channels,
                                  &num_bits,&sample_rate) == 0))
    {
        args[arg_index++] = settings->sox_path;
        args[arg_index++] = "-V";
        args[arg_index++] = input_file;
        args[arg_index++] = "-r";
        args[arg_index++] = "44100";
        args[arg_index++] = "-c";
        args[arg_index++] = "2";

        /* force output to 16 bit (word) size (if not already) */
        if (num_bits != 16)
        {
            args[arg_index++] = "-w";
        }
        args[arg_index++] = output_file;
        args[arg_index++] = NULL;

        ret = fork_execute_with_io_callback(settings->sox_path,
                                            args,fnptr,
                                            CHILD_IO_ERR);
    }
    return ret;
}
