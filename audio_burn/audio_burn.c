/*
  audio_burn - a simple audio burning application using libaudioburn
  Copyright (C) 2003, 2004 Neill Miller

  audio_burn is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  audio_burn is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with audio_burn; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include "libaudioburn.h"


#define ASCII_ESCAPE_KEY (char)0x1B
#define ASCII_RETURN_KEY (char)0x0A

#define AUDIO_BURN_USER_ABORTED    -1
#define AUDIO_BURN_SUCCESS          0
#define AUDIO_BURN_ERROR            1

#define SHOW_BANNER()                                                \
do {                                                                 \
    printf("audio_burn %s Copyright 2003, 2004 (C) Neill Miller\n\n",\
           VERSION);                                                 \
} while(0)

static int s_num_copies_remaining = 1;

void oggdec_callback(pid_t child_pid, char *buf, int buflen)
{
    float percent_complete = 0.0f;
    static int first_time = 1;

    if (buf && buflen)
    {
        percent_complete = ogg_get_percent_complete(buf,buflen);
        if (percent_complete != -1.0f)
        {
            if (first_time)
            {
                first_time = 0;
            }
            else
            {
                fprintf(stderr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
                        "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            }
            fprintf(stderr,"OGG Decoding in progress: ");
            if (percent_complete < 10.0f)
            {
                fprintf(stderr,"  %.2f%%",percent_complete);
            }
            else if (percent_complete < 100.0f)
            {
                fprintf(stderr," %.2f%%",percent_complete);
            }
            else
            {
                fprintf(stderr,"%.2f%%",percent_complete);
            }
        }
    }
    else
    {
        printf("\nOgg Decoding complete\n");
        first_time = 1;
    }
}

void mpgdec_callback(pid_t child_pid, char *buf, int buflen)
{
    float percent_complete = 0.0f;
    static int first_time = 1;

    if (buf && buflen)
    {
        percent_complete = mpg_get_percent_complete(buf,buflen);
        if (percent_complete != -1.0f)
        {
            if (first_time)
            {
                first_time = 0;
            }
            else
            {
                fprintf(stderr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
                        "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            }
            fprintf(stderr,"MP3 Decoding in progress: ");
            if (percent_complete < 10.0f)
            {
                fprintf(stderr,"  %.2f%%",percent_complete);
            }
            else if (percent_complete < 100.0f)
            {
                fprintf(stderr," %.2f%%",percent_complete);
            }
            else
            {
                fprintf(stderr,"%.2f%%",percent_complete);
            }
        }
    }
    else
    {
        printf("\nMP3 Decoding complete\n");
        first_time = 1;
    }
}

void normalize_callback(pid_t child_pid, char *buf, int buflen)
{
    float percent_complete = 0.0f;
    static int first_time = 1;

    if (buf && buflen)
    {
        percent_complete = normalize_get_percent_complete(buf,buflen);
        if (percent_complete != -1.0f)
        {
            if (first_time)
            {
                first_time = 0;
            }
            else
            {
                fprintf(stderr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
                        "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            }
            fprintf(stderr,"Normalization in progress: ");
            if (percent_complete < 10.0f)
            {
                fprintf(stderr,"  %.2f%%",percent_complete);
            }
            else if (percent_complete < 100.0f)
            {
                fprintf(stderr," %.2f%%",percent_complete);
            }
            else
            {
                fprintf(stderr,"%.2f%%",percent_complete);
            }
        }
    }
    else
    {
        printf("\nNormalization complete\n");
        first_time = 1;
    }
}

void sox_resample_callback(pid_t child_pid, char *buf, int buflen)
{
    if (buf && buflen)
    {
        fprintf(stderr,"Resampling wav file...");
        while(child_still_running(child_pid))
        {
            fprintf(stderr,".");
            sleep(2);
        }
        fprintf(stderr,"done.\n");
    }
}

void cdrecord_callback(pid_t child_pid, char *buf, int buflen)
{
    char *ptr = NULL, *end = NULL;

    if (buf && buflen)
    {
        end = (char *)(buf + buflen);
        for(ptr = buf; ptr != end; ptr++)
        {
            if (iscntrl((int)*ptr) || !isprint((int)*ptr))
            {
                *ptr = '\n';
            }
        }
        fprintf(stderr,"%s",buf);
    }
}

void print_help()
{
    SHOW_BANNER();

    printf("Usage: audio_burn [OPTIONS] file1 file2 file3 ...\n\n");
    printf("Available OPTIONS are:\n");
    printf("-h, --help        : prints this help message and exits\n");
    printf("-V, --version     : prints the version and exits\n");
    printf("-v, --verbose     : "
           "prints out verbose output during run-time\n");
    printf("                  : "
           "NOTE: required for -v output to cdrecord\n");
    printf("-d, --device=N    : "
           "use specified device number for cd burning\n");
    printf("-t, --tmpdir=X    : "
           "use specified tmpdir for processing files\n");
    printf("-s, --simulate    : "
           "simulates the burning (data is not written)\n");
    printf("-g, --gracetime=S : "
           "set the time in sec. before starting to write\n");
    printf("-D, --dao         : use session at once mode (DAO)\n");
    printf("-e, --eject       : eject the cd upon completion\n");
    printf("-C, --copies=M    : "
           "will burn M copies of the same cd in a row\n");
    printf("--speed=N         : sets the preferred burning speed\n");
    printf("--burnfree        : enables burnfree mode in cdrecord\n");
    printf("--atapi           : uses ATAPI device support in cdrecord\n");
    printf("--atapi-dev=/dev/X: uses specified ATAPI device for "
           "cdrecord rather than probing\n");
    printf("--noresample      : "
           "disables use of sox for wav resampling\n");
    printf("--nonormalize     : disables use of normalize\n");
    printf("--nooggdec        : "
           "disables use of oggdec for ogg decoding\n");
    printf("--nompgdec        : "
           "disables use of mpg321 for mp3 decoding\n");
    printf("--norecord        : "
           "disables use of cdrecord for cd burning\n");
    printf("--nocleantmp      : disables removal of temporary files\n");
    printf("--saveconfig      : "
           "saves options to ~/.audioburnrc file on exit\n");
    printf("--cdlength=M      : "
           "specifies target cd length of M minutes\n");
    printf("\nIt may be useful to disable programs that you do not "
           "have installed\n");
}

void parse_command_line_args(int argc, char **argv,
                             settings_t *settings)
{
    int val = 0;
    int banner_shown = 0;
    int option_index = 0;
    char *cur_long_option = (char *)0;

    static struct option long_options[] =
    {
        {"help",0,0,0},
        {"version",0,0,0},
        {"verbose",0,0,0},
        {"device",1,0,0},
        {"tmpdir",1,0,0},
        {"simulate",0,0,0},
        {"gracetime",1,0,0},
        {"speed",1,0,0},
        {"copies",1,0,0},
        {"dao",0,0,0},
        {"eject",0,0,0},
        {"burnfree",0,0,0},
        {"atapi",0,0,0},
        {"atapi-dev",1,0,0},
        {"noresample",0,0,0},
        {"nonormalize",0,0,0},
        {"nooggdec",0,0,0},
        {"nompgdec",0,0,0},
        {"norecord",0,0,0},
        {"nocleantmp",0,0,0},
        {"saveconfig",0,0,0},
        {"cdlength",1,0,0},
        {0,0,0,0}
    };

    if (!settings)
    {
        fprintf(stderr,"Invalid settings object passed in.\n");
        exit(1);
    }

    if (argc < 2)
    {
        print_help();
        exit(1);
    }

    while ((val = getopt_long(argc,argv,"-vsDeC:Vhd:t:g:",
                              long_options,&option_index)) != -1)
    {
        switch (val)
        {
            case 0:
                cur_long_option =
                    (char *)long_options[option_index].name;

                if (strcmp(cur_long_option,"help") == 0)
                {
                    goto opt_help;
                }
                else if (strcmp(cur_long_option,"version") == 0)
                {
                    goto opt_version;
                }
                else if (strcmp(cur_long_option,"verbose") == 0)
                {
                    goto opt_verbose;
                }
                else if (strcmp(cur_long_option,"device") == 0)
                {
                    goto opt_device_number;
                }
                else if (strcmp(cur_long_option,"tmpdir") == 0)
                {
                    goto opt_tmp_dir;
                }
                else if (strcmp(cur_long_option,"simulate") == 0)
                {
                    goto opt_simulate;
                }
                else if (strcmp(cur_long_option,"gracetime") == 0)
                {
                    goto opt_gracetime;
                }
                else if (strcmp(cur_long_option,"speed") == 0)
                {
                    settings_set_device_speed(settings,atoi(optarg));
                }
                else if (strcmp(cur_long_option,"copies") == 0)
                {
                    goto opt_copies;
                }
                else if (strcmp(cur_long_option,"dao") == 0)
                {
                    goto opt_session_at_once;
                }
                else if (strcmp(cur_long_option,"eject") == 0)
                {
                    goto opt_eject;
                }
                else if (strcmp(cur_long_option,"burnfree") == 0)
                {
                    settings_set_burnfree(settings);
                }
                else if (strcmp(cur_long_option,"atapi") == 0)
                {
                    settings_set_atapi(settings, 1);
                }
                else if (strcmp(cur_long_option,"atapi-dev") == 0)
                {
                    settings_set_atapi(settings, 0);
                    settings_set_atapi_dev(settings, optarg);
                }
                else if (strcmp(cur_long_option,"noresample") == 0)
                {
                    settings->sox_enabled = 0;
                }
                else if (strcmp(cur_long_option,"nonormalize") == 0)
                {
                    settings->normalize_enabled = 0;
                }
                else if (strcmp(cur_long_option,"nooggdec") == 0)
                {
                    settings->oggdec_enabled = 0;
                }
                else if (strcmp(cur_long_option,"nompgdec") == 0)
                {
                    settings->mpgdec_enabled = 0;
                }
                else if (strcmp(cur_long_option,"norecord") == 0)
                {
                    settings->cdrecord_enabled = 0;
                }
                else if (strcmp(cur_long_option,"nocleantmp") == 0)
                {
                    settings_clean_tmp_files(settings,0);
                }
                else if (strcmp(cur_long_option,"saveconfig") == 0)
                {
                    settings->save_settings = 1;
                }
                else if (strcmp(cur_long_option,"cdlength") == 0)
                {
                    settings_set_disc_length_in_minutes(
                        settings,atoi(optarg));
                }
                else
                {
                    printf("Invalid option (try --help for "
                           "more information)\n");
                    exit(1);
                }
                break;
            case 'v':
          opt_verbose:
                settings_set_verbose_output(settings);
                break;
            case 'V':
          opt_version:
                printf("%s\n",VERSION);
                exit(0);
            case 'h':
          opt_help:
                print_help();
                exit(0);
            case 'd':
          opt_device_number:
                settings_set_preferred_device_number(
                    settings,atoi(optarg));
                break;
            case 't':
          opt_tmp_dir:
                settings_set_temporary_directory(settings,optarg);
                break;
            case 's':
          opt_simulate:
                settings_set_dummy_run(settings);
                break;
            case 'g':
          opt_gracetime:
                settings_set_gracetime(settings,atoi(optarg));
                break;
            case 'D':
          opt_session_at_once:
                settings_set_session_at_once(settings);
                break;
            case 'e':
          opt_eject:
                settings_set_device_eject(settings);
                break;
            case 'C':
          opt_copies:
                s_num_copies_remaining = atoi(optarg);
                if (s_num_copies_remaining > 1)
                {
                    /* multiple copies implies eject */
                    settings_set_device_eject(settings);

                    /* and a gracetime of 2 seconds */
                    settings_set_gracetime(settings, 2);
                }
                break;
            case 1:
                if (!banner_shown)
                {
                    SHOW_BANNER();
                    banner_shown = 1;
                }

                if (!is_supported_file_type(settings,optarg))
                {
                    if (settings->verbose)
                    {
                        printf("skipping file : %s\n\t(unsupported file "
                               "type; please check extension)\n",optarg);
                    }
                    continue;
                }
                if (settings_add_input_file(settings,optarg))
                {
                    if (settings->verbose)
                    {
                        printf("skipping file : %s (invalid file)\n",
                               optarg);
                    }
                }
                else
                {
                    if (settings->verbose)
                    {
                        printf("added file    : %s\n",optarg);
                    }
                }
                break;
            default:
                printf("Invalid option (try --help for "
                       "more information)\n");
                exit(1);
        }
    }

    if (!banner_shown)
    {
        SHOW_BANNER();
    }
}

int resample_wav_file(settings_t *settings,
                      char *input_file,
                      char *output_file)
{
    int ret = 1;
    int num_bits = 0;
    int num_channels = 0;
    int sample_rate = 0;

    if (!settings || !input_file || !output_file)
    {
        return ret;
    }

    if (settings->sox_enabled &&
        wav_verify_format(input_file))
    {
        if (wav_get_file_information(input_file,
                                     &num_channels,
                                     &num_bits,
                                     &sample_rate) == 0)
        {
            if (settings->verbose)
            {
                printf("wav file is %s with rate %d and "
                       "size %d\n",((num_channels == 2)
                                    ? "stereo" : "mono"),
                       sample_rate, num_bits);
                printf("Need to resample to stereo with rate "
                       "%d and size %d\n", 44100, 16);
            }
        }

        ret = sox_resample_wav(settings,input_file,output_file,
                               sox_resample_callback);
    }
    else
    {
        /*
          just soft link the input file as the
          expected output file name
        */
        ret = soft_link(input_file,output_file);
        if (ret)
        {
            fprintf(stderr,"Failed to create a symlink "
                    "from %s to %s\n",input_file,
                    output_file);
            return ret;
        }
    }
    return ret;
}

int process_input_files(settings_t *settings)
{
    int i = 0;
    int ret = 1;
    int file_type = 0;
    char *current_file = (char *)0;
    char tmpbuf[MAX_BUF_LEN] = {0};
    char output_file[MAX_BUF_LEN] = {0};
    char resampled_wav_file[MAX_BUF_LEN] = {0};
    char input_file_full_path_name[MAX_BUF_LEN] = {0};

    if (settings)
    {
        for(i = 0; i < settings->num_input_files; i++)
        {
            get_full_path_name(settings->input_files[i],
                               input_file_full_path_name,MAX_BUF_LEN);
            current_file = input_file_full_path_name;

            if (settings->verbose)
            {
                printf("Processing file: %s\n",current_file);
            }

            strncpy(tmpbuf,current_file,MAX_BUF_LEN);
            strip_filename_extension(tmpbuf);
            generate_output_filename(settings,tmpbuf,
                                     output_file,MAX_BUF_LEN);

            file_type = get_file_type(current_file);
            switch(file_type)
            {
                case FILE_TYPE_OGG:
                    if (settings->oggdec_enabled &&
                        ogg_decode(settings,current_file,output_file,
                                   oggdec_callback))
                    {
                        fprintf(stderr,"Failed to decode ogg file.\n");
                        return ret;
                    }
                    break;
                case FILE_TYPE_MP3:
                    if (settings->mpgdec_enabled &&
                        mpg_decode(settings,current_file,output_file,
                                   mpgdec_callback))
                    {
                        fprintf(stderr,"Failed to decode mp3 file.\n");
                        return ret;
                    }
                    break;
                case FILE_TYPE_WAV:
                    if (resample_wav_file(settings,current_file,
                                          output_file))
                    {
                        fprintf(stderr,"Failed to resample wav file.\n");
                        return ret;
                    }
                    break;
            }

            /* finally, verify the generated output file */
            if (wav_verify_format(output_file))
            {
                /*
                  if we have sox enabled and we've just generated
                  this wav file, we should try to resample it here
                  to make it a valid format
                */
                if ((file_type != FILE_TYPE_WAV) &&
                    (settings->sox_enabled))
                {
                    strncpy(tmpbuf,current_file,MAX_BUF_LEN);
                    strip_filename_extension(tmpbuf);
                    strncat(tmpbuf,"-resampled",MAX_BUF_LEN-strlen(tmpbuf));
                    generate_output_filename(settings,tmpbuf,
                                             resampled_wav_file,
                                             MAX_BUF_LEN);

                    if (resample_wav_file(settings,output_file,
                                          resampled_wav_file))
                    {
                        fprintf(stderr,"Failed to resample wav file.\n");
                    }
                    else
                    {
                        /* delete the original generated wav file */
                        delete_file(output_file);
                        strncpy(output_file,resampled_wav_file,MAX_BUF_LEN);
                        goto valid_input_wav;
                    }
                }
                fprintf(stderr,"Processed output file %s is "
                        "invalid!  Skipping.\n",output_file);
                continue;
            }
            else
            {
              valid_input_wav:
                /*
                  if all went well up to this point, store the
                  output filename in the settings object
                */
                if (settings_add_output_file(settings,output_file))
                {
                    fprintf(stderr,"Failed to add valid output file "
                            "%s.  Skipping.\n",output_file);
                    continue;
                }
            }
        }
        ret = (settings->num_output_files ? 0 : 1);
    }
    return ret;
}

int normalize_output_files(settings_t *settings)
{
    int ret = 1;

    if (settings && settings->num_output_files)
    {
        if (settings->verbose)
        {
            printf("Preparing to normalize %d %s\n",
                   settings->num_output_files,
                   (settings->num_output_files > 1 ? "files" : "file"));
        }

        if (normalize_file_list(settings,
                                settings->output_files,
                                normalize_callback) == 0)
        {
            ret = 0;
        }
    }
    return ret;
}

void fix_output_file_permissions(settings_t *settings)
{
    int i = 0;

    if (settings)
    {
        for(i = 0; i < settings->num_output_files; i++)
        {
            if (change_file_permissions(settings->output_files[i]))
            {
                fprintf(stderr,"Failed to modify permissions on\n%s. "
                        "Skipping.\n",settings->output_files[i]);
            }
        }
    }
}

int burn_audio_cd_copies(settings_t *settings, cdrecorder_t *recorders)
{
    int ret = AUDIO_BURN_ERROR;
    int current_disc_to_burn = 1;
    int num_copies_to_burn = s_num_copies_remaining;
    char input_char;

    while(num_copies_to_burn > 0)
    {
        if (settings->verbose)
        {
            fprintf(stderr,"Burning disc number %.3d/%.3d\n",
                    current_disc_to_burn, s_num_copies_remaining);
        }

        ret = cdrecord_burn_audio_cd(settings,recorders,
                                     cdrecord_callback);

        if (ret)
        {
            if (settings->verbose)
            {
                fprintf(stderr,"Failed with %d copies left to go\n",
                        num_copies_to_burn);
            }
            break;
        }

        current_disc_to_burn++;
        num_copies_to_burn--;

        if (num_copies_to_burn == 0)
        {
            ret = AUDIO_BURN_SUCCESS;
            break;
        }

        fprintf(stderr,"***********************************************"
                "*****************\n");
        fprintf(stderr," Disc burning complete.\n");
        fprintf(stderr," Please insert new CD and press the <ENTER> key "
                "to resume burning.\n\n");
        fprintf(stderr," Press <ESC> followed by <ENTER> to abort and "
                "quit now.\n");
        fprintf(stderr,"***********************************************"
                "*****************\n");

        while(1)
        {
            input_char = getchar();
            if (input_char == ASCII_ESCAPE_KEY)
            {
                return AUDIO_BURN_USER_ABORTED;
            }
            else if (input_char == ASCII_RETURN_KEY)
            {
                break;
            }
        }
    }
    return ret;
}

int burn_cd(settings_t *settings, cdrecorder_t *recorders)
{
    int ret = 1;
    int device_index = 0;

    if (settings->cdrecord_enabled && recorders)
    {
        if (recorders->num_devices)
        {
            device_index = settings->preferred_device_number;

            if ((device_index > -1) &&
                (device_index < recorders->num_devices))
            {
              check_audio_cd_length:
                /*
                  finally, verify that the selected audio tracks
                  can fit on a single audio CD
                */
                if (check_filelist_with_cd_length(
                        settings->target_disc_len_in_minutes,
                        settings->output_files,
                        settings->num_output_files,
                        settings->verbose) == 0)
                {
                    switch(burn_audio_cd_copies(settings, recorders))
                    {
                        case AUDIO_BURN_USER_ABORTED:
                            fprintf(stderr,"CD Burning session aborted "
                                    "by user request\n");
                            ret = AUDIO_BURN_SUCCESS;
                            break;
                        case AUDIO_BURN_SUCCESS:
                            ret = AUDIO_BURN_SUCCESS;
                            break;
                        default:
                            ret = AUDIO_BURN_ERROR;
                    }
                }
                else
                {
                    fprintf(stderr,"Sorry -- the selected audio tracks "
                            "won't fit on a single %d-minute audio CD!\n",
                            settings->target_disc_len_in_minutes);
                }
            }
            else
            {
                fprintf(stderr,"Valid recorder device not specified\n");
            }
        }
        else
        {
            if (strlen(settings->atapi_device))
            {
                goto check_audio_cd_length;
            }
            fprintf(stderr,"No valid recorder devices found\n");
        }
    }
    return ret;
}

void clean_temporary_files(settings_t *settings)
{
    int i = 0;

    if (settings)
    {
        for(i = 0; i < settings->num_output_files; i++)
        {
            if (delete_file(settings->output_files[i]))
            {
                fprintf(stderr,"Failed to remove %s. "
                        "Skipping.\n",settings->output_files[i]);
            }
        }
    }
}

int main(int argc, char **argv)
{
    int ret = 1;
    int num_utils_enabled = 0;
    int i = 0, file_type = 0;
    settings_t settings;
    cdrecorder_t recorders;
    char *home_directory = getenv("HOME");
    char *audioburn_rc_file = NULL;
    char buf[MAX_BUF_LEN] = {0};

    if (home_directory)
    {
        snprintf(buf, MAX_BUF_LEN,
                 "%s/.audioburnrc", home_directory);
        audioburn_rc_file = buf;
    }

    /* initialize session settings */
    if (settings_initialize(&settings, audioburn_rc_file))
    {
        fprintf(stderr,"Failed to initialize settings object\n");
        exit(1);
    }

    /* setup use of all utilities by default */
    if (settings_enable_utilities(&settings,
                                  (ENABLE_OGGDEC | ENABLE_MPGDEC |
                                   ENABLE_SOX | ENABLE_CDRECORD |
                                   ENABLE_NORMALIZE)))
    {
        fprintf(stderr,"Failed to enable utility settings\n");
        goto cleanup;
    }

    /*
      set other defaults (which can be overriden
      by command line options if specified)
    */
    settings_set_preferred_device_number(&settings,0);
    settings_set_temporary_directory(&settings,"/tmp/");

    /* check command line arguments */
    parse_command_line_args(argc,argv,&settings);

    /* initialize the libaudioburn library */
    if (libaudioburn_initialize(&settings) == 0)
    {
        if (settings.verbose)
        {
            printf("\n");
        }

        if (settings.oggdec_enabled)
        {
            if (settings.verbose)
            {
                printf("oggdec    found at: %s\n",
                       settings.oggdec_path);
            }
            ++num_utils_enabled;
        }
        if (settings.mpgdec_enabled)
        {
            if (settings.verbose)
            {
                printf("mpg321    found at: %s\n",
                       settings.mpgdec_path);
            }
            ++num_utils_enabled;
        }
        if (settings.normalize_enabled)
        {
            if (settings.verbose)
            {
                printf("normalize found at: %s\n",
                       settings.normalize_path);
            }
            ++num_utils_enabled;
        }
        if (settings.sox_enabled)
        {
            if (settings.verbose)
            {
                printf("sox       found at: %s\n",
                       settings.sox_path);
            }
            ++num_utils_enabled;
        }
        if (settings.cdrecord_enabled)
        {
            if (settings.verbose)
            {
                printf("cdrecord  found at: %s\n",
                       settings.cdrecord_path);
            }
            ++num_utils_enabled;
        }

        if (num_utils_enabled)
        {
            if (settings.verbose)
            {
                printf("\n");
            }
        }
        else
        {
            printf("Disabling all utilities makes audio_burn worthless!\n");
        }
    }
    else
    {
        fprintf(stderr,"Failed to initialize the libaudioburn "
                "library.\nProgram terminating.\n");
        goto cleanup;
    }

    if (strlen(settings.config_file) && settings.verbose)
    {
        fprintf(stderr,"Read Config options from %s\n", settings.config_file);
    }

    /* probe for cdrecorder information (if enabled) */
    if (settings.cdrecord_enabled)
    {
        if (cdrecord_get_recorder_info(&settings,&recorders))
        {
            fprintf(stderr,"Failed to get cdrecorder information.\n");
            fprintf(stderr,"Please make sure that you have configured a");
            fprintf(stderr," cdrecorder for normal use (i.e. it\nshows up");
            fprintf(stderr," in the output of \"cdrecord -scanbus\").\n\n");
            fprintf(stderr,"For some more information please visit:\n\n");
            fprintf(stderr,"http://mlug.missouri.edu/~rjudd/projects/"
                    "CD-Writing.html\n");
            goto cleanup;
        }
        else if (strlen(settings.atapi_device))
        {
            printf("Using specified ATAPI device %s by user request\n",
                   settings.atapi_device);
        }
        else
        {
            cdrecord_print_recorder_info(&recorders);
            printf("\n");

            /* make sure the preferred device (if any is valid) */
            if ((settings.preferred_device_number < 0) ||
                (settings.preferred_device_number >=
                 recorders.num_devices))
            {
                fprintf(stderr,"Specified cd burning device %d was not "
                        "detected.\nPlease specify another device!\n",
                        settings.preferred_device_number);
                goto cleanup;
            }
            else if (settings.verbose)
            {
                printf("Using CD burning device number %d\n",
                       settings.preferred_device_number);
            }
        }
    }

    /* see if we have any work to do */
    if ((settings.num_input_files == 0) || (num_utils_enabled == 0))
    {
        fprintf(stderr,"\nNothing to do.  Program Terminating.\n");
        goto cleanup;
    }

    /*
      run through the specified input files and
      explode any detected playlists
    */
  playlist_explode:
    for(i = 0; i < settings.num_input_files; i++)
    {
        file_type = get_file_type(settings.input_files[i]);
        switch(file_type)
        {
            case FILE_TYPE_M3U:
            {
                int val = settings_add_m3u_playlist_file(
                    &settings, settings.input_files[i]);
                if (val)
                {
                    fprintf(stderr, "Failed to add m3u playlist "
                            "contents: ");
                    switch(val)
                    {
                        case 1:
                            fprintf(stderr,"Cannot expand list\n");
                            return ret;
                        case -1:
                            fprintf(stderr,
                                    "Too many files specified\n");
                            return ret;
                        case -2:
                            fprintf(stderr,
                                    "Invalid file in playlist\n");
                            return ret;
                        default:
                            fprintf(stderr,"Unknown error\n");
                            return ret;
                    }
                }
                goto playlist_explode;
            }
        }
    }

    /*
      compute total length of tracks specified and compare
      against the total cd length.  error exit if the tracks
      are longer than the specified audio disc length
    */
    if (check_filelist_with_cd_length(
            settings.target_disc_len_in_minutes, settings.input_files,
            settings.num_input_files, settings.verbose))
    {
        printf("\n*** There are too many songs to burn to a %d minute "
               "audio CD!\n", settings.target_disc_len_in_minutes);
        printf(" Please specify fewer songs to burn at a time, or use\n"
               " the --cdlength option to specify that your disc can\n"
               " hold more audio.\n");
        exit(1);
    }

    /* process/decode all input files to have them ready for burning */
    if (process_input_files(&settings))
    {
        fprintf(stderr,"Failed to process input files.\n");
        fprintf(stderr,"Please contact the author.  Terminating.\n");
        goto cleanup;
    }

    /* if specified, normalize all outputted files to each other */
    if (settings.normalize_enabled)
    {
        if (normalize_output_files(&settings))
        {
            fprintf(stderr,"Failed to normalize outputted wav files.\n");
            fprintf(stderr,"Please contact the author.  Terminating.\n");
            goto cleanup;
        }
    }

    /*
      now that we've edited the files, make the permissions more
      user friendly (for the case that we are being run setuid)
    */
    fix_output_file_permissions(&settings);

    /* finally, burn the data */
    if (settings.cdrecord_enabled)
    {
        if (burn_cd(&settings,&recorders))
        {
            fprintf(stderr,"Failed to write cd track data.\n");
            fprintf(stderr,"Please contact the author.  Terminating.\n");
            goto cleanup;
        }
        else
        {
            if (settings.verbose)
            {
                printf("Removing temporary files ... ");
            }
            if (settings.clean_tmp_files)
            {
                clean_temporary_files(&settings);
            }
            if (settings.verbose)
            {
                printf("done.\n");
            }
            printf("CD Burning Complete!\n");
        }
    }

    /* if we get here, all is well */
    ret = 0;

  cleanup:

    /* uninitialize session settings */
    settings_uninitialize(&settings);

    return ret;
}
