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

int libaudioburn_initialize(settings_t *settings)
{
    int ret = 1;
    int err = 0;

    /* test privileges */
    if (elevate_privileges())
    {
        fprintf(stderr,"Permission Error: Please run this program "
                "as root or modify the program\n"
                "  permissions to have the set ID bit set\n\n");
        return ret;
    }
    else
    {
        drop_privileges();
    }

    /* initialize all utility libs specified */
    if (settings)
    {
        if (settings->oggdec_enabled)
        {
            err += ogg_initialize(settings);
        }
        if ((err == 0) && settings->mpgdec_enabled)
        {
            err += mpg_initialize(settings);
        }
        if ((err == 0) && settings->normalize_enabled)
        {
            err += normalize_initialize(settings);
        }
        if ((err == 0) && settings->sox_enabled)
        {
            err += sox_initialize(settings);
        }
        if ((err == 0) && settings->cdrecord_enabled)
        {
            err += cdrecord_initialize(settings);
        }
        ret = (err ? 1 : 0);
    }
    return ret;
}
