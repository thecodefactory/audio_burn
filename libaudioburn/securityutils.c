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

static uid_t __my_uid = -1;

int elevate_privileges()
{
    int ret = 1;
    uid_t effective_uid = geteuid();

    __my_uid = getuid();
    if (effective_uid == 0)
    {
        if (setuid(0) == 0)
        {
            ret = 0;
        }
    }
    return ret;
}

void drop_privileges()
{
    setreuid(__my_uid,0);
}

int change_file_permissions(char *filename)
{
    int ret = 1;
    struct stat statbuf;

    if (__my_uid == -1)
    {
        __my_uid = getuid();
    }

    if (filename)
    {
        memset(&statbuf,0,sizeof(statbuf));

        /* make sure the specified file exists */
        if (stat(filename,&statbuf) == 0)
        {
            if (chown(filename,__my_uid,-1) == 0)
            {
                if (chmod(filename,(S_IRUSR | S_IWUSR |
                                    S_IRGRP | S_IWGRP)) == 0)
                {
                    ret = 0;
                }
            }
        }
    }
    return ret;
}
