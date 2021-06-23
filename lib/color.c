#include <math.h>
#include <stdarg.h>
#include "cash.h"

double wheelsteep = 2.0;
double wheelscale = 0.5;

int userCol[2000][4], randCol[2000][4];
int fixedCol[8][3] = {0,0,0, 255,255,255, 255,0,0, 0,255,0, 0,0,255,
255,255,0, 255,0,255, 100,100,100};

int ColorTable(int s,int e, ...)
{
  va_list ap;        /* points to each unnamed arg in turn */
  int i,n,cnt,col;

  /* some checks, and now hope the user supplied enough arguments */
  if ( (s>e) || (s<0) || (e>255) ) {
    printf("Error: ColorTable has changed into ColorTable(start,end,colors)\n");
    fprintf(stderr, "error in funtion 'ColorTable(...)':\n");
    fprintf(stderr, "\tstart- or end-value out-of-range (%d, %d)\n",s,e);
  }

  n = e - s + 1;
  if (n>=1) {
    va_start(ap,e);  /* make ap point to first unnamed arg */

    for(cnt=0;cnt<n;cnt++) {   /* loop over 'n' userdefined colors */
      col=va_arg(ap,int);      /* retrieve value from next unnamed arg */
      if (col<0 || col>7) {
	fprintf(stderr,"error in function ColorTable(...):\n");
	fprintf(stderr,"\tcolor-arg %d out-of-range (%d)\n",cnt+1,col);
      }
      userCol[s+cnt][0] = 3;
      if (col == WHITE || col == BLACK || col == GRAY) userCol[s+cnt][0] = 1;
      for (i=0; i<3; i++) userCol[s+cnt][i+1] = fixedCol[col][i];
    }
    
    va_end(ap); /* clean up */
  }
}

int ColorRamp(int s,int e,int c)
{
  int i,ii,n0,n1,n2,s0,s1,s2,ss=1;
  int i1,i2,i3,bw;
  int n;

  n = e - s + 1;
  if (s > e) {ss = -1; n = 2 - n;}
  n0 = n1 = n2 = n/3;
  if ((n0+n1+n2) < n) n0++;
  if ((n0+n1+n2) < n) n1++;
  s0 = 255/n0; s1 = 255/n1; s2 = 255/n2;

  if (c == WHITE || c == BLACK || c == GRAY) bw = 1; else bw = 0;
  for (i=0; i < n; i++) {
    ii = s + ss*i; 
    if (bw) userCol[ii][0] = 1;
    else userCol[ii][0] = 3;
  }
  if (bw) {
    for (i=1; i <= n; i++) {
      ii = s + ss*(i-1);
      if (c == WHITE) userCol[ii][1] = userCol[ii][2] = userCol[ii][3] = 255 - i*255/n;
      else userCol[ii][1] = userCol[ii][2] = userCol[ii][3] = i*255/n;
    }
  } else {
    if (c == RED) {i1=1; i2=2; i3=3;}
    else if (c == BLUE) {i1=3; i2=2; i3=1;}
    else if (c == GREEN) {i1=2; i2=1; i3=3;}
    else printf("stderr","Unknown color in Ramp\n");
    for (i=1; i <= n0; i++) {
      ii = s + ss*(i-1); 
      userCol[ii][i1] = i*s0; userCol[ii][i2] = userCol[ii][i3] = 0;
    }
    for (i=1; i <= n1; i++) {
      ii = s + ss*(i+n0-1); 
      userCol[ii][i2] = i*s1; userCol[ii][i1] = 255; userCol[ii][i3] = 0;
    }
    for (i=1; i <= n2; i++) {
      ii = s + ss*(i+n0-1+n1);
      userCol[ii][i3] = i*s2; userCol[ii][i1] = userCol[ii][i2] = 255;
    }
  }
}

int ColorWheel(int s,int e)
{
  int i,j,off[4];
  double d,x;
  int n;
  n = e - s + 1;
  off[1] = 0;
  off[2] = n/3;
  off[3] = 2*n/3;
  d = 6.2832/n;
  for (i=0; i < n; i++) {
    userCol[s+i][0] = 3;
    for (j=1; j <= 3; j++) {
      userCol[s+i][j] = 0;
      x = cos((double)(i-off[j])*d) + 1.0;
      userCol[s+i][j] = wheelscale*pow(x,wheelsteep)*255;
      if (userCol[s+i][j] > 255) userCol[s+i][j]= 255;
    }
    /* printf("%d %d %d\n",userCol[s+i][1],userCol[s+i][2],userCol[s+i][3]); */
  }
}

int ColorRandom(int s,int e)
{
  int i,j,r;
  double d,x;
  int n;
  n = e - s + 1;
  ColorWheel(s,e);
  for (i=0; i < n; i++) {
    randCol[s+i][1] = userCol[s+i][1];
    randCol[s+i][2] = userCol[s+i][2];
    randCol[s+i][3] = userCol[s+i][3];
  }
  for (i=0; i < n; i++) {
    r = RANDOM()*n;
    userCol[s+i][1] = randCol[s+r][1];
    userCol[s+i][2] = randCol[s+r][2];
    userCol[s+i][3] = randCol[s+r][3];
  }
}

int ColorRGB(int i,int r,int g,int b)
{
  userCol[i][0] = 3;
  userCol[i][1] = r;
  userCol[i][2] = g;
  userCol[i][3] = b;
}

int ColorDump(char name[])
{
  FILE *f;
  int i;
  f = fopen(name,"w");
  for (i=0; i <= 255; i++)
    if (userCol[i][0]) fprintf(f,"%3d %3d %3d %3d\n",
      i,userCol[i][1],userCol[i][2],userCol[i][3]);
  fclose(f);
}

int ColorRead(char name[])
{
  FILE *f;
  int i,e;
  f = fopen(name,"r");
  while ((e = fscanf(f,"%d",&i)) != EOF) {
    userCol[i][0] = 3;
    fscanf(f,"%d %d %d",&userCol[i][1],&userCol[i][2],&userCol[i][3]);
  }
  fclose(f);
}
