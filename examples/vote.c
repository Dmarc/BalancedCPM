#include <cash.h>

extern int nrow,ncol;

main(argc, argv)
int argc; char *argv[];
{
  int time, margin;
  TYPE **a, **b;

  if (argc>1) ReadOptions(argv[1]);
  a = New(); b = New();
  margin = nrow/10;
  ColorTable(0,1,BLACK,WHITE);
  OpenDisplay("Majority Vote",nrow+2*margin,3*ncol+4*margin);

  Random(a,0.5);
  PlaneDisplay(a,margin,margin,0);
  printf("Number of bits: %ld\n",Total(a));

  for (time = 1; !Equal(a,b) && !Mouse() && time<100; time++) {
    Copy(b,a);
    Moore9(a,b);
    GE(a,a,5);
    PlaneDisplay(a,margin,2*margin+ncol,0);
  }
  printf("Number of bits: %ld\n",Total(a));

  for (time = 1; !Mouse() && time<1000; time++) {
    Moore9(b,a);
    PLANE(a[i][j] = (b[i][j] == 4 || b[i][j] >= 6););
    PlaneDisplay(a,margin,3*margin+2*ncol,0);
  }
  printf("Number of bits: %ld\n",Total(a));

  CloseDisplay();
}
