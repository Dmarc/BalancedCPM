#include <cash.h>

extern int nrow, ncol;

main()
{
  int i,j,m;
  TYPE **world;

  nrow = ncol = 25;
  m = nrow/5;

  for (i=16; i <= 255; i++)
    ColorTable(i,i,GRAY);
  ColorTable(16,23,BLACK,WHITE,RED,GREEN,BLUE,YELLOW,MAGENTA,GRAY);
  ColorRamp(32,47,BLACK);
  ColorRamp(48,63,RED);
  ColorRamp(79,64,BLUE);
  ColorRamp(80,95,GREEN);
  ColorWheel(144,175);
  ColorRandom(192,223);
  ColorDump("color.cmap");
  OpenDisplay("color",16*(nrow+m)+m,16*(ncol+m)+m);
  OpenPostscript("color.ps",16*(nrow+m)+m,16*(ncol+m)+m);

  world = New();
  for (i=0; i<16; i++)
  for (j=0; j<16; j++) {
    PlaneDisplay(world,m+i*(nrow+m),m+j*(ncol+m),i*16+j);
    PlanePostscript(world,m+i*(nrow+m),m+j*(ncol+m),i*16+j);
  }
  ClosePostscript();
  CloseDisplay();
}
