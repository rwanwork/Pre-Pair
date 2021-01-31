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

#ifndef USTRING_H
#define USTRING_H

R_UINT ustrlen (R_UCHAR *s);
void ustrcpy (R_UCHAR *s, R_UCHAR *t);
void ustrncpy (R_UCHAR *s, R_UCHAR *t, R_UINT n);
void ustrcat (R_UCHAR *s, R_UCHAR *t);
void ustrncat (R_UCHAR *s, R_UCHAR *t, R_UINT n);
void uprintf (FILE *fp, R_UCHAR *wrd, R_UINT wrd_len);
R_INT ustrncmp (R_UCHAR *s, R_UINT s_len, R_UCHAR *t, R_UINT t_len);

#endif



