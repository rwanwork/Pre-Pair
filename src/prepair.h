#ifndef PREPAIR_H
#define PREPAIR_H

#define GEN_LOOKUP_SIZE 1024          /*  Initial lookup table size  */

#define TOP_BIT 0x80000000      /*  The top bit in a 32-bit integer  */
#define NO_TOP_BIT 0x7FFFFFFF        /*  Everything but the top bit  */

#define MIN_BUFF_SIZE (3 * word_info -> maxword)
#define INIT_BUFF_SIZE 1048576

/*  Initialisation  */
void initPrepair (WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info, unsigned int maxword, bool docasefold, bool dostem, bool printsorted);

/*  Read from and write to sequences  */
void writeFiles (FILE_STRUCT *file_info, unsigned int wrd_key, unsigned int casefold_result, unsigned int stem_result, unsigned int nonwrd_key);
unsigned int readFiles (FILE_STRUCT *file_info, unsigned int *wrd_key, unsigned int *casefold_result, unsigned int *stem_result, unsigned int *nonwrd_key);

/*  Manage files  */
void openFiles (unsigned char *filename, FILE_STRUCT *file_info, const char *filemode, bool dicts_only);
void seqReEncode (FILE_STRUCT *file_info, unsigned int *map, enum WORDTYPE type);
void closeFilesEncode (FILE_STRUCT *file_info, unsigned int *word_map, unsigned int *nonword_map);
void closeFilesDecode (FILE_STRUCT *file_info, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info);

/*  Main encoding/decoding functions  */
void fileEncode (FILE_STRUCT *file_info, FILE *fp, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info);
void fileDecode (FILE_STRUCT *file_info, FILE *fp, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info);

#endif
