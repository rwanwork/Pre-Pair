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

#include <stdlib.h>
#include <stdio.h>

#include "common-def.h"
#include "ustring.h"


R_UINT ustrlen (R_UCHAR *s) {
  R_UINT i = 0;

  for (i = 0; *s != (R_UCHAR) '\0'; s++) {
    i++;
  }

  return (i);
}


void ustrcpy (R_UCHAR *s, R_UCHAR *t) {
  while ((*s++ = *t++) != (R_UCHAR) '\0')
    ;
}


void ustrncpy (R_UCHAR *s, R_UCHAR *t, R_UINT n) {
  R_UINT i = 0;

  while (i < n) {
    i++;
    *s++ = *t++;
  }

  return;
}


void ustrcat (R_UCHAR *s, R_UCHAR *t) {
  R_UINT i, j;

  i = j = 0;
  while (s[i] != (R_UCHAR) '\0') {
    i++;
  }
  while ((s[i++] = t[j++]) != (R_UCHAR) '\0')
    ;
}


void ustrncat (R_UCHAR *s, R_UCHAR *t, R_UINT n) {
  R_UINT i, j;

  i = j = 0;
  while (s[i] != (R_UCHAR) '\0') {
    i++;
  }
  while ((j < n) && ((s[i++] = t[j++]) != (R_UCHAR) '\0'))
    ;
}


void uprintf (FILE *fp, R_UCHAR *wrd, R_UINT wrd_len) {
  R_UINT i = 0;

  for (i = 0; i < wrd_len; i++) {
    (void) putc ((R_INT) wrd[i], fp);
  }

  return;
}


R_INT ustrncmp (R_UCHAR *s, R_UINT s_len, R_UCHAR *t, R_UINT t_len) {
  R_UINT i = 0;
  R_UINT j = 0;

  if (s_len < t_len) {
    while ((i < s_len) && (s[i] == t[j])) {
      i++;
      j++;
    }
    if (i == s_len) {
      return ((R_INT) s_len - (R_INT) t_len);
    }
  }
  else if (s_len > t_len) {
    while ((j < t_len) && (s[i] == t[j])) {
      i++;
      j++;
    }
    if (j == t_len) {
      return ((R_INT) s_len - (R_INT) t_len);
    }
  }
  else {
    while ((i < s_len) && (s[i] == t[j])) {
      i++;
      j++;
    }
    if (i == s_len) {
      return (0);
    }
  }

  return ((R_INT) s[i] - (R_INT) t[j]);
}
