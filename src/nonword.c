#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#include "common-def.h"
#include "prepair-defn.h"
#include "fcode.h"
#include "nonword.h"

unsigned int getNonWord (unsigned char **src_p, unsigned char *src_end, unsigned char *w, unsigned int lim, bool *notdone, unsigned int *long_tokens) {
  unsigned int len = 0;
  unsigned int c = 0;

  *notdone = false;
  do {
    if (*src_p == src_end) {
      *notdone = true;
      break;
    }
    c = (unsigned int) **src_p;
    if (ISWORD (c)) {
      break;
    }
    len++;
    if (len > lim) {
      len--;
      *notdone = true;
      (*long_tokens)++;
      break;
    }
    (*src_p)++;

    *w = (unsigned char) c;
    w++;
  } while (true);

  return (len);
}
