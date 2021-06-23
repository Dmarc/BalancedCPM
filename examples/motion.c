#include <cash.h>

extern int nrow,ncol;

main(argc, argv)
int argc; char *argv[];
{
  int i,j,time,margin,radius=25;
  TYPE **a;

  if (argc>1) {
    ReadOptions(argv[1]);
    InDat("%d","radius",&radius);
  }

  margin = 0;
  ColorTable(16,18,BLACK,WHITE,RED);
  OpenDisplay("Motion",nrow+2*margin,ncol+2*margin);

  a = New();

  for(i=-radius; i<=radius; i++)
    for(j=-radius; j<=radius; j++) {
      if (i*i + j*j <= radius*radius) a[nrow/2+i][ncol/2+j] = 1;
      if (i > 0 &&  a[nrow/2+i][ncol/2+j]) a[nrow/2+i][ncol/2+j] = 2;
    }
  PlaneDisplay(a,margin,margin,16);

  printf("Particles: %ld\n",Total(a));

  for (time = 1; !Mouse() && time <= 10000; time++) {
    Motion(a,a,(float)1,time);
    PlaneDisplay(a,margin,margin,16);
  }
  printf("Particles: %ld\n",Total(a));
  CloseDisplay();
}
