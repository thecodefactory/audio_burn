/*
 * Copyright (C) Tony Arcieri <bascule@inferno.tusculum.edu>
 * Copyright (C) 2001  Haavard Kvaalen <havardk@xmms.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 */

#include "libaudioburn.h"

int vorbis_get_song_length(char *filename, int *length)
{
    int ret = -1;

#ifdef HAVE_OGGVORBIS
    FILE *stream = (FILE *)0;
    OggVorbis_File vf;
	
    if (strncasecmp(filename, "http://", 7))
    {
        if ((stream = fopen(filename, "r")) == NULL)
        {
            fprintf(stderr,"Failed to fopen ogg file %s\n",filename);
            return ret;
        }
   
        /*
         * The open function performs full stream detection and
         * machine initialization.  If it returns zero, the stream
         * *is* Vorbis and we're fully ready to decode.
         */
        if (ov_open(stream, &vf, NULL, 0) < 0)
        {
            fclose(stream);
            fprintf(stderr,"Failed to ov_open ogg file %s\n",filename);
            return ret;
        }
	
        /* Retrieve the length */
        *length = ov_time_total(&vf, -1);
  	 
        /*
         * once the ov_open succeeds, the stream belongs to
         * vorbisfile.a.  ov_clear will fclose it
         */
        ov_clear(&vf);
        ret = 0;
    }
    else
    {
        /* streaming song info */
        *length = -1;
        fprintf(stderr,"%s appears to be a streaming file.\n",filename);
    }

#endif /* HAVE_OGGVORBIS */

    return ret;
}
