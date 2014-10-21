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

int ogg_initialize(settings_t *settings)
{
    int ret = 1;
    struct stat statbuf;

    if (settings)
    {
        if (which("oggdec",settings->oggdec_path,
                  MAX_PATH_LEN) == 0)
        {
            ret = 0;
        }
        else
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(OGGDEC_PATH,&statbuf) ||
                (!(statbuf.st_mode & S_IXUSR)))
            {
                fprintf(stderr,"Cannot find required program "
                        "oggdec.\nPlease install this program "
                        "either from your vendor, or visit:\n\n%s\n",
                        OGGDEC_DOWNLOAD_SITE);
            }
            else
            {
                memcpy(settings->oggdec_path,OGGDEC_PATH,
                       MIN(strlen(OGGDEC_PATH),MAX_PATH_LEN-1));
                ret = 0;
            }
        }
    }
    return ret;
}

int ogg_decode(settings_t *settings, char *input_file,
               char *output_file, fn_callback_t fnptr)
{
    int ret = 1;
    char *args[5] = {(char *)0,(char *)0,
                     (char *)0,(char *)0,
                     (char *)0};

    if (settings && input_file && output_file)
    {
        args[0] = settings->oggdec_path;
        args[1] = input_file;
        args[2] = "-o";
        args[3] = output_file;
        args[4] = NULL;

        ret = fork_execute_with_io_callback(settings->oggdec_path,
                                            args,fnptr,
                                            CHILD_IO_ERR);
    }
    return ret;
}

float ogg_get_percent_complete(char *buf, int buflen)
{
    float ret = -1.0f;
    int complete = 0;
    char *ptr = (char *)0;
    char *end = (char *)0;
    char *start = (char *)0;

    if (buf && buflen)
    {
        end = (char *)(buf + buflen);

        for(ptr = buf; ptr < end; ptr++)
        {
            /* extract first percent of the form: [ xy.z%] */
            if (*ptr == '[')
            {
                while(*ptr && (*ptr != ']'))
                {
                    if (!start && isdigit((int)*ptr))
                    {
                        start = ptr;
                    }
                    else if (*ptr == '%')
                    {
                        *ptr = '\0';
                        complete = 1;
                        break;
                    }
                    ptr++;
                }
                if (complete)
                {
                    ret = (float)atof(start);
                }
                break;
            }
            if (ret != -1.0f)
            {
                break;
            }
        }
    }
    return ret;
}

int ogg_get_song_length(char *filename)
{
    int ret = -1;
    int length = 0;

    if (filename)
    {
        if (vorbis_get_song_length(filename,&length) == 0)
        {
            ret = length;
        }
    }
    return ret;
}
