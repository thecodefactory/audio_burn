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

int normalize_initialize(settings_t *settings)
{
    int ret = 1;
    struct stat statbuf;

    if (settings)
    {
        if (which("normalize",settings->normalize_path,
                  MAX_PATH_LEN) == 0)
        {
            ret = 0;
        }
        else
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(NORMALIZE_PATH,&statbuf) ||
                (!(statbuf.st_mode & S_IXUSR)))
            {
                fprintf(stderr,"Cannot find required program "
                        "normalize.\nPlease install this program "
                        "either from your vendor, or visit:\n\n%s\n",
                        NORMALIZE_DOWNLOAD_SITE);
            }
            else
            {
                memcpy(settings->normalize_path,NORMALIZE_PATH,
                       MIN(strlen(NORMALIZE_PATH),MAX_PATH_LEN-1));
                ret = 0;
            }
        }
    }
    return ret;
}

int normalize_file(settings_t *settings,
                   char *input_file,
                   fn_callback_t fnptr)
{
    int ret = 1;
    char *args[2] = {(char *)0,(char *)0};

    if (settings && input_file)
    {
        args[0] = settings->normalize_path;
        args[1] = input_file;
        args[2] = NULL;

        ret = fork_execute_with_io_callback(settings->normalize_path,
                                            args,fnptr,
                                            CHILD_IO_ERR);
    }
    return ret;
}

int normalize_file_list(settings_t *settings,
                        char *input_files[],
                        fn_callback_t fnptr)
{
    int i = 0;
    int ret = 1;
    char *args[MAX_NORMALIZED_FILES] = {0};

    if (settings && input_files)
    {
        args[0] = settings->normalize_path;
        args[1] = "-b";

        for(i = 0; i < MAX_NORMALIZED_FILES-1; i++)
        {
            if (input_files[i])
            {
                args[i+2] = input_files[i];
            }
            else
            {
                break;
            }
        }

        ret = fork_execute_with_io_callback(settings->normalize_path,
                                            args,fnptr,
                                            CHILD_IO_ERR);
    }
    return ret;
}

float normalize_get_percent_complete(char *buf, int buflen)
{
    float ret = -1.0f;
    char *ptr = (char *)0;
    char *start = (char *)0;

    if (buf && buflen)
    {
        ptr = strstr(buf,"batch ");

        if (ptr)
        {
            /* scan over to the first percent digit */
            while(*ptr && !isdigit((int)*ptr))
            {
                ptr++;
            }
            start = ptr;

            while(*ptr && (*ptr != '%'))
            {
                ptr++;
            }
            *ptr = '\0';

            ret = (float)atof(start);
        }
    }
    return ret;
}
