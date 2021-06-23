#include <cash.h>

extern int nrow,ncol,scale;

main()
{
  int time;
  TYPE **state, **input;

  nrow = 100;
  ncol = 100;
 scale =4;	
  state = New(); input = New();

  ColorTable(0,1,BLACK,WHITE);
  OpenDisplay("Modulo prime",nrow,ncol);
  Init(state);

  for (time = 1; time <= 10000; time++) {
	Init(state);
    PlaneDisplay(state,0,0,0);
    Moore8(input,state);
//    PLANE(state[i][j] = (input[i][j] == 3 || (input[i][j] == 2 && state[i][j])););
    PLANE(state[i][j] = (input[i][j]%4););
 //   PlaneDisplay(state,0,0,0);
//    if (Mouse()) Init(state);
  }
  CloseDisplay();
}
