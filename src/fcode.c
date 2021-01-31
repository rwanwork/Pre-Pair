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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "common-def.h"
#include "wmalloc.h"
#include "ustring.h"
#include "prepair-defn.h"
#include "fcode.h"

static FCODETREE *splayFcode (FCODETREE *p);
static void traverseFcodeDict (FCODETREE *t, FCODENODE *fcode_dict, R_BOOLEAN printsorted, R_UINT *fcode_map, R_UINT *pos);

/* Splay the tree about node p. The new root (i.e. node p) is
** returned. The splay operation is described in Sleator and Tarjan,
** Journal of the ACM, July 1985.  */
static FCODETREE *splayFcode (FCODETREE *p) {
  FCODETREE *q, *g;

  q = p -> prnt;

  /* splay the tree about p */
  while (q != FCODETREENULL) {
    g = q -> prnt;
    if (p == q -> left) {
      if (g == FCODETREENULL)
        ROTATER (p,q);
      else if (q == g -> left) {
	ROTATER (q,g); ROTATER (p,q);
      }
      else {
	ROTATER (p,q); ROTATEL (p,g);
      }
    }
    else {
      if (g == FCODETREENULL)
        ROTATEL (p,q);
      else if (q == g -> rght) {
	ROTATEL (q,g); ROTATEL (p,q);
      }
      else {
	ROTATEL (p,q); ROTATER (p,g);
      }
    }
    q = p -> prnt;
  }

  return (p);
}


static void traverseFcodeDict (FCODETREE *t, FCODENODE *fcode_dict, R_BOOLEAN printsorted, R_UINT *fcode_map, R_UINT *pos) {
  /*  Word id #0 not used  */
  fcode_dict[0].item = NULL;
  fcode_dict[0].init_id = 0;
  fcode_dict[0].id = 0;

  if (t == (FCODETREE *) NULL) {
    return;
  }
  traverseFcodeDict (t -> left, fcode_dict, printsorted, fcode_map, pos);
  fcode_dict[*pos].item = t -> item;
  fcode_dict[*pos].len = t -> len;
  fcode_dict[*pos].freq = t -> freq;
  fcode_dict[*pos].init_id = t -> id;
  fcode_dict[*pos].id = *pos;
  fcode_map[t -> id] = *pos;
  (*pos)++;
  if (printsorted == R_TRUE) {
    printf ("%10u\t", t -> freq);
    uprintf (stderr, t -> item, t -> len);
    printf (" (%u)\n", t -> len);
  }
  traverseFcodeDict (t -> rght, fcode_dict, printsorted, fcode_map, pos);
}


/*
**  Process the item by inserting it into a splay tree (or updating the 
**  frequency if it already exists).  Return the item's id.
*/
R_UINT fcodeEncode (R_UCHAR *item, R_UINT len, FCODETREE **fcode_root, R_UINT *itemcount, R_UINT *item_compares, R_UINT *total_itemlen) {
  R_INT cmp = 0;
  FCODETREE *p, *q;
  R_UINT key = 0;

  if (len == 0) {
    return (EMPTY_FCODE);
  }

  /* standard search in a binary search tree... */
  p = *fcode_root;
  q = FCODETREENULL;
  while (p) {
    q = p;
    cmp = ustrncmp (item, len, p -> item, p -> len);
    (*item_compares)++;
    if (cmp < 0) {
      p = p -> left;
    }
    else {
      if (cmp > 0) {
        p = p -> rght;
      }
      else {
        (p -> freq)++;
        key = p -> id;
        break;
      }
    }
  }
  if (!p) {
    /* the search failed to find the item */
    /* make and fill a new tree node */
    p = wmalloc (sizeof (FCODETREE) * 1);
    p -> item = wmalloc (sizeof (R_UCHAR) * (size_t) len);
    ustrncpy (p -> item, item, len);
    p -> id = *itemcount;
    (*itemcount)++;
    key = p -> id;
    p -> freq = 1;
    p -> len = len;
    (*total_itemlen) += len;

    /* Link into tree, q is pointing at parent.
    ** If q is null, this is the first insertion
    */
    p -> prnt = q;
    p -> left = p -> rght = FCODETREENULL;
    if (q) {
      if (cmp < 0) {
        q -> left = p;
      }
      else {
        q -> rght = p;
      }
    }
    else {
	*(fcode_root) = p;
    }
  }

  *(fcode_root) = splayFcode (p);

  return (key);
}


void fcodeDictEncode (FILE_STRUCT *file_info, FCODETREE *fcode_root, FCODENODE *fcode_dict, R_UINT *fcode_map, R_BOOLEAN printsorted, R_UINT nitems, enum WORDTYPE type) {
  R_UINT curr = FIRST_FCODE;
  FILE *fp = NULL;
  R_UCHAR *buf = NULL;
  R_UCHAR *p = NULL;
  R_UCHAR *end = NULL;
  R_UINT pos = FIRST_FCODE;

  if (type == ISWORD) {
    fp = file_info -> wd_fp;
    buf = file_info -> wd_buf;
    p = file_info -> wd_p;
    end = file_info -> wd_end;
  }
  else {
    fp = file_info -> nwd_fp;
    buf = file_info -> nwd_buf;
    p = file_info -> nwd_p;
    end = file_info -> nwd_end;
  }

  /*  Traverse the wordtree and save it to an array  */
  traverseFcodeDict (fcode_root, fcode_dict, printsorted, fcode_map, &pos);

  /*  Do not encode word in position 0, the zero-length word  */
  while (curr < nitems) {
    *p = (R_UCHAR) fcode_dict[curr].len;
    (p)++;
    memcpy (p, fcode_dict[curr].item, (size_t) fcode_dict[curr].len);
    (p) += fcode_dict[curr].len;
    curr++;

    if (p > end) {
      (void) fwrite (buf, sizeof (R_UCHAR), (size_t) (p - buf), fp);
      p = buf;
    }
  }

  if (type == ISWORD) {
    file_info -> wd_p = p;
  }
  else {
    file_info -> nwd_p = p;
  }

  return;
}


R_UINT fcodeDictDecode (FILE_STRUCT *file_info, FCODENODE **fcode_dict, R_UINT nitems, enum WORDTYPE type) {
  R_UINT i = 0;
  R_UINT buffsize = 0;
  R_UINT diff;

  FILE *fp = NULL;
  R_UCHAR *buf = NULL;
  R_UCHAR *p = NULL;
  R_UCHAR *end = NULL;

  R_UINT init_nitems = nitems;

  if (type == ISWORD) {
    fp = file_info -> wd_fp;
    buf = file_info -> wd_buf;
    p = file_info -> wd_p;
    end = file_info -> wd_end;
  }
  else {
    fp = file_info -> nwd_fp;
    buf = file_info -> nwd_buf;
    p = file_info -> nwd_p;
    end = file_info -> nwd_end;
  }

  /*  The 0th word is the zero-length word  */
  (*fcode_dict)[0].item = NULL;
  (*fcode_dict)[0].len = 0;

  p = end;
  i = FIRST_FCODE;
  while (R_TRUE) {
    if (end - p < (MAXWORDLEN + MAXWORDLEN_HEADER)) {
      diff = (R_UINT) (end - p);
      memcpy (buf, p, (size_t) diff);
      buffsize = (R_UINT) fread (buf + diff, sizeof (R_UCHAR), (size_t) (OUTBUFMAX - diff), fp);
      buffsize += diff;
      end = buf + buffsize;
      p = buf;
    }
    if ((p == end) && (feof (fp) == R_TRUE)) {
      break;
    }

    (*fcode_dict)[i].len = (R_UINT) (*p);
    (p)++;
    (*fcode_dict)[i].item = wmalloc (sizeof (R_UCHAR) * ((*fcode_dict)[i].len));
    memcpy ((*fcode_dict)[i].item, p, (size_t) ((*fcode_dict)[i].len));
    (p) += (*fcode_dict)[i].len;
    i++;
    if (i == init_nitems) {
      init_nitems = init_nitems << 1;
      (*fcode_dict) = wrealloc ((*fcode_dict), init_nitems * sizeof (FCODENODE));
    }
  }

  if (type == ISWORD) {
    file_info -> wd_p = p;
  }
  else {
    file_info -> nwd_p = p;
  }

  return (i);
}
