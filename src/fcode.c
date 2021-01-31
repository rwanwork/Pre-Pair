#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#include "common-def.h"
#include "wmalloc.h"
#include "ustring.h"
#include "prepair-defn.h"
#include "fcode.h"

static FCODETREE *splayFcode (FCODETREE *p);
static void traverseFcodeDict (FCODETREE *t, FCODENODE *fcode_dict, bool printsorted, unsigned int *fcode_map, unsigned int *pos);

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


static void traverseFcodeDict (FCODETREE *t, FCODENODE *fcode_dict, bool printsorted, unsigned int *fcode_map, unsigned int *pos) {
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
  if (printsorted == true) {
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
unsigned int fcodeEncode (unsigned char *item, unsigned int len, FCODETREE **fcode_root, unsigned int *itemcount, unsigned int *item_compares, unsigned int *total_itemlen) {
  int cmp = 0;
  FCODETREE *p, *q;
  unsigned int key = 0;

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
    p -> item = wmalloc (sizeof (unsigned char) * (size_t) len);
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


void fcodeDictEncode (FILE_STRUCT *file_info, FCODETREE *fcode_root, FCODENODE *fcode_dict, unsigned int *fcode_map, bool printsorted, unsigned int nitems, enum WORDTYPE type) {
  unsigned int curr = FIRST_FCODE;
  FILE *fp = NULL;
  unsigned char *buf = NULL;
  unsigned char *p = NULL;
  unsigned char *end = NULL;
  unsigned int pos = FIRST_FCODE;

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
    *p = (unsigned char) fcode_dict[curr].len;
    (p)++;
    memcpy (p, fcode_dict[curr].item, (size_t) fcode_dict[curr].len);
    (p) += fcode_dict[curr].len;
    curr++;

    if (p > end) {
      (void) fwrite (buf, sizeof (unsigned char), (size_t) (p - buf), fp);
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


unsigned int fcodeDictDecode (FILE_STRUCT *file_info, FCODENODE **fcode_dict, unsigned int nitems, enum WORDTYPE type) {
  unsigned int i = 0;
  unsigned int buffsize = 0;
  unsigned int diff;

  FILE *fp = NULL;
  unsigned char *buf = NULL;
  unsigned char *p = NULL;
  unsigned char *end = NULL;

  unsigned int init_nitems = nitems;

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
  while (true) {
    if (end - p < (MAXWORDLEN + MAXWORDLEN_HEADER)) {
      diff = (unsigned int) (end - p);
      memcpy (buf, p, (size_t) diff);
      buffsize = (unsigned int) fread (buf + diff, sizeof (unsigned char), (size_t) (OUTBUFMAX - diff), fp);
      buffsize += diff;
      end = buf + buffsize;
      p = buf;
    }
    if ((p == end) && (feof (fp) == true)) {
      break;
    }

    (*fcode_dict)[i].len = (unsigned int) (*p);
    (p)++;
    (*fcode_dict)[i].item = wmalloc (sizeof (unsigned char) * ((*fcode_dict)[i].len));
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
