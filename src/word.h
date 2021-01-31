#ifndef WORD_H
#define WORD_H


typedef struct wordstruct {
  /*  General settings  */
  bool docasefold;                    /*  Perform case-folding  */
  unsigned int maxword;                      /*  Maximum length of a word  */
  bool dostem;                                  /*  Stem words  */

  unsigned int nwords;                                /*  Number of words  */
  unsigned int total_words_len;        /*  Length of all words in lexicon  */
  unsigned int nwords_prims;     /*  Number of words which are in Latin-1  */
  unsigned int cmps;               /*  Number of splay tree nodes visited  */
  unsigned int *map;           /*  Map used to re-encode sequence symbols  */

  /*  Statistics about parsing  */
  unsigned int total_tokens;
  unsigned int total_length;
  unsigned int long_tokens;
  unsigned int enforce_tags;
  unsigned int zerolength_sym;

  /*  Front-coding words  */
  FCODETREE *root_fc;
  FCODENODE *dict_fc;
  bool printsorted;            /*  Print words in sorted order  */
} WORD_STRUCT;


unsigned int getWord (unsigned char **src_p, unsigned char *src_end, unsigned char *w, unsigned int lim, bool *notdone, unsigned int *long_tokens, unsigned int *enforce_tags);

#endif

