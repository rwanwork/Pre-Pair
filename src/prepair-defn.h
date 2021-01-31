/*    Pre-Pair
**    Word-based Pre-processor for Re-Pair
**    Copyright (C) 2003, 2007 by Raymond Wan (rwan@kuicr.kyoto-u.ac.jp)
**
**    Version 1.0.1 -- 2007/04/02
**
**    This file is part of the Pre-Pair software.
**
**    Pre-Pair is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    Pre-Pair is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License along
**    with Pre-Pair; if not, write to the Free Software Foundation, Inc.,
**    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef PREPAIR_DEF_H
#define PREPAIR_DEF_H

#define UINT_SIZE_BITS 32

/*  Size of file buffers (for input or output)  */
#define OUTBUFMAX       1048576

#define MAXPRIMS 256
/*  A user can specify the maximum length of a word at the command 
**  line. The length, maxword, must satisfy:  
**  WORDLEN <= maxword <= MAXWORDLEN  */
#define WORDLEN 12
#define MAXWORDLEN 16

/*  The header for a word in the word dictionary contains a
**  one byte match/length header (4 bits each) and, for appended
**  words, an additional 4 bytes to reference the longest match
**  in the dictionary.  */
#define MAXWORDLEN_HEADER  5

#define ISWORD(C)  ((isalnum ((R_INT) C)) || ((R_UINT) C == (R_UINT) '<') || ((R_UINT) C == (R_UINT) '>') || ((R_UINT) C == (R_UINT) '/'))
enum WORDTYPE { ISWORD = 0, ISNONWORD = 1 };

enum PROGMODE { MODE_NONE = 0, MODE_ENCODE = 1, MODE_DECODE = 2, MODE_DECODE_NONE = 3, MODE_DECODE_LINK = 4 };

typedef struct filestruct {
  /*  Word dictionary, extension ".wd"  */
  R_UCHAR *wd_name;
  FILE *wd_fp;
  R_UCHAR *wd_buf;
  R_UCHAR *wd_p;
  R_UCHAR *wd_end;

  /*  Word sequence, extension ".ws"  */
  R_UCHAR *ws_name;
  FILE *ws_fp;
  R_UINT *ws_buf;
  R_UINT *ws_p;
  R_UINT *ws_end;

  /*  Non-word dictionary (phrase hierarchy), extension ".nwd"  */
  R_UCHAR *nwd_name;
  FILE *nwd_fp;
  R_UCHAR *nwd_buf;
  R_UCHAR *nwd_p;
  R_UCHAR *nwd_end;

  /*  Non-word sequence, extension ".nws"  */
  R_UCHAR *nws_name;
  FILE *nws_fp;
  R_UINT *nws_buf;
  R_UINT *nws_p;
  R_UINT *nws_end;

  /*  Case-folding modifiers, extension ".cfm"  */
  R_UCHAR *cfm_name;
  FILE *cfm_fp;
  R_UINT *cfm_buf;
  R_UINT *cfm_p;
  R_UINT *cfm_end;

  /*  Stemming modifiers, extension ".sm"  */
  R_UCHAR *sm_name;
  FILE *sm_fp;
  R_UINT *sm_buf;
  R_UINT *sm_p;
  R_UINT *sm_end;

  R_BOOLEAN verbose_level;
  enum PROGMODE mode;
} FILE_STRUCT;

#endif
