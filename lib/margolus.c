#include "cash.h"

extern int nrow, ncol, first, last, ranmargolus;
extern int boundary;

static int initmargolus=0;
static TYPE **c, **cw, **ccw, **opp, **cm, **cwm, **ccwm, **oppm, **ran;

void InitMargolus()
{
  if (nrow & 1 || ncol & 1) {
    fprintf(stderr,"Margolus: Use even values for nrow and ncol\n");
    exit(1);
  }
  if (boundary != WRAP) {
    fprintf(stderr,"Margolus: Use only wrapped boundary conditions\n");
    exit(1);
  }
  nrow /=2;
  ncol /=2;

  ran = New();
  c = New();
  cw = New();
  ccw = New();
  opp = New();
  cm = New();
  cwm = New();
  ccwm = New();
  oppm = New();

  nrow *=2;
  ncol *=2;

  initmargolus = 1;
}

TYPE **Partition(TYPE **b, int mode)
{
  int i, j, nr, nc, nr1, nc1; 
  int bound;

  nr = nrow;
  nc = ncol;

  nr1 = nr + 1;
  nc1 = nc + 1;
  if (mode) {
    for (j=1; j <= nc; j++)
      b[nr1][j] = b[1][j];
    for (i=2; i <= nr1; i++)
      b[i][nc1] = b[i][1];
  }

  nr = nrow /= 2;
  nc = ncol /= 2;
  first = ncol + 3;
  last = (nrow+2)*(ncol+2) - ncol - 4;

  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) {
      cm[i][j] = b[i*2-1+mode][j*2-1+mode];
      cwm[i][j]= b[i*2-1+mode][j*2+mode];
      ccwm[i][j] = b[i*2+mode][j*2-1+mode];
      oppm[i][j] = b[i*2+mode][j*2+mode];
    }
  return c;
}

TYPE **Unpartition(TYPE **a,int mode)
{
  int i, j, nr, nc, nr1, nc1;
  nr = nrow;
  nc = ncol;

  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++) {
      a[i*2-1+mode][j*2-1+mode] = c[i][j];
      a[i*2-1+mode][j*2+mode] = cw[i][j];
      a[i*2+mode][j*2-1+mode] = ccw[i][j];
      a[i*2+mode][j*2+mode] = opp[i][j];
    }

  nr = nrow *= 2;
  nc = ncol *= 2;
  first = ncol + 3;
  last = (nrow+2)*(ncol+2) - ncol - 4;

  nr1 = nr + 1;
  nc1 = nc + 1;
  if (mode) {
    for (i=2; i <= nr1; i++)
      a[i][1] = a[i][nc1];
    for (i=1; i <= nc; i++)
      a[1][i] = a[nr1][i];
  }
  return a;
}

TYPE **Margolus(TYPE **a,TYPE **b,TYPE **(*Rule)(),int time)
{
  int mode;
  mode = time & 1;
  if (!initmargolus) InitMargolus();

  Partition(b,mode);

  if (ranmargolus) Random(ran,0.5);
  c = (*Rule)(c,cm,cwm,ccwm,oppm,ran);
  cw = (*Rule)(cw,cwm,oppm,cm,ccwm,ran);
  ccw = (*Rule)(ccw,ccwm,cm,oppm,cwm,ran);
  opp = (*Rule)(opp,oppm,ccwm,cwm,cm,ran);

  Unpartition(a,mode);

  return a;
}
