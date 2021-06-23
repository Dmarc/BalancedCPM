#include <cash.h>

extern int nrow,ncol;

TYPE **Brownian(cnew, c, cw, ccw, opp, ran)
TYPE **cnew, **c, **cw, **ccw, **opp, **ran;
{
  int i,j;
  for (i=1; i <= nrow; i++)
  for (j=1; j <= ncol; j++)
    if (ran[i][j]) cnew[i][j] = ccw[i][j];
    else cnew[i][j] = cw[i][j];
  return cnew;
}

TYPE **Diagonal(cnew, c, cw, ccw, opp, ran)
TYPE **cnew, **c, **cw, **ccw, **opp, **ran;
{
  cnew = opp;
  return cnew;
}

main(argc, argv)
int argc; char *argv[];
{
  int i,j,time,margin,radius=25,brownian=1;
  int tot;
  TYPE **a;

  if (argc>1) {
    ReadOptions(argv[1]);
    InDat("%d","brownian",&brownian);
  }

  margin = nrow/10;
  ColorTable(16,18,BLACK,WHITE,RED);
  OpenDisplay("Margolus",nrow+2*margin,ncol+2*margin);

  a = New();

  for(i=-radius; i<=radius; i++)
    for(j=-radius; j<=radius; j++)
      if (i*i + j*j <= radius*radius) a[nrow/2+i][ncol/2+j] = 1;

  printf("Particles: %ld\n",tot=Total(a));

  for (time = 1; !Mouse() && time <= 10000; time++) {
    if (brownian) Margolus(a,a,Brownian,time);
    else Margolus(a,a,Diagonal,time);
    PlaneDisplay(a,margin,margin,16);
  }
  printf("Particles: %ld\n",Total(a));
  CloseDisplay();
}
