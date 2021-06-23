#include "cash.h"

extern int nrow, ncol, first, last;

TYPE **And(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] && c[0][i]);
  return a;
}

TYPE **AndNot(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] && !(c[0][i]));
  return a;
}

TYPE **Or(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] || c[0][i]);
  return a;
}

TYPE **Xor(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] != c[0][i]);
  return a;
}

TYPE **Not(TYPE **a,TYPE  **b)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = !b[0][i];
  return a;
}

TYPE **AndCopy(TYPE **a,TYPE  **b,TYPE  **c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    if (c[0][i] && b[0][i]) a[0][i] = b[0][i];
  return a;
}
