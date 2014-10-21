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
  NOTE:
  All code here is slightly modified from mpg123 and XMMS.
  Copyright is due to the respective authors of these code bases.
*/

/*
  NOTE:
  The following has been taked from mpg123's common.c file
*/
int tabsel_123[2][3][16] = {
   { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
     {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
     {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

   { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

long freqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };

double compute_bpf(struct frame *fr)
{
    double bpf;

    switch(fr->lay) {
        case 1:
            bpf = tabsel_123[fr->lsf][0][fr->bitrate_index];
            bpf *= 12000.0 * 4.0;
            bpf /= freqs[fr->sampling_frequency] <<(fr->lsf);
            break;
        case 2:
        case 3:
            bpf = tabsel_123[fr->lsf][fr->lay-1][fr->bitrate_index];                        bpf *= 144000;
            bpf /= freqs[fr->sampling_frequency] << (fr->lsf);
            break;
        default:
            bpf = 1.0;
    }

    return bpf;
}

double compute_tpf(struct frame *fr)
{
    static int bs[4] = { 0,384,1152,1152 };
    double tpf;

    tpf = (double) bs[fr->lay];
    tpf /= freqs[fr->sampling_frequency] << (fr->lsf);
    return tpf;
}

int decode_header(struct frame *fr, unsigned long newhead)
{
    if (!head_check(newhead))
    {
        return 0;
    }

    if (newhead & (1<<20))
    {
        fr->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
        fr->mpeg25 = 0;
    }
    else
    {
        fr->lsf = 1;
        fr->mpeg25 = 1;
    }

    fr->lay = 4-((newhead>>17)&3);
    if( ((newhead>>10)&0x3) == 0x3) {
        fprintf(stderr,"Stream error\n");
        exit(1);
    }
    if(fr->mpeg25) {
        fr->sampling_frequency = 6 + ((newhead>>10)&0x3);
    }
    else
        fr->sampling_frequency = ((newhead>>10)&0x3) + (fr->lsf*3);
    fr->error_protection = ((newhead>>16)&0x1)^0x1;

    fr->bitrate_index = ((newhead>>12)&0xf);
    fr->padding   = ((newhead>>9)&0x1);
    fr->extension = ((newhead>>8)&0x1);
    fr->mode      = ((newhead>>6)&0x3);
    fr->mode_ext  = ((newhead>>4)&0x3);
    fr->copyright = ((newhead>>3)&0x1);
    fr->original  = ((newhead>>2)&0x1);
    fr->emphasis  = newhead & 0x3;

    fr->stereo    = (fr->mode == MPG_MD_MONO) ? 1 : 2;

    if(!fr->bitrate_index)
    {
        fprintf(stderr,"Free format not supported: (head %08lx)\n",newhead);
        return (0);
    }

    switch(fr->lay) {
        case 1:
            fr->framesize  = (long) tabsel_123[fr->lsf][0][fr->bitrate_index] * 12000;
            fr->framesize /= freqs[fr->sampling_frequency];
            fr->framesize  = ((fr->framesize+fr->padding)<<2)-4;
            break;
        case 2:
            fr->framesize = (long) tabsel_123[fr->lsf][1][fr->bitrate_index] * 144000;
            fr->framesize /= freqs[fr->sampling_frequency];
            fr->framesize += fr->padding - 4;
            break;
        case 3:
            fr->framesize  = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
            fr->framesize /= freqs[fr->sampling_frequency]<<(fr->lsf);
            fr->framesize = fr->framesize + fr->padding - 4;
            break; 
        default:
            fprintf(stderr,"Sorry, unknown layer type.\n"); 
            return (0);
    }
    return 1;
}

int head_check(unsigned long head)
{
    if( (head & 0xffe00000) != 0xffe00000)
	return FALSE;
    if(!((head>>17)&3))
	return FALSE;
    if( ((head>>12)&0xf) == 0xf)
	return FALSE;
    if( ((head>>10)&0x3) == 0x3 )
	return FALSE;
    if ((head & 0xffff0000) == 0xfffe0000)
        return FALSE;

    return TRUE;
}
/*
  NOTE:
  End code taken from mpg123's common.c file
*/


/*
  NOTE:
  The following is based entirely on XMMS's mpg123 plugin code
*/
int get_xing_header(xing_header_t *xing, unsigned char *buf)
{	
    int i, head_flags;
    int id, mode;
	
    memset(xing, 0, sizeof(xing_header_t));
	
    /* get selected MPEG header data */ 
    id = (buf[1] >> 3) & 1;
    mode = (buf[3] >> 6) & 3;
    buf += 4;
	
    /* Skip the sub band data */
    if (id)
    {
        /* mpeg1 */
        if (mode != 3)
            buf += 32;
        else
            buf += 17;
    }
    else
    {
        /* mpeg2 */
        if (mode != 3)
            buf += 17;
        else
            buf += 9;
    }
	
    if (strncmp(buf, "Xing", 4))
        return 0;
    buf += 4;
		
    head_flags = GET_INT32BE(buf);
	
    if (head_flags & FRAMES_FLAG)
        xing->frames = GET_INT32BE(buf);
    if (xing->frames < 1)
        xing->frames = 1;
    if (head_flags & BYTES_FLAG)
        xing->bytes = GET_INT32BE(buf);
	
    if (head_flags & TOC_FLAG)
    {
        for (i = 0; i < 100; i++)
            xing->toc[i] = buf[i];
        buf += 100;
    }
    return 1;
}

unsigned int convert_to_header(unsigned char *buf)
{
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

unsigned int get_song_time(char *filename)
{
    unsigned int head;
    unsigned char tmp[4], *buf;
    struct frame frm;
    xing_header_t xing_header;
    double tpf, bpf;
    unsigned int len;
    FILE *file = (FILE *)0;

    if (!filename)
    {
        return 0;
    }

    if ((file = fopen(filename,"r")) == NULL)
    {
        return 0;
    }

    fseek(file, 0, SEEK_SET);
    if (fread(tmp, 1, 4, file) != 4)
    {
        fclose(file);
        return 0;
    }
    head = convert_to_header(tmp);
    while(!head_check(head))
    {
        head <<= 8;
        if (fread(tmp, 1, 1, file) != 1)
        {
            fclose(file);
            return 0;
        }
        head |= tmp[0];
    }
    if (decode_header(&frm, head))
    {
        buf = malloc(frm.framesize + 4);
        fseek(file, -4, SEEK_CUR);
        fread(buf, 1, frm.framesize + 4, file);
        tpf = compute_tpf(&frm);
        if (get_xing_header(&xing_header, buf))
        {
            free(buf);
            fclose(file);
            return ((unsigned int)(tpf * xing_header.frames));
        }
        free(buf);
        bpf = compute_bpf(&frm);
        fseek(file, 0, SEEK_END);
        len = ftell(file);
        fseek(file, -128, SEEK_END);
        fread(tmp, 1, 3, file);
        if (!strncmp(tmp, "TAG", 3))
        {
            len -= 128;
        }
        fclose(file);
        return ((unsigned int)((unsigned int)(len / bpf) * tpf));
    }
    fclose(file);
    return 0;
}
/*
  NOTE:
  End code taken based heavily on XMMS's mpg123 plugin code
*/
