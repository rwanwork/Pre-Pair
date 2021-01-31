#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "common-def.h"
#include "ustring.h"
#include "casefold.h"

unsigned int casefold (unsigned char *wrd, unsigned int wrd_len) {
  unsigned int modifier = 0;
  unsigned int i = 0;
  unsigned int count = 0;

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
      wrd[i] = (unsigned char) tolower (wrd[i]);
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


void uncasefold (unsigned char *wrd, unsigned int wrd_len, unsigned int modifier) {
  unsigned int i = 0;

  /*  Top bit set so everything is in uppercase  */
  if (modifier == ALL_CAPS) {
    for (i = 0; i < wrd_len; i++) {
      wrd[i] = (unsigned char) toupper (wrd[i]);
    }
    return;
  }

  for (i = 0; i < wrd_len; i++) {
    /*  Exit loop if no uncasefolding needs to be done  */
    if (modifier == 0) {
      break;
    }
    if ((modifier & 0x1) == 1) {
      wrd[i] = (unsigned char) toupper (wrd[i]);
    }
    modifier = modifier >> 1;
  }

  return;
}
