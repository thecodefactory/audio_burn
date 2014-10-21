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
#ifndef __SECURITYUTILS_H
#define __SECURITYUTILS_H

/*
  obtains (real) root privileges if possible given
  then effective ID.
  returns 0 on success; 1 otherwise
*/
int elevate_privileges();

/* drops (real) root privileges (keeps effective) */
void drop_privileges();

/*
  attempts to change the owner and permissions
  of the specified filename to be that of the
  user that ran this program (instead of root)
  and sets the permissions to 660 (rw,rw,--).

  returns 0 on success; returns 1 otherwise
*/
int change_file_permissions(char *filename);

#endif /* __SECURITYUTILS_H */
