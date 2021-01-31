/*******************************************************************/
/*!
    \file wmalloc.c
    Wrapper functions for allocation memory.
    
    $LastChangedDate: 2013-09-10 02:02:18 +0800 (Tue, 10 Sep 2013) $
    $LastChangedRevision: 10 $

*/
/*******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common-def.h"
#include "wmalloc.h"

static unsigned int inuse_malloc = 0;
static unsigned int max_malloc = 0;
static WMSTRUCT **wm_array;
static char *tempstr;

void *wmalloc (size_t y_arg) {
  void *x_arg = malloc (y_arg);
  if (x_arg == NULL) {
    fprintf (stderr, "Error in malloc while allocating %u bytes in [%s, %u].\n", (unsigned int) y_arg, __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }
#ifdef COUNT_MALLOC
  countMalloc (x_arg, y_arg, __FILE__, __LINE__);
#endif

  return (x_arg);
}


void *wrealloc (void *x_arg, size_t y_arg) {
#ifdef COUNT_MALLOC
  countFree ((void*) x_arg);
#endif
  x_arg = realloc (x_arg, y_arg);

  if (x_arg == NULL) {
    fprintf (stderr, "Error in realloc while allocating %u bytes in [%s, %u].\n", (unsigned int) y_arg, __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }
#ifdef COUNT_MALLOC
  countMalloc ((void*) x_arg, y_arg, __FILE__, __LINE__);
#endif

  return (x_arg);
}

void wfree (void *x_arg) {
#ifdef COUNT_MALLOC
  countFree (x_arg);
#endif
  free (x_arg);
}

/*
**  Function adapted from Algorithms in C (Third edition) by Robert Sedgewick
**  (page 578)
*/
static unsigned int hash (char *v, int M) {
  int h = 0;
  int a = 127;

  /*  Skip the 0x at the beginning  */
  v += 2;
  for (; *v != '\0'; v++) {
    h = (a*h + (int) *v) % M;
  }

  return ((unsigned int) h);
}

void initWMalloc () {
  WMSTRUCT *node = NULL;
  unsigned int i = 0;

  inuse_malloc = 0;
  max_malloc = 0;

  tempstr = wmalloc (sizeof (char) * TEMPSTRLEN);

  wm_array = wmalloc (sizeof (WMSTRUCT*) * WM_SIZE);
  for (i = 0; i < WM_SIZE; i++) {
    /*  Add sentinel  */
    node = wmalloc (sizeof (WMSTRUCT));
    node -> ptr = NULL;
    node -> size = 0;
    node -> file = NULL;
    node -> line = 0;
    node -> next = NULL;
    wm_array[i] = node;
  }

  return;
}


void printWMalloc () {
  fprintf (stderr, "\tMemory used at exit:  %u\n", inuse_malloc);
  fprintf (stderr, "\tMaximum memory used at once:  %u\n", max_malloc);
  fprintf (stderr, "\tMaximum memory (MB):  %.1f\n", (double) max_malloc / (double) (1024 * 1024));

  return;
}


void printInUseWMalloc (void) {
  unsigned int i = 0;
  WMSTRUCT *curr = NULL;

  for (i = 0; i < WM_SIZE; i++) {
    if (wm_array[i] -> ptr != NULL) {
      curr = wm_array[i];
      while (curr -> ptr != NULL) {
        fprintf (stderr, "%p\t(%u)\t[%s]\t[%u]\n", curr -> ptr, (unsigned int) curr -> size, curr -> file, curr -> line);
        curr = curr -> next;
      }
    }
  }

  return;
}


void countMalloc (void *ptr, size_t amount, const char *file, const unsigned int line) {
  WMSTRUCT *node = NULL;
  unsigned int pos = 0;

  node = wmalloc (sizeof (WMSTRUCT));
  node -> ptr = ptr;
  node -> size = amount;
  node -> file = wmalloc (sizeof (char) * (strlen (file) + 1));
  node -> file = strcpy (node -> file, file);
  node -> line = line;

  (void) snprintf (tempstr, TEMPSTRLEN, "%p", ptr);
  pos = hash (tempstr, WM_SIZE);
  node -> next = wm_array[pos];
  wm_array[pos] = node;

  inuse_malloc += amount;

  if (inuse_malloc > max_malloc) {
    max_malloc = inuse_malloc;
  }

  return;
}


void countFree (void *ptr) {
  WMSTRUCT *prev = NULL;
  WMSTRUCT *curr = NULL;
  unsigned int pos = 0;

  (void) snprintf (tempstr, TEMPSTRLEN, "%p", ptr);
  pos = hash (tempstr, WM_SIZE);

  /*  Locate the node  */
  curr = wm_array[pos];
  while (curr -> ptr != NULL) {
    if (curr -> ptr == ptr) {
      break;
    }
    prev = curr;
    curr = curr -> next;
  }
  if (curr -> ptr == NULL) {
    fprintf (stderr, "(F) Fatal error.  Node %p could not be found.\n", ptr);
    exit (EXIT_FAILURE);
  }

  inuse_malloc -= curr -> size;

  if (inuse_malloc > max_malloc) {
    max_malloc = inuse_malloc;
  }

  /*  Remove the node  */
  if (prev == NULL) {
    wm_array[pos] = curr -> next;
  }
  else {
    prev -> next = curr -> next;
  }
  free (curr -> file);
  free (curr);

  return;
}


