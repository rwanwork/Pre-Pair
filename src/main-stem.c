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
#include <string.h>
#include <limits.h>

#include "common-def.h"
#include "wmalloc.h"
#include "ustring.h"
#include "casefold.h"
#include "stem.h"

static void usage (R_CHAR *progname);

static void usage (R_CHAR *progname) {
  fprintf (stderr, "Stem/Case-fold test program\n");
  fprintf (stderr, "===========================\n\n");
  fprintf (stderr, "Usage:  %s <input >output\n", progname);
  fprintf (stderr, "  or\n");
  fprintf (stderr, "        %s <word> >output\n\n", progname);
  fprintf (stderr, "Either apply stemming and case-folding to a document\n");
  fprintf (stderr, "given via stdin, or stem one word at the command line\n");
  fprintf (stderr, "If the first case is chosen, then something is sent\n");
  fprintf (stderr, "to stdout only if a difference occurs between encoding\n");
  fprintf (stderr, "and decoding.  If the second case is chosen, output\n");
  fprintf (stderr, "is always provided.  Words must not be longer than\n");
  fprintf (stderr, "MAXSTEMLEN characters in length.\n\n");
  fprintf (stderr, "This message is displayed if only either the option\n");
  fprintf (stderr, "-? or -h is given.\n");
  fprintf (stderr, "Stem/Case-fold tester version:  %s (%s)\n\n", __DATE__, __TIME__);
  exit (EXIT_SUCCESS);
}

R_INT main (R_INT argc, R_CHAR *argv[]) {
  R_UCHAR *curr;
  R_UCHAR *initial;
  R_UCHAR *final;
  R_UINT *m;
  R_UINT modifier = 0;
  R_UINT case_modifier = 0;
  R_UINT len = 0;
  R_UINT old_len = 0;

  R_UCHAR *temp;

  initial = wmalloc (sizeof (R_UCHAR) * MAXSTEMLEN);
  final = wmalloc (sizeof (R_UCHAR) * MAXSTEMLEN);
  curr = wmalloc (sizeof (R_UCHAR) * MAXSTEMLEN);
  m = wmalloc (sizeof (R_UINT) * MAXSTEMLEN);
  temp = wmalloc (sizeof (R_UCHAR) * MAXSTEMLEN);

  if ((argc == 2) && ((strcmp ("-?", argv[1]) == 0) || (strcmp ("-h", argv[1]) == 0))) {
    usage (argv[0]);
  }

  if (argc != 2) {
    while (fscanf (stdin, "%255s", (R_CHAR*) initial) != EOF) {
      curr = (R_UCHAR*) strcpy ((R_CHAR*) curr, (R_CHAR*) initial);
      old_len = (R_UINT) strlen ((R_CHAR*) curr);
      case_modifier = casefold (curr, old_len);
      modifier = stem (curr, &old_len, m);
      curr[old_len] = (R_UCHAR) '\0';
      ustrncpy (temp, curr, old_len);
      temp[old_len] = (R_UCHAR) '\0';
      len = unstem (curr, old_len, modifier);
      curr[len] = (R_UCHAR) '\0';
      uncasefold (curr, len, case_modifier);
      ustrncpy (final, curr, len);
      final[len] = (R_UCHAR) '\0';
      if (strcmp ((R_CHAR*) initial, (R_CHAR*) final) != 0) {
        fprintf (stderr, "%s --%s[%u, %u]--> %s\n", (R_CHAR*) initial, (R_CHAR*) temp, modifier, case_modifier, (R_CHAR*) final);
      }
    }
  }
  else {
    fprintf (stderr, "%s\t", argv[1]);
    curr = (R_UCHAR*) strcpy ((R_CHAR*) curr, argv[1]);
    old_len = (R_UINT) strlen ((R_CHAR*) curr);
    case_modifier = casefold (curr, old_len);
    modifier = stem (curr, &old_len, m);
    curr[old_len] = (R_UCHAR) '\0';
    ustrncpy (temp, curr, old_len);
    temp[old_len] = (R_UCHAR) '\0';
    len = unstem (curr, old_len, modifier);
    uncasefold (curr, len, case_modifier);
    ustrncpy (final, curr, len);
    final[len] = (R_UCHAR) '\0';
    fprintf (stderr, "%s --%s[%u, %u]--> %s\n", (R_CHAR*) initial, (R_CHAR*) temp, modifier, case_modifier, (R_CHAR*) final);
  }

  free (m);
  free (temp);
  free (curr);
  free (final);
  free (initial);

  return (EXIT_SUCCESS);
}
