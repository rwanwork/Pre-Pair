#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "common-def.h"
#include "prepair-defn.h"
#include "fcode.h"
#include "word.h"


/* Read the next word from the indicated stream. Returns EOF if no word
** can be found; returns positive if a word is found. At most `lim'
** characters will be read into array w.  */
unsigned int getWord (unsigned char **src_p, unsigned char *src_end, unsigned char *w, unsigned int lim, bool *notdone, unsigned int *long_tokens, unsigned int *enforce_tags) {
  unsigned int len = 0;
  unsigned int c = 0;
  unsigned int d = 0;
  unsigned int e = 0;

  *notdone = false;
  do {
    if (*src_p == src_end) {
      *notdone = true;
      break;
    }
    c = (unsigned int) **src_p;
    /*  If c isn't a word but is an apostrophe, then check the following
    **  two letters.  If the next letter is either a d, m, s, or t and
    **  the one after that is a nonword, then let it go.  Otherwise,
    **  break out of loop and reject c.  */
    if (!ISWORD (c)) {
      if (( c == (unsigned int) '\'') && (len <= (lim - 2))) {
  d = (unsigned int) (*(*src_p + 1));
        if ((d == (unsigned int) 'd') || (d == (unsigned int) 'm') || (d == (unsigned int) 's') || (d == (unsigned int) 't')) {
    if (((*src_p + 2) == src_end) || (len == (lim - 2)) || (!ISWORD (*(*src_p + 2)))) {
      /* ok */
    }
    else {
      break;
    }
  }
  else if (len <= (lim - 3)) {
    e = (unsigned int) *(*src_p + 2);
    if (((d == (unsigned int) 'l') && (e == (unsigned int) 'l')) ||
              ((e == (unsigned int) 'e') && ((d == (unsigned int) 'r') || (d == (unsigned int) 'v')))) {
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
      *notdone = true;
      (*long_tokens)++;
      break;
    }
    if ((len != 1) && (c == (unsigned int) '<')) {
      len--;
      *notdone = true;
      (*enforce_tags)++;
      break;
    }
    (*src_p)++;

    *w = (unsigned char) c;
    w++;
    if (c == (unsigned int) '>') {
      if (*src_p != src_end) {
        d = (unsigned int) (*(*src_p));
        if (ISWORD (d)) {
    (*enforce_tags)++;
        }
        break;
      }
    }
  } while (true);

  return (len);
}
