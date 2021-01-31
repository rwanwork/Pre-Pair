/*    Pre-Pair
**    Word-based Pre-processor for Re-Pair
**    Copyright (C) 2003, 2007 by Raymond Wan (rwan@kuicr.kyoto-u.ac.jp)
**
**    Version 1.0.1 -- 2007/04/02
**
**    This file is part of the Pre-Pair software.
**
**    Pre-Pair is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    Pre-Pair is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License along
**    with Pre-Pair; if not, write to the Free Software Foundation, Inc.,
**    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef PREPAIR_H
#define PREPAIR_H

#define GEN_LOOKUP_SIZE 1024          /*  Initial lookup table size  */

#define TOP_BIT 0x80000000      /*  The top bit in a 32-bit integer  */
#define NO_TOP_BIT 0x7FFFFFFF        /*  Everything but the top bit  */

#define MIN_BUFF_SIZE (3 * word_info -> maxword)
#define INIT_BUFF_SIZE 1048576

/*  Initialisation  */
void initPrepair (WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info, R_UINT maxword, R_BOOLEAN docasefold, R_BOOLEAN dostem, R_BOOLEAN printsorted);

/*  Read from and write to sequences  */
void writeFiles (FILE_STRUCT *file_info, R_UINT wrd_key, R_UINT casefold_result, R_UINT stem_result, R_UINT nonwrd_key);
R_UINT readFiles (FILE_STRUCT *file_info, R_UINT *wrd_key, R_UINT *casefold_result, R_UINT *stem_result, R_UINT *nonwrd_key);

/*  Manage files  */
void openFiles (R_UCHAR *filename, FILE_STRUCT *file_info, const R_CHAR *filemode, R_BOOLEAN dicts_only);
void seqReEncode (FILE_STRUCT *file_info, R_UINT *map, enum WORDTYPE type);
void closeFilesEncode (FILE_STRUCT *file_info, R_UINT *word_map, R_UINT *nonword_map);
void closeFilesDecode (FILE_STRUCT *file_info, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info);

/*  Main encoding/decoding functions  */
void fileEncode (FILE_STRUCT *file_info, FILE *fp, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info);
void fileDecode (FILE_STRUCT *file_info, FILE *fp, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info);

#endif
