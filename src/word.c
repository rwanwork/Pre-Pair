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
#include <ctype.h>

#include "common-def.h"
#include "prepair-defn.h"
#include "fcode.h"
#include "word.h"

 
/* Read the next word from the indicated stream. Returns EOF if no word
** can be found; returns positive if a word is found. At most `lim'
** characters will be read into array w.  */
R_UINT getWord (R_UCHAR **src_p, R_UCHAR *src_end, R_UCHAR *w, R_UINT lim, R_BOOLEAN *notdone, R_UINT *long_tokens, R_UINT *enforce_tags) {
  R_UINT len = 0;
  R_UINT c = 0;
  R_UINT d = 0;
  R_UINT e = 0;

  *notdone = R_FALSE;
  do {
    if (*src_p == src_end) {
      *notdone = R_TRUE;
      break;
    }
    c = (R_UINT) **src_p;
    /*  If c isn't a word but is an apostrophe, then check the following
    **  two letters.  If the next letter is either a d, m, s, or t and
    **  the one after that is a nonword, then let it go.  Otherwise,
    **  break out of loop and reject c.  */
    if (!ISWORD (c)) {
      if (( c == (R_UINT) '\'') && (len <= (lim - 2))) {
	d = (R_UINT) (*(*src_p + 1));
        if ((d == (R_UINT) 'd') || (d == (R_UINT) 'm') || (d == (R_UINT) 's') || (d == (R_UINT) 't')) {
	  if (((*src_p + 2) == src_end) || (len == (lim - 2)) || (!ISWORD (*(*src_p + 2)))) {
	    /* ok */
	  }
	  else {
	    break;
	  }
	}
	else if (len <= (lim - 3)) {
	  e = (R_UINT) *(*src_p + 2);
	  if (((d == (R_UINT) 'l') && (e == (R_UINT) 'l')) ||
              ((e == (R_UINT) 'e') && ((d == (R_UINT) 'r') || (d == (R_UINT) 'v')))) {
	    if (((*src_p + 3) == src_end) || (len == lim - 3) || (!ISWORD (*(*src_p + 3)))) {
	      /*  ok  */
	    }
	    else {
	      break;
	    }
	  }
	  else {
	    break;
	  }
	}
	else {
	  break;
	}
      }
      else {
	break;
      }
    }
    len++;
    if (len > lim) {
      len--;
      *notdone = R_TRUE;
      (*long_tokens)++;
      break;
    }
    if ((len != 1) && (c == (R_UINT) '<')) {
      len--;
      *notdone = R_TRUE;
      (*enforce_tags)++;
      break;
    }
    (*src_p)++;

    *w = (R_UCHAR) c;
    w++;
    if (c == (R_UINT) '>') {
      if (*src_p != src_end) {
        d = (R_UINT) (*(*src_p));
        if (ISWORD (d)) {
	  (*enforce_tags)++;
        }
        break;
      }
    }
  } while (R_TRUE);

  return (len);
}
