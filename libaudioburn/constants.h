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
#ifndef __CONSTANTS_H
#define __CONSTANTS_H

/*
  a callback function declaration.  buf will contain
  buflen bytes of data.  the child_pid is the process
  ID of the spawned child generating the callbacks.
*/
typedef void (*fn_callback_t)(pid_t child_pid,
                              char *buf,
                              int buflen);

#define CHILD_WAIT            0x00000001

#define MAX_PATH_LEN          0x00000100
#define MAX_BUF_LEN           0x00000400
#define MAX_NORMALIZED_FILES  0x00000040
#define MAX_NUM_DEVICES       0x00000008
#define MAX_DEVICENAME_LEN    0x00000040
#define MAX_NUM_INPUT_FILES   0x00000080
#define MAX_NUM_OUTPUT_FILES  MAX_NUM_INPUT_FILES
#define MAX_NUM_CDRECORD_ARGS (MAX_NUM_INPUT_FILES + 10)
#define MAX_DEVICE_OPTION_LEN 0x00000010
#define MAX_SPEED_OPTION_LEN  0x00000010
#define MAX_GRACETIME_STR_LEN 0x00000020

#define CHILD_IO_OUT          0x00000001
#define CHILD_IO_ERR          0x00000002

#define ENABLE_OGGDEC         0x00000001
#define ENABLE_MPGDEC         0x00000002
#define ENABLE_SOX            0x00000004
#define ENABLE_CDRECORD       0x00000008
#define ENABLE_NORMALIZE      0x00000010

#define FILE_TYPE_ERROR       0x00000000
#define FILE_TYPE_UNKNOWN     0x00000001
#define FILE_TYPE_WAV         0x00000002
#define FILE_TYPE_MP3         0x00000003
#define FILE_TYPE_OGG         0x00000004
#define FILE_TYPE_M3U         0x00000005
#define FILE_TYPE_PLS         0x00000006

/* defaults passed in from autoconf detection */
#define WHICH_PATH            WHICH_PROG
#define FILE_PATH             FILE_PROG
#define MPGDEC_PATH           MPG321_PROG
#define OGGDEC_PATH           OGGDEC_PROG
#define SOX_PATH              SOX_PROG
#define NORMALIZE_PATH        NORMALIZE_PROG
#define CDRECORD_PATH         CDRECORD_PROG

#define OGGDEC_DOWNLOAD_SITE \
"http://www.vorbis.com/download_unix.psp"

#define MPG321_DOWNLOAD_SITE \
"http://mpg321.sourceforge.net/"

#define SOX_DOWNLOAD_SITE \
"http://sox.sourceforge.net/"

#define NORMALIZE_DOWNLOAD_SITE \
"http://www.cs.columbia.edu/~cvaill/normalize/"

#define CDRECORD_DOWNLOAD_SITE \
"http://www.fokus.gmd.de/research/cc/glone/employees/joerg.schilling/private/cdrecord.html"

/*
  the following are settings that should be
  override-able using command line switches
*/
#define TMP_PATH \
"/tmp/"

#endif /* __CONSTANTS_H */
