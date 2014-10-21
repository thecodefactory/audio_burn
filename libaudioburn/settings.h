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
#ifndef __SETTINGS_H
#define __SETTINGS_H

#define DEFAULT_AUDIO_CD_LEN_IN_MINUTES  74

typedef struct
{
    int sox_enabled;
    int oggdec_enabled;
    int mpgdec_enabled;
    int cdrecord_enabled;
    int normalize_enabled;

    char sox_path[MAX_PATH_LEN];
    char oggdec_path[MAX_PATH_LEN];
    char mpgdec_path[MAX_PATH_LEN];
    char cdrecord_path[MAX_PATH_LEN];
    char normalize_path[MAX_PATH_LEN];

    int dao;
    int eject;
    int speed;
    int verbose;
    int dummy_run;
    int clean_tmp_files;
    int save_settings;
    int gracetime;
    int burn_free;

    int atapi;
    char atapi_device[MAX_PATH_LEN];

    int preferred_device_number;
    int target_disc_len_in_minutes;

    int num_input_files;
    char *input_files[MAX_NUM_INPUT_FILES];

    int num_output_files;
    char *output_files[MAX_NUM_OUTPUT_FILES];

    char tmpdir[MAX_PATH_LEN];

    char config_file[MAX_PATH_LEN];
} settings_t;

/*
  clears the settings object and returns 0
  on success; returns 1 on error; returns -1 on config read/create file error

  MUST be called or results of using the settings object may be
  unexpected.

  if filename is specified, settings will be initialized with options
  from the configuration file at that location (or that file will be
  created if it does not exist).  during uninitialize, settings will
  be saved to that filename if the settings->save_settings flag is set
*/
int settings_initialize(settings_t *settings, char *filename);

/*
  MUST be called when the settings object is no
  longer needed in your application.

  if a filename was specified on settings_initialize and the
  settings->save_settings flag is set, general options will be written
  to that filename during this call.
*/
void settings_uninitialize(settings_t *settings);

/*
  reads the specified filename and fills in matching settings.  if the
  filename specified does not exist, it is created.  returns 0 on
  success; 1 on error
*/
int settings_read_config_file(settings_t *settings, char *filename);

/*
  writes the specified filename with the matching settings.  the
  specified filename MUST exist or else the write is aborted.
*/
void settings_write_config_file(settings_t *settings, char *filename);

/*
  enables the use of various utilities.
  'flags' may be a bit-wise OR'd combination of
  the following values:

  ENABLE_OGGDEC
  ENABLE_MPGDEC
  ENABLE_SOX
  ENABLE_CDRECORD
  ENABLE_NORMALIZE

  Only the flags specified will initialize the
  corresponding utilities.  Non specified utilities
  cannot be used during run-time.

  returns 0 on success; returns 1 otherwise.
*/
int settings_enable_utilities(settings_t *settings, int flags);

/*
  adds the specified input file 'input_file' to the
  settings object.

  returns 0 on success; returns 1 otherwise

  an error indicates that there are too many
  input files (and no more can be added) or that
  the input_file is not a valid file.
*/
int settings_add_input_file(settings_t *settings,
                            char *input_file);

/*
  removes the specified input file 'input_file' from the
  settings object.
*/
void settings_remove_input_file(settings_t *settings,
                                char *input_file);

/*
  adds the corresponding output file name (usually
  based on the input file name) at the specified
  index (usually matches input file index).

  returns 0 on success; returns 1 otherwise

  an error indicates that there are too many
  output files (and no more can be added) or that
  the output_file is not a valid file.
*/
int settings_add_output_file(settings_t *settings,
                             char *output_file);

/*
  removes the specified output file 'output_file' from the
  settings object.
*/
void settings_remove_output_file(settings_t *settings,
                                 char *output_file);

/* set the preferred cd burning device number */
void settings_set_preferred_device_number(settings_t *settings,
                                          int device_num);

/* set the temporary directory to use for processing files */
void settings_set_temporary_directory(settings_t *settings,
                                      char *tmpdir);

/* sets the verbose output flag */
void settings_set_verbose_output(settings_t *settings);

/*
  sets the dummy_run flag on the preferences object
  to instruct the cd burner not to actually write the tracks
*/
void settings_set_dummy_run(settings_t *settings);

/* sets the gracetime in seconds before starting to write disc */
void settings_set_gracetime(settings_t *settings, int gracetime);

/* allows cdrecord to enable the burnfree option when writing */
void settings_set_burnfree(settings_t *settings);

/* allows cdrecord to use cdrecord's atapi support for writing */
void settings_set_atapi(settings_t *settings, int val);

/* allows cdrecord to use a particular atapi device for writing */
void settings_set_atapi_dev(settings_t *settings, char *device);

/* sets the flag that enables/disables cleaning of temporary files */
void settings_clean_tmp_files(settings_t *settings,
                              int clean_tmp_files);

/* sets the preferred device burning speed */
void settings_set_device_speed(settings_t *settings, int speed);

/* setting to burn disc in session at once mode */
void settings_set_session_at_once(settings_t *settings);

/* setting to eject disc after burn is complete */
void settings_set_device_eject(settings_t *settings);

/*
  sets the target audio disc length in minutes;
  default length is DEFAULT_AUDIO_CD_LEN_IN_MINUTES minutes
*/
void settings_set_disc_length_in_minutes(settings_t *settings,
                                         int len_in_minutes);

/*
  parses an m3u playlist file and adjusts the settings object
  appropriately for further processing upon success.

  returns:
   0 on success,
   1 on error,
  -1 if the expanded list contains more files than the
     MAX_NUM_INPUT_FILES, and
  -2 if a file in the playlist was found to not exist.
*/
int settings_add_m3u_playlist_file(settings_t *settings,
                                   char *filename);


#endif /* __SETTINGS_H */
