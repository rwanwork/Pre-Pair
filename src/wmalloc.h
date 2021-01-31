/*******************************************************************/
/*!
    \file wmalloc.h
    Header file for wmalloc.c .
    
    $LastChangedDate: 2013-09-10 02:02:18 +0800 (Tue, 10 Sep 2013) $
    $LastChangedRevision: 10 $

*/
/*******************************************************************/


#ifndef WMALLOC_H
#define WMALLOC_H

#define WM_SIZE 65536
#define TEMPSTRLEN 80

typedef struct wmstruct {
  void *ptr;
  size_t size;
  char *file;
  unsigned int line;
  struct wmstruct *next;
} WMSTRUCT;

void *wmalloc (size_t y_arg);
void *wrealloc (void *x_arg, size_t y_arg);
void wfree (void *x_arg);

void initWMalloc (void);
void printWMalloc (void);
void printInUseWMalloc (void);
void countMalloc (void *ptr, size_t amount, const char *file, unsigned int line);
void countFree (void *ptr);

#endif

