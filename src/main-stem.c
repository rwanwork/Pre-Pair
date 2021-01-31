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

/*  Pull the configuration file in  */
#include "PrePairConfig.h"

static void usage (char *progname);

static void usage (char *progname) {
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
  fprintf (stderr, "-? or -h is given.\n\n");
  
  fprintf (stderr, "Stem/Case-fold tester version %u.%u\n", PrePair_VERSION_MAJOR, PrePair_VERSION_MINOR);
  fprintf (stderr, "Compiled on:  %s (%s)\n\n", __DATE__, __TIME__);
  exit (EXIT_SUCCESS);
}

int main (int argc, char *argv[]) {
  unsigned char *curr;
  unsigned char *initial;
  unsigned char *final;
  unsigned int *m;
  unsigned int modifier = 0;
  unsigned int case_modifier = 0;
  unsigned int len = 0;
  unsigned int old_len = 0;

  unsigned char *temp;

  initial = wmalloc (sizeof (unsigned char) * MAXSTEMLEN);
  final = wmalloc (sizeof (unsigned char) * MAXSTEMLEN);
  curr = wmalloc (sizeof (unsigned char) * MAXSTEMLEN);
  m = wmalloc (sizeof (unsigned int) * MAXSTEMLEN);
  temp = wmalloc (sizeof (unsigned char) * MAXSTEMLEN);

  if ((argc == 2) && ((strcmp ("-?", argv[1]) == 0) || (strcmp ("-h", argv[1]) == 0))) {
    usage (argv[0]);
  }

  if (argc != 2) {
    while (fscanf (stdin, "%255s", (char*) initial) != EOF) {
      curr = (unsigned char*) strcpy ((char*) curr, (char*) initial);
      old_len = (unsigned int) strlen ((char*) curr);
      case_modifier = casefold (curr, old_len);
      modifier = stem (curr, &old_len, m);
      curr[old_len] = (unsigned char) '\0';
      ustrncpy (temp, curr, old_len);
      temp[old_len] = (unsigned char) '\0';
      len = unstem (curr, old_len, modifier);
      curr[len] = (unsigned char) '\0';
      uncasefold (curr, len, case_modifier);
      ustrncpy (final, curr, len);
      final[len] = (unsigned char) '\0';
      if (strcmp ((char*) initial, (char*) final) != 0) {
        fprintf (stderr, "%s --%s[%u, %u]--> %s\n", (char*) initial, (char*) temp, modifier, case_modifier, (char*) final);
      }
    }
  }
  else {
    fprintf (stderr, "%s\t", argv[1]);
    curr = (unsigned char*) strcpy ((char*) curr, argv[1]);
    old_len = (unsigned int) strlen ((char*) curr);
    case_modifier = casefold (curr, old_len);
    modifier = stem (curr, &old_len, m);
    curr[old_len] = (unsigned char) '\0';
    ustrncpy (temp, curr, old_len);
    temp[old_len] = (unsigned char) '\0';
    len = unstem (curr, old_len, modifier);
    uncasefold (curr, len, case_modifier);
    ustrncpy (final, curr, len);
    final[len] = (unsigned char) '\0';
    fprintf (stderr, "%s --%s[%u, %u]--> %s\n", (char*) initial, (char*) temp, modifier, case_modifier, (char*) final);
  }

  free (m);
  free (temp);
  free (curr);
  free (final);
  free (initial);

  return (EXIT_SUCCESS);
}
