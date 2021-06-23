#include "cash.h"

extern int nrow, ncol, first, last;

int iPrint(FILE *file,char format[],TYPE **a)
{
  int i,j;
  int nc, nr;
  nr = nrow;
  nc = ncol;
  for (i=1; i <= nr; i++) {
    for (j=1; j <= nc; j++)
      fprintf(file,format,(int)a[i][j]);
    fprintf(file,"\n");
  }
  fprintf(file,"\n");
}

int cPrint(FILE *file,TYPE **a)
{
  int i,j;
  int nc, nr;
  char *Line;
  nr = nrow;
  nc = ncol;
  Line=(char*)calloc(ncol,sizeof(char));
  for (i=1; i <= nr; i++) {
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      Line[j-1] = a[i][j]+97;
    fprintf(file,"%s\n",Line);
  }  
}

int bPrint(FILE *file,TYPE **a)
{
  int i,j;
  int nc, nr;
  char *Line;
  nr = nrow;
  nc = ncol;
  Line=(char*)calloc(ncol,sizeof(char));
  for (i=1; i <= nr; i++) {
    /*$dir force_vector*/
    for (j=1; j <= nc; j++)
      if (a[i][j]) Line[j-1] = 'o';
      else Line[j-1] = '.';
    fprintf(file,"%s\n",Line);
  }  
}

TYPE **ReadPat(TYPE **a,int destrow,int destcol,char *filename)
{
  int i,j,row,col,k;
  FILE *file;
  file=fopen(filename,"r");
  if (file==NULL) {
    fprintf(stderr,"%s: file not found!\n",filename);
    Fill(a,0);
    return a;
  }
  fscanf(file,"%d %d",&row,&col);
  for(i=0; i < row; i++) 
    for(j=0; j < col; j++) { 
      fscanf(file,"%d",&k);
      a[destrow+i][destcol+j]=k;
    }
  fclose(file);
  return a;
}

TYPE **Init(TYPE **a)
{
  char c, answer[2], filename[128];
  int row,col;
  float p;
  printf("Enter: Read (r),  InitRandom (i), Continue (c), or Quit (q)?: ");
  scanf("%s",answer);
  switch (c = answer[0]) {
  case 'r':
    printf("Enter filename: ");
    scanf("%s",filename);
    printf("Where to put this (NROW=%d,NCOL=%d)? Enter row col: ",nrow,ncol);
    scanf("\n%d %d",&row,&col);
    printf("\n");
    Fill(a,0);
    ReadPat(a,row,col,filename);
    break;
  case 'i':
    printf("Enter fraction on: ");
    scanf("%f",&p);
    Random(a,p);
    break;
  case 'c':
    break;
  case 'q':
    exit(0);
  default:
    printf("Unknown: %s\n",answer);
  }
  return a;
}
