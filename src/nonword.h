#ifndef NONWORD_H
#define NONWORD_H

typedef struct nonwordstruct {
  /*  General settings  */
  unsigned int maxnonword;                /*  Maximum length of a nonword  */

  unsigned int nnonwords;                          /*  Number of nonwords  */
  unsigned int total_nonwords_len;     /*  Length of all words in lexicon  */
  unsigned int nnonwords_prims;  /*  Number of words which are in Latin-1  */
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
  bool printsorted;         /*  Print nonwords in sorted order  */
} NONWORD_STRUCT;


unsigned int getNonWord (unsigned char **src_p, unsigned char *src_end, unsigned char *w, unsigned int lim, bool *notdone, unsigned int *long_tokens);

#endif

