#include <cash.h>

extern int nrow,ncol,scale;

main()
{
  int time;
  TYPE **state, **input;

  nrow = 256;
  ncol = 456;
 scale =2;	
  state = New(); input = New();

  ColorTable(0,1,BLACK,WHITE);
  OpenDisplay("Game of Life",nrow,ncol);
  Init(state);

  for (time = 1; time <= 10000; time++) {
    Moore8(input,state);
    PLANE(state[i][j] = (input[i][j] == 3 || (input[i][j] == 2 && state[i][j])););
    PlaneDisplay(state,0,0,0);
    if (Mouse()) Init(state);
  }
  CloseDisplay();
}
