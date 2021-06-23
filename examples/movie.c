#include <cash.h>

extern int nrow, ncol, scale;

main(argc, argv)
int argc; char *argv[];
{
  int ncol, nrow, offset;
  TYPE **plane;

  offset = 0;
  scale = 1;

  if (argc < 2) {
    printf("Supply filename\n");
    exit(0);
  }
  if (argc > 2) ColorRead(argv[2]);  /* Read colortable */

  LoadMovie(argv[1],&nrow,&ncol);
  printf("Opening %s with %d rows x %d columns\n",argv[1],nrow,ncol);
  plane = New();

  OpenDisplay(argv[1],nrow,ncol);
  OpenPNG("Frames",nrow,ncol);

  while (PlayMovie(offset) != EOF) {
    MoviePlane(plane);
    PlanePNG(plane,offset);
  }

  CloseDisplay();
  CloseMovie();
  ClosePNG();
}
