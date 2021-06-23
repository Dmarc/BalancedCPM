#include <cash.h>

extern int scale,nrow,ncol;

main(argc, argv)
int argc; char *argv[];
{
  int i, j, time, margin, movie = 1;
  TYPE **frozen, **free, **moore, **tmp;
  float dens = 0.1;

  if (argc>1) {
    ReadOptions(argv[1]);
    InDat("%f","dens",&dens);
    InDat("%d","movie",&movie);
  }

  margin = nrow/10;
  ColorTable(16,19,BLACK,WHITE,RED,GREEN);
  OpenDisplay("Diffussion limited aggregation",nrow+2*margin,3*ncol+4*margin);
  if (movie) OpenPNG("Frames",nrow,ncol);

  frozen = New(); free = New(); moore = New(); tmp = New();

  frozen[nrow/2][ncol/2] = 1;
  Random(free,dens);

  printf("Particles: %ld\n",Total(frozen)+Total(free));

  for (time = 1; !Mouse() && time <= 10000; time++) {
    Motion(free,free,1.0,time);
    Moore8(moore,frozen);
    PLANE(
      if (free[i][j] && moore[i][j] && frozen[i][j] == 0) {
         frozen[i][j] = free[i][j];
         free[i][j] = 0;
      }
    );
    PlaneDisplay(frozen,margin,margin,16);
    PlaneDisplay(free,margin,2*margin+ncol,16);
    BinSum(tmp,2,frozen,free);
    PlaneDisplay(tmp,margin,3*margin+2*ncol,16);
    if (movie) PlanePNG(tmp,16);
  }
  printf("Particles: %ld\n",Total(frozen)+Total(free));
  CloseDisplay();
  if (movie) ClosePNG();
}
