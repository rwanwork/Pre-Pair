#ifndef USTRING_H
#define USTRING_H

unsigned int ustrlen (unsigned char *s);
void ustrcpy (unsigned char *s, unsigned char *t);
void ustrncpy (unsigned char *s, unsigned char *t, unsigned int n);
void ustrcat (unsigned char *s, unsigned char *t);
void ustrncat (unsigned char *s, unsigned char *t, unsigned int n);
void uprintf (FILE *fp, unsigned char *wrd, unsigned int wrd_len);
int ustrncmp (unsigned char *s, unsigned int s_len, unsigned char *t, unsigned int t_len);

/*  Functions with the second argument as constants  */
void ustrcpy_const (unsigned char *s, const char *t);
void ustrncat_const (unsigned char *s, const char *t, unsigned int n);

#endif



