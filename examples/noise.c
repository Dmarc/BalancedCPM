#include <cash.h>

extern int nrow,ncol;

main(argc, argv)
int argc; char *argv[];
{
  int time = 0, margin = 10, noisebox = 0;
  TYPE **plane;

  if (argc>1) ReadOptions(argv[1]);
  printf("Use NoiseBox (0/1)?");
  scanf("%d",&noisebox);
  
  if (noisebox) plane = New(); 
  else plane = New();
  
  ColorTable(0,1,BLACK,WHITE);
  OpenDisplay("noise",2*margin+nrow,2*margin+ncol);
  for (time = 1; time <= 1000; time++) {
    if (noisebox) NoiseBox(plane);
    else Random(plane,0.5);
    PlaneDisplay(plane,margin,margin,0);
  }
  CloseDisplay();
}
