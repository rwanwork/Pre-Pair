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

#ifndef FCODE_H
#define FCODE_H

/*
**  Start front-code ids at 1 because and 0 is used for a 0 length 
**  front-code.
*/
#define EMPTY_FCODE 0
#define FIRST_FCODE 1

/*  If the lexicon is front-coded, the size is not written in
**  the file, so start it at this value and realloc if
**  necessary.  */
#define INIT_FCODE_SIZE 1024

#define FCODETREENULL	((struct fcodetree *) NULL)
#define FILENULL	((FILE *) NULL)
#define CHARNULL	((char *) NULL)

typedef struct fcodetree {
  R_UCHAR *item;
  R_UINT len;
  R_UINT freq;
  R_UINT id;
  struct fcodetree *left, *rght, *prnt;
} FCODETREE;


typedef struct fcodenode {
  R_UCHAR *item;
  R_UINT len;
  R_UINT freq;
  R_UINT init_id;
  R_UINT id;
} FCODENODE;


#define LOOKUPFCODE(DICT,KEY,ITEM,LEN) \
  LEN = DICT[KEY].len; \
  ustrncpy (ITEM, DICT[KEY].item, LEN);
  
/* 
**  Rotations used in the splaying operations
*/
#define ROTATEL(p,q) \
  do { \
    p->prnt = q->prnt; \
    q->prnt = p; \
    q->rght = p->left; \
    if (q->rght != FCODETREENULL) \
      q->rght->prnt = q; \
    p->left = q; \
    if (p->prnt != FCODETREENULL) { \
      if (q == p->prnt->left) \
        p->prnt->left = p; \
      else \
        p->prnt->rght = p; \
    } \
  } while (0)

#define ROTATER(p,q) \
  do { \
    p->prnt = q->prnt; \
    q->prnt = p; \
    q->left = p->rght; \
    if (q->left != FCODETREENULL) \
      q->left->prnt = q; \
    p->rght = q; \
    if (p->prnt != FCODETREENULL) { \
      if (q == p->prnt->left) \
        p->prnt->left = p; \
      else \
        p->prnt->rght = p; \
    } \
  } while (0)

R_UINT fcodeEncode (R_UCHAR *item, R_UINT len, FCODETREE **fcode_root, R_UINT *itemcount, R_UINT *item_compares, R_UINT *total_itemlen);

void fcodeDictEncode (FILE_STRUCT *file_info, FCODETREE *fcode_root, FCODENODE *fcode_dict, R_UINT *fcode_map, R_BOOLEAN printsorted, R_UINT nitems, enum WORDTYPE type);

R_UINT fcodeDictDecode (FILE_STRUCT *file_info, FCODENODE **fcode_dict, R_UINT nitems, enum WORDTYPE type);

#endif
