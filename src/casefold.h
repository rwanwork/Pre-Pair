#ifndef CASEFOLD_H
#define CASEFOLD_H

#define ALL_CAPS 0x10000                    /*  65,536 or (1 << 16)  */


/*  Maximum length of string that can be case-folded depends on
**  the size of an unsigned integer.  However, in word-based
**  Re-Pair, the maximum length with be far shorter  */
#define MAXCASEFOLDLEN 32

unsigned int casefold (unsigned char *wrd, unsigned int wrd_len);
void uncasefold (unsigned char *wrd, unsigned int wrd_len, unsigned int modifier);

#endif
