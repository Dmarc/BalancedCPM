#include "cash.h"

#define BW 1
#define MIN(A,B) ((A) < (B) ? (A) : (B))

FILE *mf;

extern int nrow, ncol, psborder, psreverse;
extern int userCol[256][4];
static float linewidth;

int PagePostscript() 
{
  fprintf(mf,"gsave showpage grestore\n");
}

int ClosePostscript()
{ 
  fprintf(mf,"showpage\n");
  fclose(mf);
}

int OpenPostscript(char name[],int y,int x)
{ 
  float scale, yscale;
  scale = 500.0/x;
  yscale = 750.0/y;
  if (scale > yscale) scale = yscale; 
  linewidth = 0.2/scale;
  mf = fopen(name,"w");
  fprintf(mf,"%%!\n");
  fprintf(mf,"%%%%BoundingBox: 0 0 550 800\n");
  fprintf(mf,"/courier {/Courier findfont 1 scalefont setfont show}def\n");
  fprintf(mf,"/italics {/Courier-Oblique findfont 1 scalefont setfont show}def\n");
  fprintf(mf,"/symbol {/Symbol findfont 1 scalefont setfont show}def\n");
  fprintf(mf,"25 775 translate\n");
  fprintf(mf,"%8.4f %8.4f scale\n",scale,-scale);
  fprintf(mf,"/g {setgray}def\n");
  fprintf(mf,"/c {setrgbcolor}def\n");
  fprintf(mf,"/f {closepath fill}def\n");
  fprintf(mf,"/m {newpath moveto}def\n");
  fprintf(mf,"/d {lineto currentpoint stroke moveto}def\n");
  fprintf(mf,"/r{/h exch def /w exch def /y exch def /x exch def\n");
  fprintf(mf,"   newpath x y moveto w 0 rlineto 0 h rlineto\n");
  fprintf(mf,"   w neg 0 rlineto closepath fill}def\n");
  fprintf(mf,"1 setlinecap newpath 0 0 moveto\n");
  fprintf(mf,"%8.3f setlinewidth\n",linewidth);
}

int TextPostscript(char txt[],int y,int x)
{ /* graphic text */
  fprintf(mf,"0 g %d %d m\n",x,y);
  fprintf(mf,"1 -1 scale ");
  fprintf(mf,"0 g (%s) courier ",txt);
  fprintf(mf,"1 -1 scale\n");
}

int psrect(int x,int y,int dx,int dy,int col,int off)
{
  int c; 
  c = col + off;
  if (userCol[c][0]) {
    if (userCol[c][0] == BW) {
      if (psreverse) fprintf(mf,"%8.5f g ",1.0-userCol[c][1]/255.);
      else fprintf(mf,"%8.5f g ",userCol[c][1]/255.);
    }else
      fprintf(mf,"%8.5f %8.5f %8.5f c ",(float)userCol[c][1]/255.,
       (float) userCol[c][2]/255.,(float)userCol[c][3]/255.);
    fprintf(mf,"%d %d %d %d r\n",x,y,dx,dy);
  }else
    if (col) fprintf(mf,"0 g %d %d %d %d r\n",x,y,dx,dy);

  return (c);
}

int PlanePostscript(TYPE **matrix,int aty,int atx,int off)
{
  int i,j,oldc,newc,oldj;
  int nr, nc;
  nr = nrow;
  nc = ncol;
  fprintf(mf,"%d %d translate 0 g\n",atx,aty);
  for (i=1; i <= nr; i++) {
    oldc = matrix[i][1];
    oldj = 1;
    for (j=2; j <= nc;j++) {
      newc = matrix[i][j];
      if (newc != oldc) {
        psrect(oldj-1,i-1,j-oldj,1,oldc,off);
        oldj = j;
        oldc = newc;
      }
    }
    psrect(oldj-1,i-1,j-oldj,1,oldc,off);
  }
  if (psborder) {
    fprintf(mf,"0 g\n");
    fprintf(mf,"%5.1f %5.1f m\n",-linewidth,-linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",nc+linewidth,-linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",nc+linewidth,nr+linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",-linewidth,nr+linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",-linewidth,-linewidth);
  }
  fprintf(mf,"%d %d translate 0 g\n",-atx,-aty);
}

int RowPostscript(TYPE *matrix,int aty,int atx,int off)
{
  int i,j,oldc,newc,oldj;
  int nc;
  nc = ncol;
  fprintf(mf,"%d %d translate 0 g\n",atx,aty);  
  oldc = matrix[1];
  oldj = 1;
  for (j=2; j <= nc;j++) {
    newc = matrix[j];
    if (newc != oldc) {
      psrect(oldj-1,0,j-oldj,1,oldc,off);
      oldj = j;
      oldc = newc;
    }
  }
  psrect(oldj-1,0,j-oldj,1,oldc,off);
  fprintf(mf,"%d %d translate 0 g\n",-atx,-aty);
}

int BlockPostscript(unsigned char *matrix,int nr,int nc,int aty,int atx,int off)
{
  int i,j,oldc,newc,oldj;
  fprintf(mf,"%d %d translate 0 g\n",atx,aty);
  for (i=0; i < nr; i++) {
    oldc = matrix[i*nc];
    oldj = 0;
    for (j=1; j < nc;j++) {
      newc = matrix[i*nc+j];
      if (newc != oldc) {
        psrect(oldj,i,j-oldj,1,oldc,off);
        oldj = j;
        oldc = newc;
      }
    }
    psrect(oldj,i,j-oldj,1,oldc,off);
  }
  if (psborder) {
    fprintf(mf,"0 g\n");
    fprintf(mf,"%5.1f %5.1f m\n",-linewidth,-linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",nc+linewidth,-linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",nc+linewidth,nr+linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",-linewidth,nr+linewidth);
    fprintf(mf,"%5.1f %5.1f d\n",-linewidth,-linewidth);
  }
  fprintf(mf,"%d %d translate 0 g\n",-atx,-aty);
}
