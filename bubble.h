#ifndef BUBBLE_DEF

#define BUBBLE_DEF

#include <math.h>
#include <limits.h>
#include <omp.h>
#include <string.h>

#include "allocate.h"

double BubbleHamiltonian(int time, int dispersetime, int num, int neighbour_energy, int neighbour_copy, int neighbour_connected, int ncol, int nrow, TYPE **state, int mediumcell, int heat_bath, int** Jarray, Cell cells, double area_constraint , double temperature);

bool test_only_two(TYPE **state, bool condwrap, int neighbour_copy, int pixel, int icandidate, int ncol, int nrow, int x, int y);

bool connected_4(TYPE **state, int nb_nei_id, bool condwrap, int pixel, int ncol, int nrow, int x, int y);

bool connected_8(TYPE **state, int nb_nei_id, bool condwrap, int pixel, int ncol, int nrow, int x, int y);


void ComputePerimeter(int maxcells, Cell cells, int ncol, int nrow, TYPE **state, int mediumcell, int neighbour_energy);
void FindNeighbours(int maxcells, Cell cells, int ncol, int nrow, TYPE **state, int mediumcell, int neighbour_connnected, int maxneighbours, int* side_interf12, int* side_interf10, int* side_interf20);
double ComputeEnergy(int maxcells, Cell cells, int ncol, int nrow, TYPE **state, int neighbour_energy, int** Jarray, double area_constraint);
void ComputeCenterCoords(Cell cells, int ncol, int nrow, TYPE **state, int nbrcells, int mediumcell);
int ComputeBoundary(Cell cells, int ncol, int nrow, TYPE **state, int neighbour_energy);
void AffichageCouleurs(int affichage, Cell cells, int ncol, int nrow, char *subdirname, char *subdirnameRAW, TYPE **state, TYPE **nstate);
#endif
