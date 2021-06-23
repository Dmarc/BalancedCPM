#include <cash.h>

#define N 8

extern int nrow,ncol,scale;

main()
{
  int i, max;
  TYPE **a[N+1], **b;

  ncol = nrow = 25;
  scale = 100/ncol;

  ColorWheel(1,255);
  OpenDisplay("Moore",nrow,(N+1)*ncol);

  for (i=0; i<=N; i++)
    a[i] = New();
  b = New();

  a[0][1+nrow/2][1+ncol/2] = 1;

  for (i=0; i <= N; i++) {
    if (i) Moore9(a[i],a[i-1]);
    printf("Moore: %d Maximum value: %d\n",i,max = Max(a[i]));
    MultF(b,a[i],(float)255/max);
    PlaneDisplay(b, 0, i*ncol, 0);
  }
  CloseDisplay();
}
