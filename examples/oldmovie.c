#include <cash.h>

extern int nrow, ncol, scale;

main(argc, argv)
int argc; char *argv[];
{
  int width, height, offset;

  offset = 0;
  scale = 1;

  if (argc < 2) {
    printf("Supply filename\n");
    exit(0);
  }
  if (argc > 2) ColorRead(argv[2]);  /* Read colortable */

  LoadMovie(argv[1],&height,&width);
  printf("Opening %s with %d rows x %d columns\n",argv[1],height,width);
  OpenDisplay(argv[1],height,width);

  PlayMovie(offset);
  Ask(argv[1]);

  while (1) {
    if (PlayMovie(offset) == EOF) {
      printf("End of file: ");
      Ask(argv[1]);
    }
    if (Mouse()) Ask(argv[1]);
  }

  CloseDisplay();
  CloseMovie();
}

int Ask(file)
char file[];
{
  char answer[2];
  int *w,*h;
  printf("Enter: Rewind (r), Quit (q) or Continue (c)?: ");
  scanf("%s",answer);
  if (answer[0] == 'q') {
    exit(0);
  }else if (answer[0] == 'r') {
    CloseMovie();
    LoadMovie(file,&w,&h);
    return 0;
  }else if (answer[0] == 'c') {
    return 0;
  }else
    printf("Unknown: %s\n",answer);
  return 0;
}
