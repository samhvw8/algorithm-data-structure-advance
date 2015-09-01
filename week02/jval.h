/*
  Generic Data type
  Author: Ginz
 */
#ifndef _GIN_JVAL
#define _GIN_JVAL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JVAL_I(_val) _val.i
#define JVAL_F(_val) _val.f
#define JVAL_D(_val) _val.d
#define JVAL_S(_val) _val.s
#define JVAL_V(_val) _val.v
#define JVAL_L(_val) _val.l
#define JVAL_C(_val) _val.c

typedef union _jval {
  int i;
  float f;
  double d;
  char *s;
  void *v;
  long l;
  char c;
} jval;

extern jval new_jval_int(int val);

extern jval new_jval_float(float val);

extern jval new_jval_double(double val);

extern jval new_jval_string(char* val);

extern jval new_jval_void(void* val, size_t size);

extern jval new_jval_long(long val);

extern jval new_jval_char(char val);

extern void exch(jval *a, jval *b);

extern void sort_gen(jval *arr, int l, int r, int (*compare)(jval *, jval *));

extern void sort_gen_int(jval *arr, int l, int r);

extern int search_gen(jval *arr, int l, int r, jval val,int (*compare)(jval *, jval *));

extern int search_gen_int(jval *arr, int l, int r, jval val);

extern int compare_int(jval *a, jval *b);

#endif
