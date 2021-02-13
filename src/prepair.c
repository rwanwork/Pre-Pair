/* Counts word frequencies.
   Uses a splay tree to record words. Frequencies are printed at end of input.

   Hacked from wrdint.
   Written by Alistair Moffat, March 1990.

   Updated by Raymond Wan [2000/07/19]
     - Function "isword" added.
     - Function "getword" modified.
     - A word is any alphanumeric string as well as '<', '/', and '>'.
         - The '/' character is only part of a word if it is preceeded
           by a '<'.
         - Also, the '<' character is part of a word only if it starts a
           word.
         - The '>' is part of a word only if the character before it is
           part of a word.
    Updated by Raymond Wan [2000/07/28] (changes removed)
    Updated by Raymond Wan [2002/10/30]
      - Modified to parse a document into words and non-words with
        optional case folding and stemming using the Porter stemming
        algorithm.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#include "common-def.h"
#include "wmalloc.h"
#include "ustring.h"
#include "prepair-defn.h"
#include "casefold.h"
#include "stem.h"
#include "fcode.h"
#include "word.h"
#include "nonword.h"
#include "prepair.h"

/*  Initialise word and nonword data structures  */
void initPrepair (WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info, unsigned int maxword, bool docasefold, bool dostem, bool printsorted) {
  /*  Word data structure  */
  word_info -> maxword = maxword;
  word_info -> docasefold = docasefold;
  word_info -> dostem = dostem;

  word_info -> nwords = 0;
  word_info -> total_words_len = 0;
  word_info -> nwords_prims = 0;
  word_info -> cmps = 0;
  word_info -> map = NULL;

  word_info -> total_tokens = 0;
  word_info -> total_length = 0;
  word_info -> long_tokens = 0;
  word_info -> enforce_tags = 0;
  word_info -> zerolength_sym = 0;

  word_info -> root_fc = NULL;
  word_info -> dict_fc = NULL;
  word_info -> printsorted = printsorted;

  word_info -> nwords = FIRST_FCODE;

  /*  Nonword data structure  */
  nonword_info -> maxnonword = maxword;

  nonword_info -> nnonwords = 0;
  nonword_info -> total_nonwords_len = 0;
  nonword_info -> nnonwords_prims = 0;
  nonword_info -> cmps = 0;
  nonword_info -> map = NULL;

  nonword_info -> total_tokens = 0;
  nonword_info -> total_length = 0;
  nonword_info -> long_tokens = 0;
  nonword_info -> enforce_tags = 0;
  nonword_info -> zerolength_sym = 0;

  nonword_info -> root_fc = NULL;
  nonword_info -> dict_fc = NULL;
  nonword_info -> printsorted = printsorted;

  nonword_info -> nnonwords = FIRST_FCODE;

  return;
}


void writeFiles (FILE_STRUCT *file_info, unsigned int wrd_key, unsigned int casefold_result, unsigned int stem_result, unsigned int nonwrd_key) {

  *file_info -> ws_p = wrd_key;
  *file_info -> cfm_p = casefold_result;
  *file_info -> sm_p = stem_result;
  *file_info -> nws_p = nonwrd_key;

  (file_info -> ws_p)++;
  (file_info -> cfm_p)++;
  (file_info -> sm_p)++;
  (file_info -> nws_p)++;

  /*  If one buffer is at the end, then all are  */
  if (file_info -> ws_p == file_info -> ws_end) {
    fwrite (file_info -> ws_buf, sizeof (unsigned int), (file_info -> ws_p) - (file_info -> ws_buf), file_info -> ws_fp);
    file_info -> ws_p = file_info -> ws_buf;

    fwrite (file_info -> cfm_buf, sizeof (unsigned int), (file_info -> cfm_p) - (file_info -> cfm_buf), file_info -> cfm_fp);
    file_info -> cfm_p = file_info -> cfm_buf;

    fwrite (file_info -> sm_buf, sizeof (unsigned int), (file_info -> sm_p) - (file_info -> sm_buf), file_info -> sm_fp);
    file_info -> sm_p = file_info -> sm_buf;

    fwrite (file_info -> nws_buf, sizeof (unsigned int), (file_info -> nws_p) - (file_info -> nws_buf), file_info -> nws_fp);
    file_info -> nws_p = file_info -> nws_buf;
  }

  return;
}


unsigned int readFiles (FILE_STRUCT *file_info, unsigned int *wrd_key, unsigned int *casefold_result, unsigned int *stem_result, unsigned int *nonwrd_key) {
  unsigned int intsread = 0;

  if (file_info -> ws_p == file_info -> ws_end) {
    if (feof (file_info -> ws_fp)) {
      return (0);
    }

    intsread = fread (file_info -> ws_buf, sizeof (unsigned int), OUTBUFMAX, file_info -> ws_fp);
    file_info -> ws_end = file_info -> ws_buf + intsread;
    file_info -> ws_p = file_info -> ws_buf;

    if (intsread != fread (file_info -> cfm_buf, sizeof (unsigned int), OUTBUFMAX, file_info -> cfm_fp)) {
      fprintf (stderr, "Case-folding modifier file size mismatch (%s, line %u).", __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
    file_info -> cfm_end = file_info -> cfm_buf + intsread;
    file_info -> cfm_p = file_info -> cfm_buf;

    if (intsread != fread (file_info -> sm_buf, sizeof (unsigned int), OUTBUFMAX, file_info -> sm_fp)) {
      fprintf (stderr, "Stemming modifier file size mismatch (%s, line %u).", __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
    file_info -> sm_end = file_info -> sm_buf + intsread;
    file_info -> sm_p = file_info -> sm_buf;

    if (intsread != fread (file_info -> nws_buf, sizeof (unsigned int), OUTBUFMAX, file_info -> nws_fp)) {
      fprintf (stderr, "Non-word sequence file size mismatch (%s, line %u).", __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
    file_info -> nws_end = file_info -> nws_buf + intsread;
    file_info -> nws_p = file_info -> nws_buf;
  }

  *wrd_key = *file_info -> ws_p;
#ifdef FLAG_WORDS
  /*  Remove the top bit for all word keys  */
  *wrd_key = *wrd_key & NO_TOP_BIT;
#endif
  *casefold_result = *file_info -> cfm_p;
  *stem_result = *file_info -> sm_p;
  *nonwrd_key = *file_info -> nws_p;

  (file_info -> ws_p)++;
  (file_info -> cfm_p)++;
  (file_info -> sm_p)++;
  (file_info -> nws_p)++;
  intsread = 1;

  return (intsread);
}


void openFiles (unsigned char *filename, FILE_STRUCT *file_info, const char *filemode, bool dicts_only) {
  unsigned int len = ustrlen (filename);

  file_info -> wd_name = wmalloc (sizeof (unsigned char) * (len + 1 + 3));
  ustrcpy (file_info -> wd_name, filename);
  ustrncat_const (file_info -> wd_name, ".wd", 3);
  file_info -> wd_name[len + 3] = '\0';
  FOPEN (file_info -> wd_name, file_info -> wd_fp, filemode);
  file_info -> wd_buf = wmalloc (sizeof (unsigned char) * OUTBUFMAX);
  /*  Mark the end of the buffer (MAXWORDLEN + MAXWORDLEN_HEADER + 1)
  **  from the actual end.  */
  file_info -> wd_end = file_info -> wd_buf + OUTBUFMAX - (MAXWORDLEN + MAXWORDLEN_HEADER + 1);

  if (dicts_only == false) {
    file_info -> ws_name = wmalloc (sizeof (unsigned char) * (len + 1 + 3));
    ustrcpy (file_info -> ws_name, filename);
    ustrncat_const (file_info -> ws_name, ".ws", 3);
    file_info -> ws_name[len + 3] = '\0';
    FOPEN (file_info -> ws_name, file_info -> ws_fp, filemode);
    file_info -> ws_buf = wmalloc (sizeof (unsigned int) * OUTBUFMAX);
    file_info -> ws_end = file_info -> ws_buf + OUTBUFMAX;
  }

  file_info -> nwd_name = wmalloc (sizeof (unsigned char) * (len + 1 + 4));
  ustrcpy (file_info -> nwd_name, filename);
  ustrncat_const (file_info -> nwd_name, ".nwd", 4);
  file_info -> nwd_name[len + 4] = '\0';
  FOPEN (file_info -> nwd_name, file_info -> nwd_fp, filemode);
  file_info -> nwd_buf = wmalloc (sizeof (unsigned char) * OUTBUFMAX);
  /*  Mark the end of the buffer (MAXWORDLEN + MAXWORDLEN_HEADER + 1)
  **  from the actual end.  */
  file_info -> nwd_end = file_info -> nwd_buf + OUTBUFMAX - (MAXWORDLEN + MAXWORDLEN_HEADER + 1);

  if (dicts_only == false) {
    file_info -> nws_name = wmalloc (sizeof (unsigned char) * (len + 1 + 4));
    ustrcpy (file_info -> nws_name, filename);
    ustrncat_const (file_info -> nws_name, ".nws", 4);
    file_info -> nws_name[len + 4] = '\0';
    FOPEN (file_info -> nws_name, file_info -> nws_fp, filemode);
    file_info -> nws_buf = wmalloc (sizeof (unsigned int) * OUTBUFMAX);
    file_info -> nws_end = file_info -> nws_buf + OUTBUFMAX;

    file_info -> cfm_name = wmalloc (sizeof (unsigned char) * (len + 1 + 4));
    ustrcpy (file_info -> cfm_name, filename);
    ustrncat_const (file_info -> cfm_name, ".cfm", 4);
    file_info -> cfm_name[len + 4] = '\0';
    FOPEN (file_info -> cfm_name, file_info -> cfm_fp, filemode);
    file_info -> cfm_buf = wmalloc (sizeof (unsigned int) * OUTBUFMAX);
    file_info -> cfm_end = file_info -> cfm_buf + OUTBUFMAX;

    file_info -> sm_name = wmalloc (sizeof (unsigned char) * (len + 1 + 3));
    ustrcpy (file_info -> sm_name, filename);
    ustrncat_const (file_info -> sm_name, ".sm", 3);
    file_info -> sm_name[len + 3] = '\0';
    FOPEN (file_info -> sm_name, file_info -> sm_fp, filemode);
    file_info -> sm_buf = wmalloc (sizeof (unsigned int) * OUTBUFMAX);
    file_info -> sm_end = file_info -> sm_buf + OUTBUFMAX;
  }

  if (strcmp (filemode, "w") == 0) {
    file_info -> wd_p = file_info -> wd_buf;
    file_info -> ws_p = file_info -> ws_buf;
    file_info -> nwd_p = file_info -> nwd_buf;
    file_info -> nws_p = file_info -> nws_buf;
    file_info -> cfm_p = file_info -> cfm_buf;
    file_info -> sm_p = file_info -> sm_buf;
  }
  else {
    file_info -> wd_p = file_info -> wd_end;
    file_info -> ws_p = file_info -> ws_end;
    file_info -> nwd_p = file_info -> nwd_end;
    file_info -> nws_p = file_info -> nws_end;
    file_info -> cfm_p = file_info -> cfm_end;
    file_info -> sm_p = file_info -> sm_end;
  }

  return;
}


/*  Re-encode the sequence file.  */
void seqReEncode (FILE_STRUCT *file_info, unsigned int *map, enum WORDTYPE type) {
  char *temp_mv;
  unsigned char *temp_file;
  FILE *temp_fp;
  unsigned int buffsize;
  unsigned int i;

  unsigned char *name;
  FILE *fp;
  unsigned int *buf = NULL;
  unsigned int *p = NULL;

  int result = 0;

  temp_file = wmalloc (sizeof (unsigned char) * 9);
  map[0] = 0;
  if (type == ISWORD) {
    name = file_info -> ws_name;
    fp = file_info -> ws_fp;
    buf = file_info -> ws_buf;
    ustrcpy_const (temp_file, "temp.ws");
    temp_file[7] = '\0';
  }
  else {
    name = file_info -> nws_name;
    fp = file_info -> nws_fp;
    buf = file_info -> nws_buf;
    ustrcpy_const (temp_file, "temp.nws");
    temp_file[8] = '\0';
  }

  temp_mv = wmalloc (ustrlen (name) + ustrlen (temp_file) + 5);
  sprintf (temp_mv, "mv %s %s", name, temp_file);
  result = system (temp_mv);
  if (result == -1) {
    fprintf (stderr, "EE\tError in execution of the 'mv' command!");
  }

  FOPEN (temp_file, temp_fp, "r");
  FOPEN (name, fp, "w");
  do {
    buffsize = fread (buf, sizeof (unsigned int), OUTBUFMAX, temp_fp);
    p = buf;
#ifdef FLAG_WORDS
    for (i = 0; i < buffsize; i++) {
      if ((*p & NO_TOP_BIT) == *p) {
  /*  top bit not set  */
  *p = map[*p];
      }
      else {
  /*  top bit is set  */
  *p = map[*p & NO_TOP_BIT];
  *p = *p | TOP_BIT;
      }
      p++;
    }
#else
    for (i = 0; i < buffsize; i++) {
      *p = map[*p];
      p++;
    }
#endif
    fwrite (buf, sizeof (unsigned int), buffsize, fp);
  } while (!feof (temp_fp));
  fclose (temp_fp);

  sprintf (temp_mv, "rm %s", temp_file);
  result = system (temp_mv);
  if (result == -1) {
    fprintf (stderr, "EE\tError in execution of the 'mv' command!");
  }

  wfree (temp_mv);
  fclose (fp);

  return;
}


void closeFilesEncode (FILE_STRUCT *file_info, unsigned int *word_map, unsigned int *nonword_map) {

  if (file_info -> wd_p != file_info -> wd_buf) {
    fwrite (file_info -> wd_buf, sizeof (unsigned char), (file_info -> wd_p) - (file_info -> wd_buf), file_info -> wd_fp);
  }
  wfree (file_info -> wd_buf);
  fclose (file_info -> wd_fp);
  wfree (file_info -> wd_name);

  if (file_info -> ws_p != file_info -> ws_buf) {
    fwrite (file_info -> ws_buf, sizeof (unsigned int), (file_info -> ws_p) - (file_info -> ws_buf), file_info -> ws_fp);
  }
  fclose (file_info -> ws_fp);
  seqReEncode (file_info, word_map, ISWORD);
  wfree (file_info -> ws_name);
  wfree (file_info -> ws_buf);

  if (file_info -> nwd_p != file_info -> nwd_buf) {
    fwrite (file_info -> nwd_buf, sizeof (unsigned char), (file_info -> nwd_p) - (file_info -> nwd_buf), file_info -> nwd_fp);
  }
  wfree (file_info -> nwd_buf);
  fclose (file_info -> nwd_fp);
  wfree (file_info -> nwd_name);

  if (file_info -> nws_p != file_info -> nws_buf) {
    fwrite (file_info -> nws_buf, sizeof (unsigned int), (file_info -> nws_p) - (file_info -> nws_buf), file_info -> nws_fp);
  }
  fclose (file_info -> nws_fp);
  seqReEncode (file_info, nonword_map, ISNONWORD);
  wfree (file_info -> nws_name);
  wfree (file_info -> nws_buf);

  if (file_info -> cfm_p != file_info -> cfm_buf) {
    fwrite (file_info -> cfm_buf, sizeof (unsigned int), (file_info -> cfm_p) - (file_info -> cfm_buf), file_info -> cfm_fp);
  }
  fclose (file_info -> cfm_fp);
  wfree (file_info -> cfm_name);
  wfree (file_info -> cfm_buf);

  if (file_info -> sm_p != file_info -> sm_buf) {
    fwrite (file_info -> sm_buf, sizeof (unsigned int), (file_info -> sm_p) - (file_info -> sm_buf), file_info -> sm_fp);
  }
  fclose (file_info -> sm_fp);
  wfree (file_info -> sm_name);
  wfree (file_info -> sm_buf);

  return;
}


void closeFilesDecode (FILE_STRUCT *file_info, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info) {
  fclose (file_info -> wd_fp);
  wfree (file_info -> wd_name);
  wfree (file_info -> wd_buf);

  fclose (file_info -> ws_fp);
  wfree (file_info -> ws_name);
  wfree (file_info -> ws_buf);

  fclose (file_info -> nwd_fp);
  wfree (file_info -> nwd_name);
  wfree (file_info -> nwd_buf);

  fclose (file_info -> nws_fp);
  wfree (file_info -> nws_name);
  wfree (file_info -> nws_buf);

  fclose (file_info -> cfm_fp);
  wfree (file_info -> cfm_name);
  wfree (file_info -> cfm_buf);

  fclose (file_info -> sm_fp);
  wfree (file_info -> sm_name);
  wfree (file_info -> sm_buf);

  return;
}


/*
**  Process the file
*/
void fileEncode (FILE_STRUCT *file_info, FILE *fp, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info) {
  unsigned char *wrd_buff;
  unsigned int wrd_buff_len = 0;
                    /*  The original word buffer length, before stemming  */
  unsigned char *nonwrd_buff;
  unsigned int nonwrd_buff_len = 0;
  unsigned int wrd_key = 0;
  unsigned int nonwrd_key = 0;
  unsigned int casefold_result = 0;
  unsigned int stem_result = 0;
  unsigned int *m = NULL;
  bool notdone = false;

#ifdef CHARSTATS
  unsigned int wrd_array[MAXPRIMS];
  unsigned int nonwrd_array[MAXPRIMS];
  unsigned int i = 0;
#endif

  unsigned char *src_buff = NULL;
  unsigned char *src_p = NULL;
  unsigned char *src_end = NULL;
  unsigned int num_read = 0;
  unsigned int space_area = 0;
  unsigned int text_area = 0;
#ifdef FLAG_WORDS
  bool end_phrase = false;
#endif

#ifdef CHARSTATS
  for (i = 0; i < MAXPRIMS; i++) {
    wrd_array[i] = 0;
    nonwrd_array[i] = 0;
  }
#endif

  m = wmalloc (sizeof (unsigned int) * word_info -> maxword);
  wrd_buff = wmalloc (sizeof (unsigned char) * word_info -> maxword);
  nonwrd_buff = wmalloc (sizeof (unsigned char) * word_info -> maxword);
  src_buff = wmalloc (sizeof (unsigned char) * (INIT_BUFF_SIZE + 1));

  num_read = fread (src_buff, sizeof (unsigned char), INIT_BUFF_SIZE, fp);
  src_p = src_buff;
  src_end = src_buff + num_read;

  do {
    if (notdone == false) {
      wrd_buff_len = getWord (&src_p, src_end, wrd_buff, word_info -> maxword, &notdone, &word_info -> long_tokens, &word_info -> enforce_tags);
      if (word_info -> docasefold) {
        casefold_result = casefold (wrd_buff, wrd_buff_len);
      }
      if (word_info -> dostem) {
        stem_result = stem (wrd_buff, &wrd_buff_len, m);
      }
      (word_info -> total_length) += wrd_buff_len;
      wrd_key = fcodeEncode (wrd_buff, wrd_buff_len, &word_info -> root_fc, &word_info -> nwords, &word_info -> cmps, &word_info -> total_words_len);
    }
    else {
      wrd_key = EMPTY_FCODE;
      casefold_result = 0;
      stem_result = 0;
      notdone = false;
    }
    (word_info -> total_tokens)++;

    if ((notdone == false) && (src_p != src_end)) {
      nonwrd_buff_len = getNonWord (&src_p, src_end, nonwrd_buff, nonword_info -> maxnonword, &notdone, &nonword_info -> long_tokens);
#ifdef FLAG_WORDS
      if ((nonwrd_buff[0] == '.') || (nonwrd_buff[0] == ',') ||
          (nonwrd_buff[0] == ';') || (nonwrd_buff[0] == '?') ||
          (nonwrd_buff[0] == '!') || (nonwrd_buff[0] == ':') ||
    ((nonwrd_buff_len >= 4) && (nonwrd_buff[1] == '-') &&
           (nonwrd_buff[2] == '-') && (isspace (nonwrd_buff[0]))
     && (isspace (nonwrd_buff[3])))) {
  end_phrase = true;
      }
#endif
      (nonword_info -> total_length) += nonwrd_buff_len;
      nonwrd_key = fcodeEncode (nonwrd_buff, nonwrd_buff_len, &nonword_info -> root_fc, &nonword_info -> nnonwords, &nonword_info -> cmps, &nonword_info -> total_nonwords_len);
    }
    else {
      nonwrd_key = EMPTY_FCODE;
      notdone = false;
    }
    (nonword_info -> total_tokens)++;

#ifdef CHARSTATS
    for (i = 0; i < wrd_buff_len; i++) {
      if (wrd_array[wrd_buff[i]] == 0) {
        wrd_array[wrd_buff[i]] = 1;
      }
    }
    for (i = 0; i < nonwrd_buff_len; i++) {
      if (nonwrd_array[nonwrd_buff[i]] == 0) {
        nonwrd_array[nonwrd_buff[i]] = 1;
      }
    }
#endif
#ifdef FLAG_WORDS
    if (end_phrase == true) {
      wrd_key = wrd_key | TOP_BIT;
      end_phrase = false;
    }
#endif

    if (wrd_key == EMPTY_FCODE) {
      (word_info -> zerolength_sym)++;
    }

    if (nonwrd_key == EMPTY_FCODE) {
      (nonword_info -> zerolength_sym)++;
    }

    writeFiles (file_info, wrd_key, casefold_result, stem_result, nonwrd_key);

    /*  Reload the buffer  */
    space_area = src_p - src_buff;
    text_area = src_end - src_p;
    if ((text_area < MIN_BUFF_SIZE) && (!feof (fp))) {
      memcpy (src_buff, src_p, text_area);
      num_read = fread (src_buff + text_area, sizeof (unsigned char), space_area, fp);
      src_p = src_buff;
      src_end = src_buff + text_area + num_read;
    }
  }  while (src_p != src_end);

#ifdef CHARSTATS
  nprims = 1;              /*  Include 0-length word as a character  */
  for (i = 0; i < MAXPRIMS; i++) {
    if (wrd_array[i] != 0) {
      nprims++;
    }
  }
  word_info -> nwords_prims = nprims;

  nprims = 1;
  for (i = 0; i < MAXPRIMS; i++) {
    if (nonwrd_array[i] != 0) {
      nprims++;
    }
  }
  nonword_info -> nnonwords_prims = nprims;
#endif

  wfree (nonwrd_buff);
  wfree (wrd_buff);
  wfree (m);

  return;
}


void fileDecode (FILE_STRUCT *file_info, FILE *fp, WORD_STRUCT *word_info, NONWORD_STRUCT *nonword_info) {
  unsigned char *wrd;
  unsigned int wrd_len;
  unsigned int wrd_key;
  unsigned int casefold_mod;
  unsigned int stem_mod;
  unsigned char *nonwrd;
  unsigned int nonwrd_len;
  unsigned int nonwrd_key;
  unsigned char *space;
  unsigned int space_len = 0;
  unsigned char *newline;
  unsigned int newline_len = 0;

  wrd = wmalloc (sizeof (unsigned char) * word_info -> maxword);
  nonwrd = wmalloc (sizeof (unsigned char) * nonword_info -> maxnonword);
  space = wmalloc (sizeof (unsigned char) * nonword_info -> maxnonword);
  newline = wmalloc ((sizeof (unsigned char) * nonword_info -> maxnonword));

  if (file_info -> mode == MODE_DECODE) {
    while (readFiles (file_info, &wrd_key, &casefold_mod, &stem_mod, &nonwrd_key) != 0) {
      wrd_key = wrd_key & NO_TOP_BIT;
      if (wrd_key != 0) {
        LOOKUPFCODE (word_info -> dict_fc, wrd_key, wrd, wrd_len);
        wrd_len = unstem (wrd, wrd_len, stem_mod);
        uncasefold (wrd, wrd_len, casefold_mod);
        uprintf (fp, wrd, wrd_len);
      }
      if (nonwrd_key != 0) {
        LOOKUPFCODE (nonword_info -> dict_fc, nonwrd_key, nonwrd, nonwrd_len);
        uprintf (fp, nonwrd, nonwrd_len);
      }
    }
  }
  else if (file_info -> mode == MODE_DECODE_NONE) {
    /*  Forced-pairing not supported!!!  */
    space[0] = ' ';
    space_len = 1;
    newline[0] = '\n';
    newline_len = 1;
    while (readFiles (file_info, &wrd_key, &casefold_mod, &stem_mod, &nonwrd_key) != 0) {
      wrd_key = wrd_key & NO_TOP_BIT;
      if (wrd_key != 0) {
        LOOKUPFCODE (word_info -> dict_fc, wrd_key, wrd, wrd_len);
        uprintf (fp, wrd, wrd_len);
        /*  Add a newline after every closing tag.  */
  if ((wrd_len > 2) && (wrd[0] == '<') && (wrd[1] == '/')) {
          uprintf (fp, newline, newline_len);
  }
  else {
          uprintf (fp, space, space_len);
  }
      }
      if (nonwrd_key != 0) {
  /*  Do nothing  */
      }
    }
  }
  else if (file_info -> mode == MODE_DECODE_LINK) {
    space[0] = ' ';
    space_len = 1;
    newline[0] = '\n';
    newline_len = 1;
    while (readFiles (file_info, &wrd_key, &casefold_mod, &stem_mod, &nonwrd_key) != 0) {
      wrd_key = wrd_key & NO_TOP_BIT;
      if (wrd_key != 0) {
        LOOKUPFCODE (word_info -> dict_fc, wrd_key, wrd, wrd_len);
        uprintf (fp, wrd, wrd_len);
      }
      if (nonwrd_key != 0) {
        LOOKUPFCODE (nonword_info -> dict_fc, nonwrd_key, nonwrd, nonwrd_len);
  /*  If the first non-word is a newline, add a newline; space
  **  otherwise.  */
        if (nonwrd[0] == '\n') {
          uprintf (fp, newline, newline_len);
  }
  else {
    uprintf (fp, space, space_len);
  }
      }
      else {
  /*  Ensure a space is added after every word token, at least.  */
  uprintf (fp, space, space_len);
      }
    }
  }
  else {
    fprintf (stderr, "Invalid option chosen (%s, line %u).", __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }

  wfree (space);
  wfree (newline);
  wfree (nonwrd);
  wfree (wrd);

  return;
}


