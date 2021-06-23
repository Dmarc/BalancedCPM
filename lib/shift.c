#include "cash.h"

extern int nrow, ncol, boundary, boundaryvalue;

TYPE **Boundaries(TYPE **a)
{
  int i,j;
  int bound,nc,nr,nr1,nc1;
  nr = nrow;
  nc = ncol;
  nr1 = nrow + 1;
  nc1 = ncol + 1;
  if (boundary == WRAP) {
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) {
      a[i][0] = a[i][nc];
      a[i][nc1] = a[i][1];
    }
    /*$dir force_vector*/
    for (j=0; j <= nc1; j++) {
      a[0][j] = a[nr][j];
      a[nr1][j] = a[1][j];
    }
  }else if (boundary == FIXED) {
    bound = boundaryvalue;
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      a[i][0] = a[i][nc1] = bound;
    /*$dir force_vector*/
    for (j=0; j <= nc1; j++) {
      a[0][j] = a[nr1][j] = bound;
    }
  }else if (boundary == ECHO) {
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) {
      a[i][0] = a[i][1];
      a[i][nc1] = a[i][nc];
    }
    /*$dir force_vector*/
    for (j=0; j <= nc1; j++) {
      a[0][j] = a[1][j];
      a[nr1][j] = a[nr][j];
    }
  }
  return a;
}

TYPE **North(TYPE **a,TYPE  **b)
{
  int i,j;
  int bound,nc, nr;
  nr = nrow;
  nc = ncol;
  
  /*$dir force_vector*/
  for (i=1; i <= nr; i++)
    a[i] = b[i-1];

  if (boundary == WRAP) {
    a[1] = b[nr];
  }else if (boundary == FIXED) {
    bound = boundaryvalue;
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      b[0][j] = bound;
  }else if (boundary == ECHO) {
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      b[0][j] = b[1][j];
  }

  return a;
}

TYPE **South(TYPE **a,TYPE  **b)
{
  int i,j;
  int bound,nc, nr;
  nr = nrow;
  nc = ncol;
  
  /*$dir force_vector*/
  for (i=1; i <= nr; i++)
    a[i] = b[i+1];

  if (boundary == WRAP) {
    a[nr] = b[1];
  }else if (boundary == FIXED) {
    bound = boundaryvalue;
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      b[nr+1][j] = bound;
  }else if (boundary == ECHO) {
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      b[nr+1][j] = b[nr][j];
  }

  return a;
}

TYPE **East(TYPE **a,TYPE  **b)
{
  int i;
  int bound,nc, nr;
  nr = nrow;
  nc = ncol;

  /*$dir force_vector*/
  for (i=1; i <= nr; i++) 
    a[i] = &b[i][1];

  if (boundary == WRAP) {
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      b[i][nc+1] = b[i][1]; 
  }else if (boundary == FIXED) {
    bound = boundaryvalue;
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      b[i][nc+1] = bound;
  }else if (boundary == ECHO) {
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      b[i][nc+1] = b[i][nc];
  }
  return a;
}

TYPE **West(TYPE **a,TYPE  **b)
{
  int i;
  int bound,nc, nr;
  nr = nrow;
  nc = ncol;

  /*$dir force_vector*/
  for (i=1; i <= nr; i++)
    a[i] = &b[i][-1];

  if (boundary == WRAP) {
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      b[i][0] = b[i][nc]; 
  } else if (boundary == FIXED) {
    bound = boundaryvalue;
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      b[i][0] = bound;
  } else if (boundary == ECHO) {
    /*$dir force_vector*/
    for (i=1; i <= nr; i++) 
      b[i][0] = b[i][1];
  }
  return a;
}
