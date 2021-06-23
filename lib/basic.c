#include "cash.h"

FILE *parfile;

int seed=55;
int seedset=0;
int graphics=1;
int nrow=100;
int ncol=100;
int first=0;
int last=0;
int boundary=0;
int boundaryvalue=0;
int scale=1;
int ranmargolus=1;
int psborder=1;
int psreverse=1;

int InDat(char *format,char *text,void *value)
{
  FullInDat(parfile,format,text,value,1);
}

int FullInDat(FILE *fp,char *format,char *text,void *value,int nvalues)
{
  int j;
  char line[80];

  rewind(fp);
  while (fscanf(fp,"%s",line) != EOF) {
    if (!strcmp(line, text)) {
      for (j=0; j<nvalues; j++)
        fscanf(fp,format,&value[j]);
      return (j);
    }
    while (getc(fp) != '\n');
  }
  return (0);
}

int ReadOptions(char filename[])
{
  parfile = fopen(filename,"r");
  if (parfile == NULL) {
    fprintf(stderr,"%s: File does not exist!\n",filename);
    exit(1);
  }
  FullInDat(parfile,"%d","nrow",&nrow,1);
  FullInDat(parfile,"%d","ncol",&ncol,1);
  FullInDat(parfile,"%d","boundary",&boundary,1);
  FullInDat(parfile,"%d","boundaryvalue",&boundaryvalue,1);
  FullInDat(parfile,"%d","graphics",&graphics,1);
  FullInDat(parfile,"%d","scale",&scale,1);
  FullInDat(parfile,"%d","seed",&seed,1);
  FullInDat(parfile,"%d","psborder",&psborder,1);
  FullInDat(parfile,"%d","psreverse",&psreverse,1);
  FullInDat(parfile,"%d","ranmargolus",&ranmargolus,1);
  if (!seedset) {
    SEED(seed);
    seedset = 1;
  }
}

TYPE **NewP()
{
  TYPE **a;
  a = (TYPE **)calloc(nrow+2, sizeof(TYPE *));
  if (a == NULL) {
    fprintf(stderr,"NewP: error in memory allocation\n");
    exit(1);
  }
  return a;
}

TYPE **New()
{
  TYPE **a;
  int i,j;
  a = NewP(); 
  a[0] = (TYPE *)calloc((nrow+2)*(ncol+2),sizeof(TYPE));
  if (a[0] == NULL) {
    fprintf(stderr,"Error in memory allocation\n");
    exit(1);
  }
  for (i=1,j=nrow+2; i < j; i++)
    a[i] = a[i-1] + ncol + 2;

  if (first == 0) {
    first = ncol + 3;
    last = (nrow+2)*(ncol+2) - ncol - 4;
  }
  return a;
}

int PlaneFree(TYPE **a)
{
  free(a[0]);
  free(a);
  return 0;
}

TYPE **Fill(TYPE **a,TYPE c)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = c;
  return a;
}

long Total(TYPE **a)
{
  int i,j;
  long k=0;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
    k += a[i][j];
  return k;
}

int Equal(TYPE **a,TYPE  **b)
{
  int i,j,e=1;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    if (a[0][i] != b[0][i]) e=0;
  return e;
}

int Max(TYPE **a)
{
  int i,j,k;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  k=a[1][1];
  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
    if (a[i][j] > k) k = a[i][j];
  return k;
}

int Min(TYPE **a)
{
  int i,j,k;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  k=a[1][1];
  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
    if (a[i][j] < k) k = a[i][j];
  return k;
}

TYPE **Explode(TYPE **a,TYPE  **b)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      a[i][j] = b[i][j];
  return a;
}

TYPE **Copy(TYPE **a,TYPE  **b)
{
  int i,j;
  /*$dir force_vector*/
  for (i=first,j=last; i <= j; i++)
    a[0][i] = b[0][i];
  return a;
}

TYPE *CopyRow(TYPE *a,TYPE  *b)
{
  int i,j;
  /*$dir force_vector*/
  for (i=1,j=ncol; i <= j; i++) 
    a[i] = b[i];
  return a;
}

static int diffinit = 0;
static TYPE **diff;

#define DIFF(A,B) ((A) > (B) ? ((A)--,(B)++) : (A) < (B) ? ((A)++,(B)--) : 0)
#define DFBD(A,B) ((A) > (B) ? (B)++ : (A) < (B) ? (B)-- : 0)

TYPE **Diffusion(TYPE **a,TYPE **b,float d)
{
  int i, j, nc, nr;
  nr = nrow;
  nc = ncol;
  if (!diffinit) {
    diff = New();
    diffinit = 1;
  }
  VonNeumann4(diff,b);
  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      a[i][j] = b[i][j] + d*diff[i][j] - 4*d*b[i][j] + 0.5;
  return a;
}

static TYPE T;
#define ROTR(A,B,C,D) {T = A; A = C; C = D; D = B; B = T;}
#define ROTL(A,B,C,D) {T = A; A = B; B = D; D = C; C = T;}
#define SWAP(A,B) {T = A; A = B; B = T;}

TYPE **Motion(TYPE **a,TYPE **b,float d,int time)
{
  int i,ii,j,jj,nr,nc,nr1,nc1;
  int mode;
  double u;
  if (nrow & 1 || ncol & 1)  {
    fprintf(stderr,"Motion: use an even number of rows and columns (%d,%d)\n",nrow,ncol);
    exit(1);
  }
  if (d > 1) d = 1.0;
  mode = time & 1;
  nr = nrow; nr1 = nrow+1;
  nc = ncol; nc1 = ncol+1;
  if (a != b) Copy(a,b);
  a = Boundaries(a);
  /*
  if (nrow < 5) {
    printf("\nTime:%d\n",time);
    for (i=0; i<=nr1; i++) {
      for (j=0; j <=nc1; j++)
	printf("%2d",a[i][j]);
      printf("\n");
    }
  }
  */

  if (mode && boundary == ECHO) { /* Messy part */
    for (i=2; i < nr; i+=2) {
      ii = i + 1;
      /*$dir force_vector*/
      for (j=2; j < nc; j+=2) {
	if ((u = RANDOM()) < d) { 
	  jj = j + 1;
	  if (u < d/2.0) ROTR(a[i][j],a[i][jj],a[ii][j],a[ii][jj])
	  else ROTL(a[i][j],a[i][jj],a[ii][j],a[ii][jj])
	}
      }
    } 
    /*$dir force_vector*/
    for (i=2; i < nr; i+=2) {
      ii = i + 1;
      if (RANDOM() < d/2.) SWAP(a[i][1],a[ii][1]);
      if (RANDOM() < d/2.) SWAP(a[i][nc],a[ii][nc]);
    } 
    /*$dir force_vector*/
    for (j=2; j < nc; j+=2) {
      jj = j + 1;
      if (RANDOM() < d/2.) SWAP(a[1][j],a[1][jj]);
      if (RANDOM() < d/2.) SWAP(a[nr][j],a[nr][jj]);
    }
  }
  else { /* Normal case */
    for (i=1-mode; i <= nr; i+=2) {
      ii = i + 1;
      /*$dir force_vector*/
      for (j=1-mode; j <= nc; j+=2) {
	if ((u = RANDOM()) < d) { 
	  jj = j + 1;
	  if (u < d/2.0) ROTR(a[i][j],a[i][jj],a[ii][j],a[ii][jj])
	  else ROTL(a[i][j],a[i][jj],a[ii][j],a[ii][jj])
	}
      }
    }
  }
  if (mode && boundary == WRAP) {
    /*$dir force_vector*/
    for (i=2; i <= nr; i++) 
      a[i][1] = a[i][nc1];
    /*$dir force_vector*/
    for (j=2; j <= nc; j++) 
      a[1][j] = a[nr1][j];
    a[1][1] = a[nr1][nc1];
  }
  return a;
}
