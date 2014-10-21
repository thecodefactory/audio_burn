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
#ifndef __LIBAUDIOBURN_H
#define __LIBAUDIOBURN_H

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "securityutils.h"
#include "constants.h"
#include "settings.h"
#include "processutils.h"
#include "miscutils.h"
#include "vorbis.h"
#include "oggutils.h"
#include "mpg.h"
#include "mpgutils.h"
#include "wav.h"
#include "wavutils.h"
#include "normutils.h"
#include "soxutils.h"
#include "cdrecordutils.h"

/*
  high-level initialization for this library.
*/
int libaudioburn_initialize(settings_t *settings);


#endif /* __LIBAUDIOBURN_H */
