#ifndef ALLOCATE_DEF
#define ALLOCATE_DEF

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "operation.h"

typedef struct cell {
	double *xcoord;
	double *ycoord;
	double *temp;
	int *targetarea;
	int *area;
	int *celltype;
	int *perimeter;
	int *surf_energy; //cells.sur_energy=J*cells.perimeter pour une mousse, mais pas pour un tissu.
	//int *oldperimeter; Unused
	int *nneighbours;
	int **neighbours;
} Cell;

Cell AllocateCells(int n, int maxneighbour);
void FreeCells(Cell cells, int n);
void Duplicate(Cell copie, Cell original, int maxcells);

int PutCell(TYPE **plane, int y, int x, TYPE m, int ncol, int nrow, int side1, int side2);
void InitBubblePlane(int init_config, float fillfactor,int nrow,int ncol, int target_area, double a1, double a2, TYPE **state, Cell cells, int sliding);
int AssignNormalTargetarea(int mean, double mu2adim, int minimum);
int GeneratePolydispersity(int polydispersity, int blob, int maxcells, double fillfactor, int nrow, int ncol, int target_area, double targetareamu2, int target_area2, double alpha, Cell cells);

#endif
