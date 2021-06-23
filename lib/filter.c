#include "cash.h"

extern int nrow, ncol, first, last;
  
TYPE **EQ(TYPE **a,TYPE  **b,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] == c);
  return a;
}

TYPE **NE(TYPE **a,TYPE  **b,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] != c);
  return a;
}

TYPE **GE(TYPE **a,TYPE  **b,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] >= c);
  return a;
}

TYPE **LE(TYPE **a,TYPE  **b,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] <= c);
  return a;
}

TYPE **GT(TYPE **a,TYPE  **b,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] > c);
  return a;
}

TYPE **LT(TYPE **a,TYPE  **b,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] < c);
  return a;
}

TYPE **IN(TYPE **a,TYPE  **b,int c,int  d)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = (b[0][i] >= c && b[0][i] <= d);
  return a;
}
