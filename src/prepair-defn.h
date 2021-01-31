#ifndef PREPAIR_DEF_H
#define PREPAIR_DEF_H

#define UINT_SIZE_BITS 32

/*  Size of file buffers (for input or output)  */
#define OUTBUFMAX       1048576

#define MAXPRIMS 256
/*  A user can specify the maximum length of a word at the command 
**  line. The length, maxword, must satisfy:  
**  WORDLEN <= maxword <= MAXWORDLEN  */
#define WORDLEN 12
#define MAXWORDLEN 16

/*  The header for a word in the word dictionary contains a
**  one byte match/length header (4 bits each) and, for appended
**  words, an additional 4 bytes to reference the longest match
**  in the dictionary.  */
#define MAXWORDLEN_HEADER  5

#define ISWORD(C)  ((isalnum ((int) C)) || ((unsigned int) C == (unsigned int) '<') || ((unsigned int) C == (unsigned int) '>') || ((unsigned int) C == (unsigned int) '/'))
enum WORDTYPE { ISWORD = 0, ISNONWORD = 1 };

enum PROGMODE { MODE_NONE = 0, MODE_ENCODE = 1, MODE_DECODE = 2, MODE_DECODE_NONE = 3, MODE_DECODE_LINK = 4 };

typedef struct filestruct {
  /*  Word dictionary, extension ".wd"  */
  unsigned char *wd_name;
  FILE *wd_fp;
  unsigned char *wd_buf;
  unsigned char *wd_p;
  unsigned char *wd_end;

  /*  Word sequence, extension ".ws"  */
  unsigned char *ws_name;
  FILE *ws_fp;
  unsigned int *ws_buf;
  unsigned int *ws_p;
  unsigned int *ws_end;

  /*  Non-word dictionary (phrase hierarchy), extension ".nwd"  */
  unsigned char *nwd_name;
  FILE *nwd_fp;
  unsigned char *nwd_buf;
  unsigned char *nwd_p;
  unsigned char *nwd_end;

  /*  Non-word sequence, extension ".nws"  */
  unsigned char *nws_name;
  FILE *nws_fp;
  unsigned int *nws_buf;
  unsigned int *nws_p;
  unsigned int *nws_end;

  /*  Case-folding modifiers, extension ".cfm"  */
  unsigned char *cfm_name;
  FILE *cfm_fp;
  unsigned int *cfm_buf;
  unsigned int *cfm_p;
  unsigned int *cfm_end;

  /*  Stemming modifiers, extension ".sm"  */
  unsigned char *sm_name;
  FILE *sm_fp;
  unsigned int *sm_buf;
  unsigned int *sm_p;
  unsigned int *sm_end;

  bool verbose_level;
  enum PROGMODE mode;
} FILE_STRUCT;

#endif
