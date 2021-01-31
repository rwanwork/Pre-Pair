#ifndef FCODE_H
#define FCODE_H

/*
**  Start front-code ids at 1 because and 0 is used for a 0 length 
**  front-code.
*/
#define EMPTY_FCODE 0
#define FIRST_FCODE 1

/*  If the lexicon is front-coded, the size is not written in
**  the file, so start it at this value and realloc if
**  necessary.  */
#define INIT_FCODE_SIZE 1024

#define FCODETREENULL  ((struct fcodetree *) NULL)
#define FILENULL  ((FILE *) NULL)
#define CHARNULL  ((char *) NULL)

typedef struct fcodetree {
  unsigned char *item;
  unsigned int len;
  unsigned int freq;
  unsigned int id;
  struct fcodetree *left, *rght, *prnt;
} FCODETREE;


typedef struct fcodenode {
  unsigned char *item;
  unsigned int len;
  unsigned int freq;
  unsigned int init_id;
  unsigned int id;
} FCODENODE;


#define LOOKUPFCODE(DICT,KEY,ITEM,LEN) \
  LEN = DICT[KEY].len; \
  ustrncpy (ITEM, DICT[KEY].item, LEN);
  
/* 
**  Rotations used in the splaying operations
*/
#define ROTATEL(p,q) \
  do { \
    p->prnt = q->prnt; \
    q->prnt = p; \
    q->rght = p->left; \
    if (q->rght != FCODETREENULL) \
      q->rght->prnt = q; \
    p->left = q; \
    if (p->prnt != FCODETREENULL) { \
      if (q == p->prnt->left) \
        p->prnt->left = p; \
      else \
        p->prnt->rght = p; \
    } \
  } while (0)

#define ROTATER(p,q) \
  do { \
    p->prnt = q->prnt; \
    q->prnt = p; \
    q->left = p->rght; \
    if (q->left != FCODETREENULL) \
      q->left->prnt = q; \
    p->rght = q; \
    if (p->prnt != FCODETREENULL) { \
      if (q == p->prnt->left) \
        p->prnt->left = p; \
      else \
        p->prnt->rght = p; \
    } \
  } while (0)

unsigned int fcodeEncode (unsigned char *item, unsigned int len, FCODETREE **fcode_root, unsigned int *itemcount, unsigned int *item_compares, unsigned int *total_itemlen);

void fcodeDictEncode (FILE_STRUCT *file_info, FCODETREE *fcode_root, FCODENODE *fcode_dict, unsigned int *fcode_map, bool printsorted, unsigned int nitems, enum WORDTYPE type);

unsigned int fcodeDictDecode (FILE_STRUCT *file_info, FCODENODE **fcode_dict, unsigned int nitems, enum WORDTYPE type);

#endif
