#include <cash.h>

extern int nrow,ncol,scale;

main()
{
  int i,j;
  TYPE **tmp,**state, **input;

  scale = 3;

  tmp =  New(); state = New(); input = New();

  ColorTable(0,1,BLACK,WHITE);
  OpenDisplay("Shake",nrow,2*ncol);
  for (i=1; i<=20; i++) 
  for (j=1; j<=ncol; j++) 
  if (j <= 50) state[i][j] = 1;
  else state[i][j] = 2;
  printf("%d %d\n",Total(EQ(tmp,state,1)),Total(EQ(tmp,state,2)));
  PlaneDisplay(state,0,0,0);
  Shake(input,state);
  PlaneDisplay(input,0,ncol,0);
  printf("%d %d\n",Total(EQ(tmp,input,1)),Total(EQ(tmp,input,2)));
  CloseDisplay();
}
