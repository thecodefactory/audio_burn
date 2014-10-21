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

/*
  strictly speaking, we advance to either a digit, or a '-' sign for
  handling negative values
*/
#define ADVANCE_TO_DIGIT(ptr, end)                \
do {                                              \
    while(ptr && (ptr < end) && (*ptr != '-') &&  \
          (!isdigit((int)*ptr)))                  \
        ptr++;                                    \
} while(0)

#define ADVANCE_TO_CRLF(ptr, end)                 \
do {                                              \
    while(ptr && (ptr < end) && (*ptr != '\r') && \
          (*ptr != '\n'))                         \
        ptr++;                                    \
} while(0)

#define ADVANCE_TO_SLASH_OR_ALPHA(ptr, end)       \
do {                                              \
    while(ptr && (ptr < end) && (*ptr != '/') &&  \
          (*ptr != '\\') && (!isalpha((int)*ptr)))\
        ptr++;                                    \
} while(0)

#define PTR_IS_VALID(ptr, end) \
(ptr && (ptr < end) && (*ptr != '\0'))

#define CHECK_NUM(ptr, end, str, len, arg)   \
do {                                         \
    if (strncasecmp(buf,str,len) == 0)       \
    {                                        \
        ptr = buf + len;                     \
        ADVANCE_TO_DIGIT(ptr, end);          \
        if (PTR_IS_VALID(ptr, end)) {        \
            settings->arg = atoi(ptr);       \
        }                                    \
        continue;                            \
    }                                        \
} while(0)

#define MPL MAX_PATH_LEN
#define CHECK_STR(ptr, end, str, len, arg)  \
do {                                        \
    if (strncasecmp(buf,str,len) == 0)      \
    {                                       \
        ptr = buf + len;                    \
        ADVANCE_TO_SLASH_OR_ALPHA(ptr, end);\
        if (PTR_IS_VALID(ptr, end)) {       \
            strncpy(settings->arg,ptr,MPL); \
            { char *sptr = settings->arg;   \
              char *eptr =                  \ 
                 (char*)(settings->arg+MPL);\
            ADVANCE_TO_CRLF(sptr, eptr);    \
            if (sptr) *sptr ='\0';          \
            }                               \
        }                                   \
        continue;                           \
    }                                       \
} while(0)

int settings_initialize(settings_t *settings, char *filename)
{
    int ret = 1;

    if (settings)
    {
        memset(settings,0,sizeof(settings_t));
        memset(settings->atapi_device,0,MAX_PATH_LEN);
        settings->speed = -1;
        settings->clean_tmp_files = 1;
        settings->target_disc_len_in_minutes =
            DEFAULT_AUDIO_CD_LEN_IN_MINUTES;

        if (filename)
        {
            strncpy(settings->config_file, filename, MAX_PATH_LEN);
            ret = settings_read_config_file(settings, filename);
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}

void settings_uninitialize(settings_t *settings)
{
    int i = 0;

    if (settings)
    {
        if (settings->config_file && settings->save_settings)
        {
            settings_write_config_file(settings, settings->config_file);
        }

        for(i = 0; i < settings->num_input_files; i++)
        {
            if (settings->input_files[i])
            {
                free(settings->input_files[i]);
            }
        }
        for(i = 0; i < settings->num_output_files; i++)
        {
            if (settings->output_files[i])
            {
                free(settings->output_files[i]);
            }
        }
        memset(settings,0,sizeof(settings_t));
    }
}

int settings_read_config_file(settings_t *settings, char *filename)
{
    int ret = 1;
    FILE *fd = NULL;
    char buf[512] = {0};
    char *ptr = NULL;
    char *end = (char *)(buf + 512);

    if (settings && filename)
    {
        if (fd = fopen(filename, "a+"))
        {
            fseek(fd, 0L, SEEK_SET);

            while(fgets(buf, 512, fd))
            {
                if (buf[0] == '#')
                {
                    continue;
                }
                CHECK_NUM(ptr, end, "dao", 3, dao);
                CHECK_NUM(ptr, end, "eject", 5, eject);
                CHECK_NUM(ptr, end, "speed", 5, speed);
                CHECK_NUM(ptr, end, "verbose", 7, verbose);
                CHECK_NUM(ptr, end, "simulate", 8, dummy_run);
                CHECK_NUM(ptr, end, "gracetime", 9, gracetime);
                CHECK_NUM(ptr, end, "burnfree",8, burn_free);
                CHECK_NUM(ptr, end, "atapi", 5, atapi);
                CHECK_STR(ptr, end, "atapidev", 8, atapi_device);
                CHECK_NUM(ptr, end, "cleantmp", 8, clean_tmp_files);
                CHECK_NUM(ptr, end, "preferred_device", 16,
                          preferred_device_number);
                CHECK_NUM(ptr, end, "target_disc_mins", 16,
                          target_disc_len_in_minutes);
                CHECK_STR(ptr, end, "tmpdir", 6, tmpdir);

                memset(buf, 0, 512);
            }

            fclose(fd);
            ret = change_file_permissions(filename);
            if (ret)
            {
                fprintf(stderr, "Cannot change permissions on %s\n",
                        filename);
                fprintf(stderr, "Please make sure you have read and "
                        "write access to this file\n");
            }
        }
        else
        {
            fprintf(stderr, "Cannot open or create file %s\n", filename);
            fprintf(stderr, "Please make sure you have read and "
                    "write access to this file\n");
        }
    }
    return ret;
}

void settings_write_config_file(settings_t *settings, char *filename)
{
    FILE *fd = 0;
    char line[MAX_BUF_LEN] = {0};
    static char *header =
        "# Generated by libaudioburn\r\n"
        "#  (C) 2004 Neill Miller\r\n"
        "#  Contact author: neillm@thecodefactory.org\r\n#\r\n"
        "# Write out all user configurable settings.\r\n#\r\n\r\n";

    if (!filename || (fd = fopen(filename,"w")) == 0)
    {
        return;
    }
    if (fwrite(header,sizeof(char),strlen(header),fd) == 0)
    {
        fclose(fd);
        return;
    }
    snprintf(line,MAX_BUF_LEN,
             "DAO = %d\r\n",settings->dao);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "EJECT = %d\r\n",settings->eject);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "SPEED = %d\r\n",settings->speed);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "VERBOSE = %d\r\n",settings->verbose);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "SIMULATE = %d\r\n",settings->dummy_run);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "GRACETIME = %d\r\n",settings->gracetime);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "BURNFREE = %d\r\n",settings->burn_free);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "ATAPI = %d\r\n",settings->atapi);
    fwrite(line,sizeof(char),strlen(line),fd);

    if (strlen(settings->atapi_device))
    {
        snprintf(line,MAX_BUF_LEN,
                 "#\r\n# NOTE: Do not have \"ATAPI = 1\" and use "
                 "the \"ATAPIDEV = /dev/X\" at\r\n# the same "
                 "time! These two options CANNOT be used at the "
                 "same time!\r\n#\r\n");
        fwrite(line,sizeof(char),strlen(line),fd);

        snprintf(line,MAX_BUF_LEN,
                 "ATAPIDEV = %s\r\n",settings->atapi_device);
        fwrite(line,sizeof(char),strlen(line),fd);
    }

    snprintf(line,MAX_BUF_LEN,
             "CLEANTMP = %d\r\n",settings->clean_tmp_files);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "PREFERRED_DEVICE = %d\r\n",
             settings->preferred_device_number);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "TARGET_DISC_MINS = %d\r\n",
             settings->target_disc_len_in_minutes);
    fwrite(line,sizeof(char),strlen(line),fd);

    snprintf(line,MAX_BUF_LEN,
             "TMPDIR = %s\r\n",settings->tmpdir);
    fwrite(line,sizeof(char),strlen(line),fd);

    fclose(fd);

    if (settings->verbose)
    {
        fprintf(stderr,"Wrote Config options to %s\n", filename);
    }
}

int settings_enable_utilities(settings_t *settings, int flags)
{
    int ret = 1;

    if (flags && settings)
    {
        settings->sox_enabled = ((flags & ENABLE_SOX) ? 1 : 0);
        settings->oggdec_enabled = ((flags & ENABLE_OGGDEC) ? 1 : 0);
        settings->mpgdec_enabled = ((flags & ENABLE_MPGDEC) ? 1 : 0);
        settings->cdrecord_enabled =
            ((flags & ENABLE_CDRECORD) ? 1 : 0);
        settings->normalize_enabled =
            ((flags & ENABLE_NORMALIZE) ? 1 : 0);
        ret = 0;
    }
    return ret;
}

int settings_add_input_file(settings_t *settings, char *input_file)
{
    int ret = 1;
    struct stat statbuf;

    if (settings && input_file)
    {
        if (settings->num_input_files < MAX_NUM_INPUT_FILES)
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(input_file,&statbuf) == 0)
            {
                settings->input_files[settings->num_input_files++] =
                    strdup(input_file);

                ret = 0;
            }
            else
            {
                fprintf(stderr,"ERROR -- Cannot access file:\n%s.\n"
                        "Please make sure the path is correct.\n",
                        input_file);
            }
        }
    }
    return ret;
}

void settings_remove_input_file(settings_t *settings,
                                char *input_file)
{
    int i = 0;

    if (settings && input_file)
    {
        for(i = 0; i < settings->num_input_files; i++)
        {
            /*
              this can come back to bite me since we're allowing
              non-exact input_file name matches here, but for
              now I'm hoping it's unlikely ;-)
            */
            if ((strcmp(settings->input_files[i],input_file) == 0) ||
                (strstr(settings->input_files[i],input_file) != NULL))
            {
                free(settings->input_files[i]);

                /* shift any tracks after this point down, if necessary */
                if (settings->num_input_files > 1)
                {
                    for(; i < settings->num_input_files - 1; i++)
                    {
                        settings->input_files[i] =
                            settings->input_files[i+1];
                    }
                }
                else
                {
                    settings->input_files[0] = NULL;
                }
                settings->num_input_files--;
                break;
            }
        }
    }
}

int settings_add_output_file(settings_t *settings,
                             char *output_file)
{
    int ret = 1;
    struct stat statbuf;

    if (settings && output_file)
    {
        if (settings->num_output_files < MAX_NUM_OUTPUT_FILES)
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(output_file,&statbuf) == 0)
            {
                settings->output_files[settings->num_output_files++] =
                    strdup(output_file);

                ret = 0;
            }
        }
    }
    return ret;
}

void settings_remove_output_file(settings_t *settings,
                                 char *output_file)
{
    int i = 0;

    if (settings && output_file)
    {
        for(i = 0; i < settings->num_output_files; i++)
        {
            /*
              this can come back to bite me since we're allowing
              non-exact output_file name matches here, but for
              now I'm hoping it's unlikely ;-)
            */
            if ((strcmp(settings->output_files[i],output_file) == 0) ||
                (strstr(settings->output_files[i],output_file) != NULL))
            {
                free(settings->output_files[i]);

                /* shift any tracks after this point down */
                for(; i < settings->num_output_files - 1; i++)
                {
                    settings->output_files[i] =
                        settings->output_files[i+1];
                }
                settings->num_output_files--;
                break;
            }
        }
    }
}

void settings_set_preferred_device_number(settings_t *settings,
                                          int device_num)
{
    if (settings)
    {
        settings->preferred_device_number = device_num;
    }
}

void settings_set_temporary_directory(settings_t *settings,
                                      char *tmpdir)
{
    int len = 0;

    if (settings && tmpdir)
    {
        memset(settings->tmpdir,0,MAX_PATH_LEN);

        len = strlen(tmpdir);
        memcpy(settings->tmpdir,tmpdir,MIN(len,MAX_PATH_LEN-1));

        /* make sure the tmpdir ends with a '/' */
        if (tmpdir[len-1] != '/')
        {
            strcat(settings->tmpdir,"/");
        }
    }
}

void settings_set_verbose_output(settings_t *settings)
{
    if (settings)
    {
        settings->verbose = 1;
    }
}

void settings_set_dummy_run(settings_t *settings)
{
    if (settings)
    {
        settings->dummy_run = 1;
    }
}

void settings_set_gracetime(settings_t *settings, int gracetime)
{
    if (settings)
    {
        settings->gracetime = gracetime;

        /* cdrecord will not accept values smaller than 2 seconds */
        if (settings->gracetime < 2)
        {
            settings->gracetime = 2;
        }
    }
}

void settings_set_burnfree(settings_t *settings)
{
    if (settings)
    {
        settings->burn_free = 1;
    }
}

void settings_set_atapi(settings_t *settings, int val)
{
    if (settings)
    {
        settings->atapi = val;
    }
}

void settings_set_atapi_dev(settings_t *settings, char *device)
{
    int len = 0;

    if (settings && device)
    {
        memset(settings->atapi_device,0,MAX_PATH_LEN);

        len = strlen(device);
        memcpy(settings->atapi_device,device,MIN(len,MAX_PATH_LEN-1));
        fprintf(stderr,"SET ATAPI DEV TO %s\n",settings->atapi_device);
    }
}

void settings_clean_tmp_files(settings_t *settings,
                              int clean_tmp_files)
{
    if (settings)
    {
        settings->clean_tmp_files = clean_tmp_files;
    }
}

void settings_set_device_speed(settings_t *settings, int speed)
{
    if (settings)
    {
        settings->speed = speed;
    }
}

void settings_set_session_at_once(settings_t *settings)
{
    if (settings)
    {
        settings->dao = 1;
    }
}

void settings_set_device_eject(settings_t *settings)
{
    if (settings)
    {
        settings->eject = 1;
    }
}

void settings_set_disc_length_in_minutes(settings_t *settings,
                                         int len_in_minutes)
{
    if (settings)
    {
        settings->target_disc_len_in_minutes = len_in_minutes;
    }
}

int settings_add_m3u_playlist_file(settings_t *settings, char *filename)
{
    int ret = 1, i = 0;
    int original_num_input_files = 0;
    int new_input_files = 0;
    char **file_list = NULL;

    if (settings && filename)
    {
        original_num_input_files = settings->num_input_files;

        file_list = explode_m3u_playlist_file(
            filename, &new_input_files, settings->verbose);
        if (file_list && new_input_files)
        {
            if ((new_input_files + (original_num_input_files - 1)) >
                (MAX_NUM_INPUT_FILES - 1))
            {
                ret = -1;
                goto error_exit;
            }

            settings_remove_input_file(settings, filename);

            for(i = 0; i < new_input_files; i++)
            {
                if (i > (MAX_NUM_INPUT_FILES - 1))
                {
                    break;
                }

                ret = settings_add_input_file(settings, file_list[i]);
                if (ret)
                {
                    ret = -2;
                    goto error_exit;
                }
            }

            ret = 0;
        }
    }

  error_exit:
    if (file_list)
    {
        for(i = 0; i < new_input_files; i++)
        {
            free(file_list[i]);
            file_list[i] = NULL;
        }
        free(file_list);
        file_list = NULL;
    }

    return ret;
}
