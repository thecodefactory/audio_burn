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

int mpg_initialize(settings_t *settings)
{
    int ret = 1;
    struct stat statbuf;

    if (settings)
    {
        if (which("mpg321",settings->mpgdec_path,
                  MAX_PATH_LEN) == 0)
        {
            ret = 0;
        }
        else
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(MPGDEC_PATH,&statbuf) ||
                (!(statbuf.st_mode & S_IXUSR)))
            {
                fprintf(stderr,"Cannot find required program "
                        "mpg321.\nPlease install this program "
                        "either from your vendor, or visit:\n\n%s\n",
                        MPG321_DOWNLOAD_SITE);
            }
            else
            {
                memcpy(settings->mpgdec_path,MPGDEC_PATH,
                       MIN(strlen(MPGDEC_PATH),MAX_PATH_LEN-1));
                ret = 0;
            }
        }
    }
    return ret;
}

int mpg_decode(settings_t *settings, char *input_file,
               char *output_file, fn_callback_t fnptr)
{
    int ret = 1;
    char *args[6] = {(char *)0,(char *)0,
                     (char *)0,(char *)0,
                     (char *)0,(char *)0};

    if (settings && input_file && output_file)
    {
        args[0] = settings->mpgdec_path;
        args[1] = "-v";
        args[2] = "-w";
        args[3] = output_file;
        args[4] = input_file;
        args[5] = NULL;

        ret = fork_execute_with_io_callback(settings->mpgdec_path,
                                            args,fnptr,
                                            CHILD_IO_ERR);
    }
    return ret;
}

float mpg_get_percent_complete(char *buf, int buflen)
{
    float ret = -1.0f;
    char *ptr = (char *)0;
    char *end = (char *)0;
    char *start = (char *)0;
    static int total_frames = 0;
    int current_frame = 0;

    if (buf && buflen)
    {
        /*
          gather the total (approximate) number
          of frames if we don't know it yet
        */
        if (total_frames == 0)
        {
            end = (char *)(buf + buflen);

            for(ptr = buf; ptr < end; ptr++)
            {
                if (*ptr == '[')
                {
                    while(*ptr && (*ptr != ']'))
                    {
                        if (!start && isdigit((int)*ptr))
                        {
                            start = ptr;
                        }
                        ptr++;
                    }
                    if (*ptr == ']')
                    {
                        total_frames = atoi(start);
                    }
                    break;
                }
                if (total_frames)
                {
                    break;
                }
            }
        }

        /* get the current frame that is being processed */
        ptr = strstr(buf,"Frame#");

        if (ptr)
        {
            while(*ptr && (!isdigit((int)*ptr)))
            {
                ptr++;
            }
            start = ptr;

            while(*ptr && (isdigit((int)*ptr)))
            {
                ptr++;
            }
            *ptr = '\0';

            current_frame = atoi(start);

            if (total_frames != 0)
            {
                ret = (float)((float)current_frame /
                              (float)total_frames);
                ret *= 100.0f;
            }
        }

        /* reset the static total frame holder */
        if (current_frame >= total_frames)
        {
            total_frames = 0;
        }
    }
    return ((ret > 100.0f) ? 100.0f : ret);
}

int mpg_get_song_length(char *filename)
{
    int ret = -1;

    if (filename)
    {
        ret = get_song_time(filename);
        ret = ((ret == 0) ? -1 : ret);
    }
    return ret;
}
