unsigned char matrix[8][32];

extern int scale;

main()
{
  int i,j,color;
  for (i=0; i<8; i++)
  for (j=0; j<32; j++)
    matrix[i][j] = color = i*32+j + 1;

  scale = 10;
  ColorWheel(1,255);
  OpenDisplay("array",10,32);
  BlockDisplay(matrix,8,32,0,0,0);
  while (!Mouse());
  BlockDisplay(matrix[7],1,32,9,0,0);
  CloseDisplay();

  OpenPostscript("array.ps",8,32);
  BlockPostscript(matrix,8,32,0,0,0);
  ClosePostscript();
}
