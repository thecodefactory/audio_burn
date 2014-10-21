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

int MIN(int a, int b)
{
    return ((a < b) ? a : b);
}

int is_supported_file_type(settings_t *settings, char *filename)
{
    int ret = 0;
    int type = 0;

    if (settings)
    {
        type = get_file_type(filename);

        switch(type)
        {
            case FILE_TYPE_MP3:
                ret = ((settings->mpgdec_enabled) ? 1 : 0);
                break;
            case FILE_TYPE_OGG:
                ret = ((settings->oggdec_enabled) ? 1 : 0);
                break;
            case FILE_TYPE_WAV:
                ret = 1;
                break;
            case FILE_TYPE_M3U:
                ret = 1;
                break;
            case FILE_TYPE_PLS:
                /* NOT YET */
                ret = 0;
                break;
        }
    }
    return ret;
}

int get_file_type(char *filename)
{
    int ret = FILE_TYPE_ERROR;
    int len = 0;
    char *ptr = (char *)0;
    char *extension = (char *)0;

    if (filename)
    {
        len = strlen(filename);
        ptr = filename + (len - 1);

        while(*ptr && (ptr > filename) && (*ptr != '.'))
        {
            ptr--;
        }
        if (*ptr == '.')
        {
            ptr++;
            extension = ptr;

            if (strcasecmp(extension,"wav") == 0)
            {
                ret = FILE_TYPE_WAV;
            }
            else if (strcasecmp(extension,"mp3") == 0)
            {
                ret = FILE_TYPE_MP3;
            }
            else if (strcasecmp(extension,"ogg") == 0)
            {
                ret = FILE_TYPE_OGG;
            }
            else if (strcasecmp(extension,"m3u") == 0)
            {
                ret = FILE_TYPE_M3U;
            }
            else if (strcasecmp(extension,"pls") == 0)
            {
                ret = FILE_TYPE_PLS;
            }
            else
            {
                ret = FILE_TYPE_UNKNOWN;
            }
        }
    }
    return ret;
}

char *strip_path_to_filename(char *filename)
{
    int len = 0;
    char *ptr = (char *)0;
    char *ret = (char *)0;

    if (filename)
    {
        len = strlen(filename);
        ptr = (char *)(filename + (len - 1));

        while(*ptr && (ptr > filename) && (*ptr != '/') && (*ptr != '\\'))
        {
            ptr--;
        }
        ret = ((ptr > filename) ? ++ptr : ptr);
    }
    return ret;
}

char *get_path_to_filename(char *filename)
{
    int len = 0;
    char *ptr = NULL, *start = NULL, *end = NULL;
    char *ret = NULL;

    if (filename)
    {
        start = filename;
        len = strlen(filename);
        ptr = (char *)(filename + (len - 1));

        while(*ptr && (ptr > filename) && (*ptr != '/') && (*ptr != '\\'))
        {
            ptr--;
        }
        end = ptr;

        if (ptr != filename)
        {
            len = ((end - start) + sizeof(char));
            ret = (char *)malloc(len);
            if (ret)
            {
                memcpy(ret, start, (len - 1));
            }
        }
    }
    return ret;
}

void strip_filename_extension(char *filename)
{
    int len = 0;
    char *ptr = (char *)0;

    if (filename)
    {
        len = strlen(filename);
        ptr = (char *)(filename + (len - 1));

        while(*ptr && (ptr > filename) && (*ptr != '.'))
        {
            ptr--;
        }
        if (*ptr == '.')
        {
            *ptr = '\0';
        }
    }
}

int generate_output_filename(settings_t *settings,
                             char *filename,
                             char *buf, int maxlen)
{
    int ret = 1;
    char *newstr = (char *)0;

    if (settings && filename && buf)
    {
        newstr = replace_chars(' ','_',strip_path_to_filename(filename));

        if (newstr)
        {
            snprintf(buf,maxlen,"%s%.3d_%s.wav",
                     settings->tmpdir,
                     settings->num_output_files,
                     newstr);

            free(newstr);
            ret = 0;
        }
    }
    return ret;
}

int get_full_path_name(char *filename, char *buf, int maxlen)
{
    int ret = 1;
    int len = 0;
    char *pwd = getenv("PWD");
    char *ptr = (char *)0;
    char *end = (char *)0;

    if (filename && buf && maxlen)
    {
        len = strlen(filename);
        ptr = filename;
        end = (char *)(filename + len);

        while(ptr && *ptr && (ptr < end))
        {
            if ((*ptr == '/') || (*ptr == '\\'))
            {
                break;
            }
            ptr++;
        }

        if ((ptr == end) && pwd)
        {
            snprintf(buf,maxlen,"%s/%s",pwd,filename);
            pwd = (char *)0;
        }
        else
        {
            strncpy(buf,filename,maxlen);
        }
        ret = 0;
    }
    return ret;
}

int soft_link(char *oldpath, char *newpath)
{
    int ret = 1;

    if (oldpath && newpath)
    {
        ret = symlink(oldpath,newpath);
    }
    return ret;
}

int delete_file(char *filename)
{
    int ret = 1;

    if (filename)
    {
        ret = unlink(filename);
    }
    return ret;
}

int get_song_length(char *filename)
{
    int ret = -1;
    int file_type = 0;

    if (filename)
    {
        file_type = get_file_type(filename);
        switch(file_type)
        {
            case FILE_TYPE_WAV:
                ret = wav_get_song_length(filename);
                break;
            case FILE_TYPE_OGG:
                ret = ogg_get_song_length(filename);
                break;
            case FILE_TYPE_MP3:
                ret = mpg_get_song_length(filename);
                break;
            default:
                break;
        }
    }
    return ret;
}

char *replace_chars(char a, char b, char *str)
{
    char *ret = (char *)0;
    char *ptr = (char *)0;
    char *end = (char *)0;
    int len = 0;

    if (str)
    {
        len = strlen(str);
        ret = strdup(str);

        ptr = ret;
        end = (char *)(ret + len);

        while(ptr && *ptr && (ptr < end))
        {
            if (*ptr == a)
            {
                *ptr = b;
            }
            ptr++;
        }
    }
    return ret;
}

int check_filelist_with_cd_length(int cd_length_in_minutes,
                                  char **filelist,
                                  int num_files,
                                  int verbose)
{
    int i = 0;
    int ret = 1;
    int current_length = 0;
    int total_length = 0;

    if (filelist && num_files)
    {
        for(i = 0; i < num_files; i++)
        {
            current_length = get_song_length(filelist[i]);
            if (current_length != -1)
            {
                total_length += current_length;
            }
        }

        if (verbose)
        {
            printf("Total Computed Target CD Length is %.2d:%.2d\n",
                   (int)(total_length / 60),
                   (int)(total_length % 60));
        }

        if (total_length < (cd_length_in_minutes * 60))
        {
            ret = 0;
        }
    }
    return ret;
}

/*
  this internal helper function parses an m3u playlist buffer
  loosely based on information found online at:
  http://hanna.pyxidis.org/tech/m3u.html
*/
#define ADVANCE_PAST_LINEBREAK()           \
do {                                       \
    while((*ptr == '\r') || (*ptr == '\n'))\
    {                                      \
        ptr++;                             \
    }                                      \
} while(0)

static char **parse_m3u_playlist_buffer(char *buf, size_t buf_len,
                                        int *num_tracks,
                                        char *path_prefix,
                                        int verbose)
{
    int i = 0, current_index = 0, len = 0, path_prefix_len = 0;
    char **file_list = NULL;
    char *ptr = NULL, *start = NULL, *end = NULL;
    char *line_start = NULL, *line_end = NULL;
    static char *header = "#EXTM3U";
    static char *line_lead = "#EXTINF:";

    if (buf && num_tracks)
    {
        if (path_prefix)
        {
            /* the sizeof(char) is for the later appended trailing '/' */
            path_prefix_len = (strlen(path_prefix) + sizeof(char));
        }

        file_list = (char **)malloc(MAX_NUM_INPUT_FILES * sizeof(char *));
        for(i = 0; i < MAX_NUM_INPUT_FILES; i++)
        {
            file_list[i] = NULL;
        }

        start = buf;
        ptr = buf;
        end = (char *)(buf + buf_len);

        /* skip past the header */
        ptr = strstr(buf, header);
        if (!ptr)
        {
            /*
              NOTE: some m3u playlists do not have the header
              present.  if not found, try skipping it.
            */
            ptr = buf;
            goto skip_header;
        }
        ptr += strlen(header);
        ADVANCE_PAST_LINEBREAK();

      skip_header:
        while(ptr && (ptr <= end) && (*ptr != '\0'))
        {
            /*
              check if the line starts with the right information.

              NOTE: some m3u playlists do not alternate lines
              properly with the line lead, so skip it if it's
              not present
            */
            if (strncmp(ptr, line_lead, strlen(line_lead)) != 0)
            {
                goto skip_line_lead;
            }

            while((*ptr != '\r') && (*ptr != '\n'))
            {
                ptr++;
            }

          skip_line_lead:
            ADVANCE_PAST_LINEBREAK();

            line_start = ptr;
            /*
              NOTE: need to check for NULL here since the last
              line may not have a line break at the end
            */
            while((*ptr != '\0') && (*ptr != '\r') && (*ptr != '\n'))
            {
                ptr++;
            }
            line_end = ptr;
            ADVANCE_PAST_LINEBREAK();

            /* add this filename to our file_list */
            len = ((line_end - line_start)  + sizeof(char));

            /*
              if the path of the playlist was specified, we should
              ONLY append it to the filenames in the playlist if
              they are NOT absolute (i.e. starting with a '/')
            */
            if (path_prefix &&
                (*line_start != '/') && (*line_start != '\\'))
            {
                file_list[current_index] = (char *)malloc(
                    (path_prefix_len + len));
                if (!file_list)
                {
                    goto error_exit;
                }
                memset(file_list[current_index], 0,
                       (path_prefix_len + len));
                sprintf(file_list[current_index],"%s/",path_prefix);
                memcpy((char *)(file_list[current_index] +
                                path_prefix_len),
                       line_start, len - 1);
            }
            else
            {
                file_list[current_index] = (char *)malloc(len);
                if (!file_list)
                {
                    goto error_exit;
                }
                memset(file_list[current_index], 0, len);
                memcpy(file_list[current_index], line_start, len - 1);
            }

            if (verbose)
            {
                if (current_index == 0)
                {
                    printf("Adding the following m3u playlist tracks:\n");
                }
                printf(" -- %s\n", file_list[current_index]);
            }

            if ((current_index + 1) >= MAX_NUM_INPUT_FILES)
            {
                break;
            }
            current_index++;
        }

        *num_tracks = current_index;

        if (verbose)
        {
            printf("\n");
        }
    }

    return file_list;

  error_exit:
    for(i = 0; i < MAX_NUM_INPUT_FILES; i++)
    {
        if (file_list[i])
        {
            free(file_list[i]);
        }
    }
    free(file_list);

    return NULL;
}
#undef ADVANCE_PAST_LINEBREAK

char **explode_m3u_playlist_file(char *filename,
                                 int *num_tracks,
                                 int verbose)
{
    int i = 0;
    FILE *file = NULL;
    char *ptr = NULL, *path_prefix = NULL;
    char **file_list = NULL;
    char *playlist_buf = NULL;
    struct stat statbuf;

    if (filename && num_tracks)
    {
        memset(&statbuf, 0, sizeof(struct stat));

        if ((stat(filename, &statbuf) == 0) &&
            (file = fopen(filename, "rb")))
        {
            playlist_buf = (char *)malloc(statbuf.st_size + sizeof(char));
            if (playlist_buf)
            {
                if (fread(playlist_buf, 1, statbuf.st_size, file) ==
                    statbuf.st_size)
                {
                    playlist_buf[statbuf.st_size - 1] = '\0';
                    fclose(file);

                    path_prefix = get_path_to_filename(filename);

                    file_list = parse_m3u_playlist_buffer(
                        playlist_buf, statbuf.st_size,
                        num_tracks, path_prefix, verbose);

                    if (path_prefix)
                    {
                        free(path_prefix);
                    }
                    free(playlist_buf);
                }
            }
        }
    }
    return file_list;
}
