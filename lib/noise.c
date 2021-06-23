#include "cash.h"

static TYPE **ranstack[4];
static TYPE **noc, **sc, **wc, **ec, **nd, **sd, **wd, **ed;
int RandomInitialized = 0, rancounter;
extern int seed, seedset, boundary;
extern int nrow, ncol, first, last;

TYPE **Stir(TYPE **a,TYPE  **b,TYPE  **c,TYPE  **d)
{
  int i,j;
  int nc, nr,bound;
  nr = nrow;
  nc = ncol;

  bound = boundary;
  boundary = 0; 

  North(noc,c);
  South(sc,c);
  East(ec,c);
  West(wc,c);

  North(nd,d);
  South(sd,d);
  East(ed,d);
  West(wd,d);

  boundary = bound;

  for(i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) {
      a[i][j] = (c[i][j] & noc[i][j] ^ wc[i][j] ^ sc[i][j] ^ ec[i][j]) ^ d[i][j];
      b[i][j] = (d[i][j] & nd[i][j] ^ ed[i][j] ^ sd[i][j] ^ wd[i][j]) ^ c[i][j];
    }
 
  return a;
}

int RanIndex()
{
  rancounter++;
  if (rancounter == 1) { 
    Stir(ranstack[2],ranstack[3],ranstack[0],ranstack[1]);
    return rancounter;
  }
  if (rancounter == 3) {
    Stir(ranstack[0],ranstack[1],ranstack[2],ranstack[3]);
    return rancounter;
  }
  if (rancounter == 4) 
    return (rancounter = 0);
  return rancounter;
}


TYPE **NoiseBox(TYPE **a)
{
  int i,j,nr;
  nr = nrow;
  if (!RandomInitialized) InitRandom();
  j = RanIndex();
  /*$dir force_vector*/
  for (i=1; i <= nr; i++)
    a[i] = ranstack[j][i];
}

int InitRandom()
{
  int i,j,r;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  if (!seedset) {
    SEED(seed);
    seedset = 1;
  }

  noc = NewP();
  sc = NewP();
  ec = NewP();
  wc = NewP();
  nd = NewP();
  sd = NewP();
  ed = NewP();
  wd = NewP();

  for (r=0; r<4; r++)
    ranstack[r] = New();
  for (r=0; r<2; r++)
    Random(ranstack[r],0.5);
  rancounter = -1;
  return (RandomInitialized = 1);
}

TYPE **Random(TYPE **a,float b)
{
  int i,j;
  /*$dir force_vector*/
  for(i=first,j=last; i <= j; i++)
    a[0][i] = (RANDOM() < b);
  return a; 
}

TYPE **Normalize(TYPE **a,TYPE **b)
{
  int nr,nc;
  float frac,delta;
  TYPE **n;
  nr=nrow; nc=ncol;
  n=New();
  frac=(float)Total(b)/(nr*nc);
  if (frac<0.5) {
    delta=0.5-0.5*frac-frac*frac;
    Random(n,delta);
    Or(a,b,n);
  } else {
    delta=2+frac*frac-2.5*frac;
    Random(n,delta);
    And(a,b,n);
  }
  PlaneFree(n);
  return a;
}

TYPE **Shake(TYPE **a,TYPE  **b)
{
/* With special thanks to Ludo Pagie, Stan Maree, and John Couwenberg */
  int i,j,ii,jj;
  int pos,len,target;
  TYPE t;
  len = nrow*ncol;
  pos = 0;
  if (a != b) Copy(a,b);
  for (i=1; i <= nrow; i++)
  for (j=1; j <= ncol; j++) {
    target = pos + len*RANDOM();
    ii = target/ncol;
    jj = target - (ii*ncol) + 1;
    ii++;
    t = a[i][j];
    a[i][j] = a[ii][jj];
    a[ii][jj] = t;
    pos++;
    len--;
  }
  return a;
}
