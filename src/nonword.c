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
#include <ctype.h>
#include <limits.h>

#include "common-def.h"
#include "prepair-defn.h"
#include "fcode.h"
#include "nonword.h"

R_UINT getNonWord (R_UCHAR **src_p, R_UCHAR *src_end, R_UCHAR *w, R_UINT lim, R_BOOLEAN *notdone, R_UINT *long_tokens) {
  R_UINT len = 0;
  R_UINT c = 0;

  *notdone = R_FALSE;
  do {
    if (*src_p == src_end) {
      *notdone = R_TRUE;
      break;
    }
    c = (R_UINT) **src_p;
    if (ISWORD (c)) {
      break;
    }
    len++;
    if (len > lim) {
      len--;
      *notdone = R_TRUE;
      (*long_tokens)++;
      break;
    }
    (*src_p)++;

    *w = (R_UCHAR) c;
    w++;
  } while (R_TRUE);

  return (len);
}
