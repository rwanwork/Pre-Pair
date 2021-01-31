#include <stdlib.h>
#include <stdio.h>

#include "common-def.h"
#include "ustring.h"


unsigned int ustrlen (unsigned char *s) {
  unsigned int i = 0;

  for (i = 0; *s != (unsigned char) '\0'; s++) {
    i++;
  }

  return (i);
}


void ustrcpy (unsigned char *s, unsigned char *t) {
  while ((*s++ = *t++) != (unsigned char) '\0')
    ;
}


void ustrncpy (unsigned char *s, unsigned char *t, unsigned int n) {
  unsigned int i = 0;

  while (i < n) {
    i++;
    *s++ = *t++;
  }

  return;
}


void ustrcat (unsigned char *s, unsigned char *t) {
  unsigned int i, j;

  i = j = 0;
  while (s[i] != (unsigned char) '\0') {
    i++;
  }
  while ((s[i++] = t[j++]) != (unsigned char) '\0')
    ;
}


void ustrncat (unsigned char *s, unsigned char *t, unsigned int n) {
  unsigned int i, j;

  i = j = 0;
  while (s[i] != (unsigned char) '\0') {
    i++;
  }
  while ((j < n) && ((s[i++] = t[j++]) != (unsigned char) '\0'))
    ;
}


void uprintf (FILE *fp, unsigned char *wrd, unsigned int wrd_len) {
  unsigned int i = 0;

  for (i = 0; i < wrd_len; i++) {
    (void) putc ((int) wrd[i], fp);
  }

  return;
}


int ustrncmp (unsigned char *s, unsigned int s_len, unsigned char *t, unsigned int t_len) {
  unsigned int i = 0;
  unsigned int j = 0;

  if (s_len < t_len) {
    while ((i < s_len) && (s[i] == t[j])) {
      i++;
      j++;
    }
    if (i == s_len) {
      return ((int) s_len - (int) t_len);
    }
  }
  else if (s_len > t_len) {
    while ((j < t_len) && (s[i] == t[j])) {
      i++;
      j++;
    }
    if (j == t_len) {
      return ((int) s_len - (int) t_len);
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

  return ((int) s[i] - (int) t[j]);
}


void ustrcpy_const (unsigned char *s, const char *t) {
  while ((*s++ = *t++) != (unsigned char) '\0')
    ;
}


void ustrncat_const (unsigned char *s, const char *t, unsigned int n) {
  unsigned int i, j;

  i = j = 0;
  while (s[i] != (unsigned char) '\0') {
    i++;
  }
  while ((j < n) && ((s[i++] = t[j++]) != (unsigned char) '\0'))
    ;
}

