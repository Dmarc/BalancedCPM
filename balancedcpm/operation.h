#ifndef OPERATION_DEF
#define OPERATION_DEF

#include <cash.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "SFMT.h"
#include "SFMT-params2281.h"

void InitAlea(int i, long unsigned int seed);
void InitAlea2(sfmt_t * sfmt, long unsigned int seed);
double aleatoire(int i);
double aleatoire2(sfmt_t * sfmt);
int PeriodicWrap(int value,int max);
double D_PeriodicWrap(double value,int max);
void AddIfNotIn(int possible_state[][2], int s, int* pos_neg);
void init_state_table(const int nx[20], const int ny[20], int nrow, int ncol, int x, int y, int neighbour_copy, int nb_nei_id, int state_table[nb_nei_id][2], TYPE **state, int pixel, bool condwrap);
bool IsIn(int nb_nei_id, int x, int y, int state_table[nb_nei_id][2]);
void init_possible_state(int mediumcell, int neighbour_order, int possible_state[neighbour_order][2], int ncol, int nrow,TYPE **state, int *pos_neg, bool condwrap, int x, int y, const int nx[], const int ny[]);
#endif
