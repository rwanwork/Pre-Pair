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
#include <limits.h>

#include "common-def.h"
#include "ustring.h"
#include "casefold.h"

R_UINT casefold (R_UCHAR *wrd, R_UINT wrd_len) {
  R_UINT modifier = 0;
  R_UINT i = 0;
  R_UINT count = 0;

  if (wrd_len > MAXCASEFOLDLEN) {
    fprintf (stderr, "casefold:  ");
    uprintf (stderr, wrd, wrd_len);
    fprintf (stderr, " (%u)\n", wrd_len);
    fprintf (stderr, "Word length out of range for case folding (%s, line %u).", __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }

  for (i = (wrd_len - 1); i != UINT_MAX; i--) {
    modifier = modifier << 1;
    if (isupper (wrd[i])) {
      count++;
      wrd[i] = (R_UCHAR) tolower (wrd[i]);
      modifier = modifier | 0x1;
    }
  }

  /*  Everything in uppercase, so set top bit  */
  if (count == wrd_len) {
    modifier = 0;
    modifier = ALL_CAPS;
  }
  return (modifier);
}


void uncasefold (R_UCHAR *wrd, R_UINT wrd_len, R_UINT modifier) {
  R_UINT i = 0;

  /*  Top bit set so everything is in uppercase  */
  if (modifier == ALL_CAPS) {
    for (i = 0; i < wrd_len; i++) {
      wrd[i] = (R_UCHAR) toupper (wrd[i]);
    }
    return;
  }

  for (i = 0; i < wrd_len; i++) {
    /*  Exit loop if no uncasefolding needs to be done  */
    if (modifier == 0) {
      break;
    }
    if ((modifier & 0x1) == 1) {
      wrd[i] = (R_UCHAR) toupper (wrd[i]);
    }
    modifier = modifier >> 1;
  }

  return;
}
