#include <stdarg.h>
#include "cash.h"

extern int nrow, ncol, first, last;

TYPE **Sum(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] + c[0][i];
  return a;
}

TYPE **SumV(TYPE **a,TYPE  **b,TYPE  c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] + c;
  return a;
}

TYPE **Minus(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] - c[0][i];
  return a;
}

TYPE **MinusV(TYPE **a,TYPE  **b,TYPE  c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] - c;
  return a;
}

TYPE **Div(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] / c[0][i];
  return a;
}

TYPE **DivV(TYPE **a,TYPE  **b,TYPE  c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] / c;
  return a;
}

TYPE **Mod(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] % c[0][i];
  return a;
}

TYPE **ModV(TYPE **a,TYPE  **b,TYPE  c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] % c;
  return a;
}

TYPE **Mult(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] * c[0][i];
  return a;
}

TYPE **MultV(TYPE **a,TYPE  **b,TYPE  c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] * c;
  return a;
}

TYPE **MultF(TYPE **a,TYPE  **b,float c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] * c;
  return a;
}

TYPE **BinSumOld(TYPE **a,TYPE  **b[], int c)
{
  int i,j,bin;
  Copy(a,b[0]);
  for (bin=1; bin<c; bin++)
    /*$dir force_vector*/
    for (i=first,j=last; i <= j; i++)
      a[0][i] += b[bin][0][i] << bin; 
  return a;
}

TYPE **BinSum(TYPE **a,int n, ...)
{
  int i,j,bin;
  va_list ap;        /* points to each unnamed arg in turn */
  int cnt;
  TYPE **plane;      /* holds the unnamed arg */

  if (n < 1 || n > sizeof(TYPE)) {
    printf("Error in BinSum: n=%d (max value=%d\n",n,sizeof(TYPE));
    n = 0;
  }

  va_start(ap,n);  /* make ap point to first unnamed arg */
  plane=va_arg(ap,TYPE **);   /* retrieve value from next unnamed arg */
  if (n) Copy(a,plane);
  
  if (n>1) {
    for (cnt=1;cnt<n;cnt++) {
      plane=va_arg(ap,TYPE **);   /* retrieve value from next unnamed arg */
      for (i=first,j=last; i <= j; i++)
	a[0][i] += plane[0][i] << cnt;
    }
    va_end(ap);      /* clean up */
  }
  return a;
}

TYPE **RollRight(TYPE **a,TYPE **b,int c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] >> c;
  return a;
}

TYPE **RollLeft(TYPE **a,TYPE **b,int c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i] << c;
  return a;
}

TYPE **GetBits(TYPE **a,TYPE **b,int f,int l)
{
  int i,j;
  unsigned mask;
  mask = ~(~0 << (l-f+1));
  if (f > l) fprintf(stderr,"Error in GetBits: first > last");
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] >> f) & mask;
  return a;
}

TYPE **PutBits(TYPE **a,int p,int v)
{
  int i,j;
  unsigned mask;
  mask = v << p;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = a[0][i] | mask;
  return a;
}

TYPE **Hamming(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j,k;
  unsigned n;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++) {
    n = b[0][i] ^ c[0][i];
    for (k=0; n != 0; n >>= 1)
      if (n & 01) k++;
    a[0][i] = k;
  }
  return a;
}
