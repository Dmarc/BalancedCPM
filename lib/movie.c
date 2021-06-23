#include "cash.h"

extern int nrow, ncol, graphics, scale;

static int ifile = 0;
static unsigned char *data,*image;
static int nrs, ncs;

int OpenMovie(char name[],int h,int w)
{
  int e,width,height;
  height = scale*h;
  width = scale*w;
  ifile = creat(name,0644);
  if (ifile < 0) printf("Error opening %s\n",name);
  e = write(ifile,&height,sizeof(int));
  e = write(ifile,&width,sizeof(int));
  data = (unsigned char *)calloc(height*width,sizeof(unsigned char));
  return (e);
}

int LoadMovie(char name[],int *height,int *width)
{
  int w,h,e;
  long size;
  ifile = open(name,0);
  if (ifile < 0) printf("Error opening %s\n",name);
  e = read(ifile,height,sizeof(int));
  e = read(ifile,width,sizeof(int));
  size = (*height)*(*width);
  image = (unsigned char *)calloc(size,sizeof(unsigned char));
  data = (unsigned char *)calloc(size,sizeof(unsigned char));
}

int CloseMovie()
{
  close(ifile);
}

int PlaneMovie(TYPE **a,int y,int x,TYPE c)
{
  int i,j;
  int nrs, ncs;
  long pairs = 0;
  unsigned char len = 0;
  unsigned char color,newcolor;
  color = a[1][1] + c;
  nrs = scale*nrow;
  ncs = scale*ncol;
  for (i=1; i <= nrs; i++)
  for (j=1; j <= ncs; j++) {
    if ((color == (newcolor = a[i/scale][j/scale]+c)) && len < 255) len++;
    else {
      data[2*pairs] = len;
      data[2*pairs+1] = color;
      color = newcolor;
      len = 1;
      pairs++;
    }
  }
  data[2*pairs] = len;
  data[2*pairs+1] = color;
  pairs++;
  writepairs(nrs,ncs,scale*x,scale*y,pairs);
}

int RowMovie(TYPE *a,int y,int x,TYPE c)
{
  int i, j, ncs;
  long pairs = 0;
  unsigned char len = 0;
  unsigned char color,newcolor;
  color = a[1] + c;
  ncs = scale*ncol;
  for (i=1; i <= scale; i++)
  for (j=1; j <= ncs; j++) {
    if ((color == (newcolor = a[j/scale]+c)) && len < 255) len++;
    else {
      data[2*pairs] = len;
      data[2*pairs+1] = color;
      color = newcolor;
      len = 1;
      pairs++;
    }
  }
  data[2*pairs] = len;
  data[2*pairs+1] = color;
  pairs++;
  writepairs(scale,ncs,scale*x,scale*y,pairs);
}

int BlockMovie(unsigned char *a,int nr,int nc,int y,int x,int c)
{
  int i,j,e;
  long pairs = 0;
  int nrs, ncs;
  unsigned char len = 0;
  unsigned char color,newcolor;
  color = a[0] + c;
  nrs = scale*nr;
  ncs = scale*nc;
  for (i=0; i < nrs; i++)
  for (j=0; j < ncs; j++) {
    if ((color == (newcolor = a[(ncs/scale)*(i/scale)+j/scale]+c)) && len < 255) len++;
    else {
      data[2*pairs] = len;
      data[2*pairs+1] = color;
      color = newcolor;
      len = 1;
      pairs++;
    }
  }
  data[2*pairs] = len;
  data[2*pairs+1] = color;
  pairs++; 
  writepairs(nrs,ncs,scale*x,scale*y,pairs);
}

int writepairs(int nr,int nc,int x,int y,long pairs)
{
  int e;
  e = write(ifile,&nr,sizeof(int));
  e = write(ifile,&nc,sizeof(int));
  e = write(ifile,&x,sizeof(int));
  e = write(ifile,&y,sizeof(int));
  e = write(ifile,&pairs,sizeof(long));
  e = write(ifile,data,2*pairs);
}

int PlayMovie(int offset)
{
  int i,j,k,e,mi,mj;
  int len,color;
  int row,col;
  int nr,nc,x,y;
  long pairs;
  e = read(ifile,&nr,sizeof(int));  
  if (e <= 0) return EOF;
  e = read(ifile,&nc,sizeof(int));
  e = read(ifile,&x,sizeof(int));
  e = read(ifile,&y,sizeof(int));
  e = read(ifile,&pairs,sizeof(long));
  if (e <= 0 || pairs <= 0) return EOF;
  e = read(ifile,data,2*pairs);
  if (e <= 0) return EOF;
  if (!graphics) return 1;
  for (k=i=0; i < pairs; i++){
    len = data[2*i];
    color = data[2*i+1];
    for (j=0; j < len; j++,k++)
      image[k] = color + offset;
  }
  if (scale > 1) BlockDisplay(image,nr,nc,y,x,0);
  else BlockDisplayFast(image,nr,nc,y,x);
  return 1;
}

TYPE **MoviePlane(TYPE **a)
{
  int i,j,nc,nr;
  nr = nrow;
  nc = ncol;
  for (i=1; i <= nr; i++)
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      a[i][j] = image[(i-1)*nc + j-1];
  return a;
}
