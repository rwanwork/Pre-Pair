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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>

#include "common-def.h"
#include "wmalloc.h"
#include "ustring.h"
#include "prepair-defn.h"
#include "fcode.h"
#include "word.h"
#include "nonword.h"
#include "prepair.h"

static void usage (char *progname) {
  fprintf (stderr, "Pre-pair (Re-Pair Word-based Pre-processor)\n");
  fprintf (stderr, "===========================================\n\n");
  fprintf (stderr, "Usage: %s [-d | -e | -n | -l] [options] <input >output\n", progname);
  fprintf (stderr, "Options:\n");
  fprintf (stderr, "-c\t: Perform case folding.\n");
  fprintf (stderr, "-d\t: Decode.\n");
  fprintf (stderr, "-e\t: Encode.\n");
  fprintf (stderr, "-n\t: Decode with no stemming / case-folding.\n");
  fprintf (stderr, "-l\t: Decode for comparison with Link-Grammar.\n");
  fprintf (stderr, "-h/-?\t: Display this message\n");
  fprintf (stderr, "-i\t: Base filename required for naming output files (encoding)\n\t  or input files (decoding).\n");
  fprintf (stderr, "-m\t: Maximum string length, at most %u because of front coding.\n", MAXWORDLEN);
  fprintf (stderr, "-p\t: Print sorted words to stdout.\n");
  fprintf (stderr, "-s\t: Perform stemming.\n");
  fprintf (stderr, "-v\t: Verbose output\n");
  fprintf (stderr, "\nDictionary encoding settings (compile-time):\n");
  fprintf (stderr, "\tWords are encoded using ");
  fprintf (stderr, "no coding.\n");
  fprintf (stderr, "\tNonwords are encoded using ");
  fprintf (stderr, "no coding.\n");
  fprintf (stderr, "\nThe input text file is from stdin.\n");
  fprintf (stderr, "The output text file is sent to stdout.\n");
  fprintf (stderr, "Pre-pair version: %s (%s)\n\n", __DATE__, __TIME__);
  exit (EXIT_SUCCESS);
}

R_INT main (R_INT argc, R_CHAR **argv) {
  R_CHAR *progname = argv[0];
  R_UCHAR *filename = NULL;
  R_BOOLEAN verbose_level = R_FALSE;
  FILE_STRUCT *file_info = NULL;
  WORD_STRUCT *word_info = NULL;
  NONWORD_STRUCT *nonword_info = NULL;

  R_INT c;
  enum PROGMODE mode = 0;

  /*  Temporary variables used by getopt  */
  R_BOOLEAN docasefold = R_FALSE;
  R_UINT maxword = MAXWORDLEN;
  R_BOOLEAN dostem = R_FALSE;
  R_BOOLEAN printsorted = R_FALSE;

  if (argc == 1) {
    usage (progname);
  }

  while (R_TRUE) {
    c = getopt (argc, argv, "cdehi:lm:npsv?");
    if (c == EOF) {
      break;
    }

    switch (c) {
    case 'c':
      docasefold = R_TRUE;
      break;
    case 'd':
      if (mode != MODE_NONE) {
        fprintf (stderr, "Please choose one of -e, -d, -n, or -l.\n");
        exit (EXIT_FAILURE);
      }
      mode = MODE_DECODE;
      break;
    case 'e':
      if (mode != MODE_NONE) {
        fprintf (stderr, "Please choose one of -e, -d, -n, or -l.\n");
        exit (EXIT_FAILURE);
      }
      mode = MODE_ENCODE;
      break;
    case 'h':
    case '?':
      usage (progname);
      break;
    case 'i':
      filename = wmalloc (sizeof (R_UCHAR) * strlen (optarg) + 1);
      ustrcpy (filename, (R_UCHAR*) optarg);
      break;
    case 'm':
      maxword = (R_UINT) atoi (optarg);
      if ((maxword < WORDLEN) || (maxword > MAXWORDLEN)) {
        fprintf (stderr, "The maximum word length must be between WORDLEN and MAXWORDLEN, inclusive. (%s, line %u)\n", __FILE__, __LINE__);
        exit (EXIT_FAILURE);
      }
      break;
    case 'l':
      if (mode != MODE_NONE) {
        fprintf (stderr, "Please choose one of -e, -d, -n, or -l.\n");
        exit (EXIT_FAILURE);
      }
      mode = MODE_DECODE_LINK;
      break;
    case 'n':
      if (mode != MODE_NONE) {
        fprintf (stderr, "Please choose one of -e, -d, -n, or -l.\n");
        exit (EXIT_FAILURE);
      }
      mode = MODE_DECODE_NONE;
      break;
    case 'p':
      printsorted = R_TRUE;
      break;
    case 's':
      dostem = R_TRUE;
      break;
    case 'v':
      verbose_level = R_TRUE;
      break;
    default:
      fprintf (stderr, "Unexpected error:  getopt returned character code 0%d.\n", c);
      return (EXIT_FAILURE);
    }
  }

  if (optind < argc) {
    fprintf (stderr, "The following arguments were not valid:  ");
    while (optind < argc) {
      fprintf (stderr, "%s ", argv[optind++]);
    }
    fprintf (stderr, "\nRun %s with the -h option for help.\n", progname);
    exit (EXIT_FAILURE);
  }

  if (filename == NULL) {
    fprintf (stderr, "Filename required with -i option (%s, line %u).", __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }
  if (mode == MODE_NONE) {
    fprintf (stderr, "Please specify one of -e, -d, -n, or -l (%s, line %u).\n", __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }

  file_info = wmalloc (sizeof (FILE_STRUCT));
  file_info -> verbose_level = verbose_level;
  file_info -> mode = mode;

  openFiles (filename, file_info, (mode == MODE_ENCODE ? "w" : "r"), R_FALSE);

  word_info = wmalloc (sizeof (WORD_STRUCT));
  nonword_info = wmalloc (sizeof (NONWORD_STRUCT));
  initPrepair (word_info, nonword_info, maxword, docasefold, dostem, printsorted);

  if (mode == MODE_ENCODE) {
    fileEncode (file_info, stdin, word_info, nonword_info);

    word_info -> map = wmalloc (word_info -> nwords * sizeof (R_UINT));
    nonword_info -> map = wmalloc (nonword_info -> nnonwords * sizeof (R_UINT));

    /*  The 0-length symbols with an id of 0 always map back to 0.  */
    word_info -> map[0] = 0;
    nonword_info -> map[0] = 0;

    word_info -> dict_fc = wmalloc (word_info -> nwords * sizeof (FCODENODE));
    fcodeDictEncode (file_info, word_info -> root_fc, word_info -> dict_fc, word_info -> map, word_info -> printsorted, word_info -> nwords, ISWORD);

    nonword_info -> dict_fc = wmalloc (nonword_info -> nnonwords * sizeof (FCODENODE));
    fcodeDictEncode (file_info, nonword_info -> root_fc, nonword_info -> dict_fc, nonword_info -> map, nonword_info -> printsorted, nonword_info -> nnonwords, ISNONWORD);

  /* Write some overall statistics */
    if (file_info -> verbose_level == R_TRUE) {
      if (docasefold == R_TRUE) {
        fprintf (stderr, "Words:  case-folded and ");
      }
      else {
        fprintf (stderr, "Words:  NOT case-folded and ");
      }
      if (dostem == R_TRUE) {
        fprintf (stderr, "stemmed\n");
      }
      else {
        fprintf (stderr, "NOT stemmed\n");
      }
      /*  Next line added 2004/02/19  */
#ifdef FLAG_WORDS
    fprintf (stderr, "        for punctuation-based Re-Pair.\n");
#else
    fprintf (stderr, "        for word-based Re-Pair.\n");
#endif
      fprintf (stderr, "Words were literal-coded.\n");
      fprintf (stderr, "\t%6u word tokens found, of which\n", word_info -> total_tokens);
      fprintf (stderr, "\t\t%6u word tokens were longer than %u characters\n", word_info -> long_tokens, MAXWORDLEN);
      fprintf (stderr, "\t\t%6u word tokens broken because of tags\n", word_info -> enforce_tags);
      fprintf (stderr, "\t\t%6u word tokens were zero-length\n", word_info -> zerolength_sym);
      fprintf (stderr, "\t%6.3f characters in length in message (average)\n", (R_FLOAT) word_info -> total_length / (R_FLOAT) word_info -> total_tokens);
      fprintf (stderr, "\t%6.3f characters in length in lexicon (average)\n", (R_FLOAT) word_info -> total_words_len / (R_FLOAT) word_info -> nwords);
      fprintf (stderr, "\t%6u unique word tokens (incl. 0-length word)\n", word_info -> nwords);
      fprintf (stderr, "\t%6.2f average comparisons for each identified word\n", (double)(word_info -> cmps)/(word_info -> total_tokens));
    }

    if (file_info -> verbose_level == R_TRUE) {
      fprintf (stderr, "Nonwords were literal-coded.\n");
      fprintf (stderr, "\t%6u nonword tokens found, of which\n", nonword_info -> total_tokens);
      fprintf (stderr, "\t\t%6u nonword tokens were longer than %u characters\n", nonword_info -> long_tokens, MAXWORDLEN);
      fprintf (stderr, "\t\t%6u nonword tokens broken because of tags\n", nonword_info -> enforce_tags);
      fprintf (stderr, "\t\t%6u nonword tokens were zero-length\n", nonword_info -> zerolength_sym);
      fprintf (stderr, "\t\t\t(may be 1 larger than expected,\n\t\t\tif file ended with a word)\n");
      fprintf (stderr, "\t%6.3f characters in length in message (average)\n", (R_FLOAT) nonword_info -> total_length / (R_FLOAT) nonword_info -> total_tokens);
      fprintf (stderr, "\t%6.3f characters in length in lexicon (average)\n", (R_FLOAT) nonword_info -> total_nonwords_len / (R_FLOAT) nonword_info -> nnonwords);
      fprintf (stderr, "\t%6u unique nonword tokens (incl. 0-length nonword)\n", nonword_info -> nnonwords);
      fprintf (stderr, "\t%6.2f average comparisons for each identified nonword\n", (double)(nonword_info -> cmps)/(nonword_info -> total_tokens));
    }

    closeFilesEncode (file_info, word_info -> map, nonword_info -> map);
  }
  else {
    word_info -> nwords = INIT_FCODE_SIZE;
    nonword_info -> nnonwords = INIT_FCODE_SIZE;

    word_info -> dict_fc = wmalloc ((word_info -> nwords) * sizeof (FCODENODE));
    word_info -> nwords = fcodeDictDecode (file_info, &word_info -> dict_fc, word_info -> nwords, ISWORD);

    nonword_info -> dict_fc = wmalloc ((nonword_info -> nnonwords) * sizeof (FCODENODE));
    nonword_info -> nnonwords = fcodeDictDecode (file_info, &nonword_info -> dict_fc, nonword_info -> nnonwords, ISNONWORD);

    fileDecode (file_info, stdout, word_info, nonword_info);

    closeFilesDecode (file_info, word_info, nonword_info);
  }

  wfree (nonword_info);
  wfree (word_info);
  wfree (file_info);
  wfree (filename);

  return (EXIT_SUCCESS);
}
