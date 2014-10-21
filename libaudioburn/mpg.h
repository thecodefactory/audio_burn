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
#ifndef __MPG_H
#define __MPG_H

/*
  NOTE:
  All code here is slightly modified from mpg123 and XMMS.
  Copyright is due to the respective authors of these code bases.
*/
/* From mpg123's code base */
#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

#define MPG_MD_MONO 3

struct frame {
    int stereo;
    int jsbound;
    int single;
    int II_sblimit;
    int down_sample_sblimit;
    int lsf;
    int mpeg25;
    int down_sample;
    int header_change;
    int lay;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */
};

double compute_bpf(struct frame *fr);
double compute_tpf(struct frame *fr);
int decode_header(struct frame *fr, unsigned long newhead);
int head_check(unsigned long head);

/* From XMMS's dxhead.[ch] */
#define GET_INT32BE(b) \
(i = (b[0] << 24) | (b[1] << 16) | b[2] << 8 | b[3], b += 4, i)

#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008

typedef struct
{
	int frames;
	int bytes;
	unsigned char toc[100];
} xing_header_t;

int get_xing_header(xing_header_t *xing, unsigned char *buf);

/* From XMMS's mpg123.c */
unsigned int convert_to_header(unsigned char *buf);
unsigned int get_song_time(char *filename);

#endif /* __MPG_H */
