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

#ifndef WORD_H
#define WORD_H


typedef struct wordstruct {
  /*  General settings  */
  R_BOOLEAN docasefold;                    /*  Perform case-folding  */
  R_UINT maxword;                      /*  Maximum length of a word  */
  R_BOOLEAN dostem;                                  /*  Stem words  */

  R_UINT nwords;                                /*  Number of words  */
  R_UINT total_words_len;        /*  Length of all words in lexicon  */
  R_UINT nwords_prims;     /*  Number of words which are in Latin-1  */
  R_UINT cmps;               /*  Number of splay tree nodes visited  */
  R_UINT *map;           /*  Map used to re-encode sequence symbols  */

  /*  Statistics about parsing  */
  R_UINT total_tokens;
  R_UINT total_length;
  R_UINT long_tokens;
  R_UINT enforce_tags;
  R_UINT zerolength_sym;

  /*  Front-coding words  */
  FCODETREE *root_fc;
  FCODENODE *dict_fc;
  R_BOOLEAN printsorted;            /*  Print words in sorted order  */
} WORD_STRUCT;


R_UINT getWord (R_UCHAR **src_p, R_UCHAR *src_end, R_UCHAR *w, R_UINT lim, R_BOOLEAN *notdone, R_UINT *long_tokens, R_UINT *enforce_tags);

#endif

