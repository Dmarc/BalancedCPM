#include "operation.h"

//alea initialisation

sfmt_t sfmtt[20];//Variables globales. On suppose qu'il n'y a pas plus de 20 températures en parallèles

void InitAlea(int i, long unsigned int seed) {
	sfmt_init_gen_rand(& sfmtt[i], (uint32_t)seed); //time(NULL)); //100); mettre un nombre entier quelconque si on veut générer les mêmes nombres aléatoires
}

void InitAlea2(sfmt_t * sfmt, long unsigned int seed) { //utile uniquement si sfmt est délaré localement dans potts.c, plutôt que d'utiliser les variables globales sfmtt ci-dessus.
	sfmt_init_gen_rand(sfmt, (uint32_t)seed); //(uint32_t)time(NULL));  //100); mettre un nombre entier quelconque si on veut générer les mêmes nombres aléatoires
}



int PeriodicWrap(int value,int max) {//for periodic boundary conditions
	if(value<=max && value>=1)
		return value;
	if(value>max)
		value%=max;
	else{
		value%=max;
		value+=max;
	}
	return value;
}
double D_PeriodicWrap(double value,int max) {//for periodic boundary conditions
	if(value<=max && value>=1)
		return value;
	while(value>max)
		value-=max;
	while(value<1){
		value+=max;
	}
	return value;
}


double aleatoire(int i) {
	return sfmt_genrand_res53(&sfmtt[i]);
}


double aleatoire2(sfmt_t * sfmt) {//utile uniquement si sfmt est délaré localement dans potts.c, plutôt que d'utiliser les variables globales sfmtt ci-dessus.
	return sfmt_genrand_res53(sfmt);
}

void AddIfNotIn(int possible_state[][2], int s, int* pos_neg) {//Add state "s" if "s" not in possible_state. Increase number of neighbours. pos_neg is a pointer to an int which contains the position of the first negative value.
	for (int i=0 ; i<*pos_neg ; i++) {
		if (possible_state[i][0] == s) {
			possible_state[i][1] = possible_state[i][1]+1;
			return ;
		}
	}
	possible_state[*pos_neg][0] = s;
	possible_state[*pos_neg][1] = 1;
	*pos_neg = *pos_neg+1;
}

void init_state_table(const int nx[20], const int ny[20], int nrow, int ncol, int x, int y, int neighbour_copy, int nb_nei_id, int state_table[nb_nei_id][2], TYPE **state, int pixel, bool condwrap) {

	int xpos, ypos, num_nei = 0;
	if (condwrap) {
		for (int iposs=0;iposs<neighbour_copy;iposs++) {
			xpos = PeriodicWrap(x+nx[iposs], ncol); ypos = PeriodicWrap(y+ny[iposs], nrow);
			if (state[ypos][xpos] == pixel) {
				state_table[num_nei][0] = xpos ; state_table[num_nei][1] = ypos;
				num_nei += 1;
				if (num_nei == nb_nei_id)
					break;
			}
		}
	} else {
		for (int iposs=0;iposs<neighbour_copy;iposs++) {
			xpos = x+nx[iposs] ; ypos = y+ny[iposs];
			if (state[ypos][xpos] == pixel) {
				state_table[num_nei][0] = xpos ; state_table[num_nei][1] = ypos;
				num_nei += 1;
				if (num_nei == nb_nei_id)
					break;
			}
		}
	}
}

bool IsIn(int nb_nei_id, int x, int y, int state_table[nb_nei_id][2]) {
	for (int i=0;i<nb_nei_id;i++) {
		if (x == state_table[i][0] && y == state_table[i][1]) {
			return true;
		}
	}
	return false;
}

void init_possible_state(int mediumcell, int neighbour_order, int possible_state[neighbour_order][2], int ncol, int nrow,TYPE **state, int *pos_neg, bool condwrap, int x, int y, const int nx[], const int ny[]) {
	int s;
	for (int k=0 ; k<neighbour_order ; k++) {
		possible_state[k][0] = mediumcell-1;
	}
	if (!(condwrap)) {
		for (int iposs=0 ; iposs<neighbour_order ; iposs++) {// State of neighbours.
			s = state[y+ny[iposs]][x+nx[iposs]];
			AddIfNotIn(possible_state, s, pos_neg);
		}
	} else {
		for (int iposs=0 ; iposs<neighbour_order ; iposs++) {
			s = state[PeriodicWrap(y+ny[iposs],nrow)][PeriodicWrap(x+nx[iposs],ncol)];
			AddIfNotIn(possible_state, s, pos_neg);
		}
	}
}
