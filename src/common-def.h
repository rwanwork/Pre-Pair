/*******************************************************************/
/*!
    \file common-def.h
    Header file for common definitions.
    
    $LastChangedDate: 2013-09-10 02:02:18 +0800 (Tue, 10 Sep 2013) $
    $LastChangedRevision: 10 $

*/
/*******************************************************************/


#ifndef COMMON_DEF_H
#define COMMON_DEF_H

enum VERBOSE { NOVERBOSE = 0, ONELINE = 1, INFO = 2, TEST = 3, ALL = 4 };

#define VERBOSE_ONELINE(X, Y) \
  if (X >= ONELINE) { Y }

#define VERBOSE_INFO(X, Y) \
  if (X >= INFO) { Y }

#define VERBOSE_TEST(X, Y) \
  if (X >= TEST) { Y }


/***************************************************************************
Redefine common primitive data types
***************************************************************************/
typedef int R_INT;
typedef unsigned short int R_USHRT;
typedef unsigned int R_UINT;
typedef unsigned long int R_UL_INT;
typedef signed long int R_L_INT;
typedef unsigned long long int R_ULL_INT;
typedef double R_DOUBLE;
typedef float R_FLOAT;

typedef unsigned char R_UCHAR;
typedef char R_CHAR;

/***************************************************************************
Define booleans
***************************************************************************/
#ifndef R_TRUE
#define R_TRUE 1
#endif

#ifndef R_FALSE
#define R_FALSE 0
#endif

#ifndef R_BOOLEAN
#define R_BOOLEAN unsigned int
#endif

/***************************************************************************
Bit masking
***************************************************************************/
#define MASK_EIGHT (0xFFu)  /*  255  */
#define MASK_LOWER (0xFFFFFFFFu)  /*  4294967295  */
#define MASK_HIGHEST (1u << ((sizeof (unsigned int) * 8) - 1))  /*  2147483648  */

#define FOPEN(FILENAME,FP,MODE) \
  FP = fopen ((R_CHAR*) FILENAME, MODE); \
  if (FP == NULL) { \
    if (strcmp (MODE, "w") == 0) { \
      fprintf (stderr, "Error creating %s.\n", FILENAME); \
    } \
    else { \
      fprintf (stderr, "Error opening %s.\n", FILENAME); \
    } \
    fprintf (stderr, "Error %s %s.\n", (strcmp (MODE, "w") == 0) ? "creating" : "opening", FILENAME); \
    exit (EXIT_FAILURE); \
  }

#define FCLOSE(FP) \
  (void) fclose (FP);

#endif

