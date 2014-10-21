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
#ifndef __MISCUTILS_H
#define __MISCUTILS_H

#ifndef MIN
int MIN(int a, int b);
#endif /* MIN */

/*
  given a filename, this function returns 1 if the
  file extension is supported; returns 0 otherwise
*/
int is_supported_file_type(settings_t *settings, char *filename);

/*
  given a filename, return the file type.
  this value will be one of the FILE_TYPE_*
  values listed in constants.h
*/
int get_file_type(char *filename);

/*
  given a filename, strips off the leading
  path elements (if any) and returns only a pointer
  to the start of the actual filename, or NULL
  on error.
*/
char *strip_path_to_filename(char *filename);

/*
  given a filename, returns the path element (if any)
  up to the filename.  if a path is not present NULL
  is returned.  if the return value is non-NULL, it
  must be freed by the caller.
*/
char *get_path_to_filename(char *filename);

/*
  given a filename, strips off the ending
  extension name (if any) by modifying the input
  string 'filename'.
*/
void strip_filename_extension(char *filename);

/*
  given a settings object and an input filename,
  store an output filename into the provided buffer
  (up to maxlen bytes).

  returns 0 on success; returns 1 otherwise
*/
int generate_output_filename(settings_t *settings,
                             char *filename,
                             char *buf, int maxlen);

/*
  given a filename, if no path component is present, prepend
  the current working directory to the filename and store
  in the provided buffer (up to maxlen bytes).  This is
  important for proper symlink usage if necessary (i.e. when
  no path is specified at all) -- otherwise, simply copy the
  incoming filename into the provided buffer (up to maxlen bytes)

  returns 0 on success; returns 1 otherwise
*/
int get_full_path_name(char *filename, char *buf, int maxlen);


/*
  creates  a symbolic link named newpath which
  contains the string oldpath.

  returns 0 on success; returns 1 otherwise
*/
int soft_link(char *src_file, char *dst_file);

/*
  attempts to delete/remove the file specified
  by 'filename'

  returns 0 on success; returns 1 on error
*/
int delete_file(char *filename);

/*
  returns the song length of the specified 'filename'
  on success; returns -1 otherwise
*/
int get_song_length(char *filename);

/*
  replaces every occurence of 'a' with 'b' in
  string 'str'.  Returns a copied string that must be
  freed on success; NULL otherwise
*/
char *replace_chars(char a, char b, char *str);

/*
  returns whether or not the file in the file list are
  too long to be burned to a CD of length 'cd_length_in_minutes'.
  returns 1 if the specified files are too long;
  returns 0 otherwise
  If the verbose flag is specified, the total computed length
  of all specified files will be printed to stdout.
*/
int check_filelist_against_cd_length(int cd_length_in_minutes,
                                     char **filelist,
                                     int num_files,
                                     int verbose);

/*
  returns an array of filenames parsed from the specified m3u playlist
  'filename' and the number of filenames in the array in the provided
  'num_tracks' on success.  returns NULL otherwise.
*/
char **explode_m3u_playlist_file(char *filename,
                                 int *num_tracks,
                                 int verbose);

#endif /* __MISCUTILS_H */
