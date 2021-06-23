#include "cash.h"

extern int nrow, ncol;
static TYPE **n, **s, **w, **e, **ne, **nw, **se, **sw;
static TYPE **all[9];
static int initneigh = 0;

int InitNeigh()
{
  int i;
  n = NewP();
  s = NewP();
  w = NewP();
  e = NewP();
  ne = NewP();
  nw = NewP();
  se = NewP();
  sw = NewP();
  for (i=0;i<9;i++)
    all[i] = NewP();
  initneigh = 1;
}

TYPE ***Neighbors(TYPE **a[9],TYPE **b)
{
/*
   5 1 6
   2 0 3
   7 4 8
*/
  North(a[NORTH],b);
  South(a[SOUTH],b);
  West(a[WEST],b);
  East(a[EAST],b);
  West(a[NORTHWEST],a[NORTH]);
  East(a[NORTHEAST],a[NORTH]);
  West(a[SOUTHWEST],a[SOUTH]);
  East(a[SOUTHEAST],a[SOUTH]);
  a[CENTRAL] = b;

  return a;
}

TYPE **Moore8(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!initneigh) InitNeigh();

  North(n,b);
  South(s,b);
  East(e,b);
  West(w,b);
  East(ne,n);
  West(nw,n);
  East(se,s);
  West(sw,s);

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) 
      a[i][j] = n[i][j] + s[i][j] + e[i][j] + w[i][j] +
		ne[i][j] + nw[i][j] + se[i][j] + sw[i][j];

  return a;
}

TYPE **Moore9(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!initneigh) InitNeigh();

  North(n,b);
  South(s,b);
  East(e,b);
  West(w,b);
  East(ne,n);
  West(nw,n);
  East(se,s);
  West(sw,s);

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) 
      a[i][j] = b[i][j] + n[i][j] + s[i][j] + e[i][j] + w[i][j] +
		ne[i][j] + nw[i][j] + se[i][j] + sw[i][j];

  return a;
}

TYPE **VonNeumann4(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!initneigh) InitNeigh();

  North(n,b);
  South(s,b);
  East(e,b);
  West(w,b);

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) 
      a[i][j] = n[i][j] + s[i][j] + e[i][j] + w[i][j];

  return a;
}

TYPE **VonNeumann5(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!initneigh) InitNeigh();

  North(n,b);
  South(s,b);
  East(e,b);
  West(w,b);

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) 
      a[i][j] = b[i][j] + n[i][j] + s[i][j] + e[i][j] + w[i][j];

  return a;
}

TYPE **Diagonal4(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!initneigh) InitNeigh();

  North(n,b);
  South(s,b);
  East(ne,n);
  West(nw,n);
  East(se,s);
  West(sw,s);

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) 
      a[i][j] = ne[i][j] + nw[i][j] + se[i][j] + sw[i][j];

  return a;
}

TYPE **Diagonal5(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!initneigh) InitNeigh();

  North(n,b);
  South(s,b);
  East(ne,n);
  West(nw,n);
  East(se,s);
  West(sw,s);

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) 
      a[i][j] = b[i][j] + ne[i][j] + nw[i][j] + se[i][j] + sw[i][j];

  return a;
}

static int is[8]={0,1,1,1,0,-1,-1,-1};
static int js[8]={-1,-1,0,1,1,1,0,-1};

TYPE **RandomNeighbor(TYPE **a,TYPE  **b)
{
  int i,j,nr,nc;
  nr = nrow;
  nc = ncol;

  if (!initneigh) InitNeigh();
  Neighbors(all,b);

  for (i=1; i<=nr; i++)
    /*$dir force_vector*/
    for (j=1; j<=nc; j++)
      a[i][j] = all[1+(int)(8*RANDOM())][i][j];
  return a;
}
