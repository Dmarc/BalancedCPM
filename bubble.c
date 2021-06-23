#include "bubble.h"

//             reseau         4     |  6  |  8  |          16           |    20     |                 36
static const int nx[36]={ 0, 0, 1,-1, 1,-1, 1,-1, 1, 1,-1,-1, 2, 2,-2,-2, 0, 0, 2,-2, 2,-2, 0,-1,-2,-3,-3,-3,-3, 0, 1, 2, 3, 3, 3, 3};
static const int ny[36]={ 1,-1, 0, 0, 1,-1,-1, 1, 2,-2, 2,-2, 1,-1, 1,-1, 2,-2, 0, 0, 2,-2,-3,-3,-3,-3, 0,-1,-2, 3, 3, 3, 3, 0, 1, 2};

//distance between central pixel and neigborhood (neighbour_energy=8 only for now)
//static const double distance[8]={1,1,1,1,sqrt(2),sqrt(2),sqrt(2),sqrt(2)};


double BubbleHamiltonian(int time, int dispersetime, int num, int neighbour_energy, int neighbour_copy, int neighbour_connected, int ncol, int nrow, TYPE **state, int mediumcell, int heat_bath, int** Jarray, Cell cells, double area_constraint , double temperature)
{
	bool sub_flag, nei_flag;
	double sum_E=0;
	int pixel, icandidate, ineighbour, y, x, pos_neg;
	int state_copy[neighbour_copy];
	
	for(int i=0;i<ncol*nrow;i++) {
		pixel=state[y=(int)(aleatoire(num)*nrow) +1][x=(int)(aleatoire(num)*ncol) +1]; //CASH PLANE are from 1 to ncol or nrow.

		//flag to pass PeriodicWrap funtion if far of boundary.
		bool condwrap = true;
		if (x>neighbour_energy/4+2 && x<ncol-2-neighbour_energy/4 && y>neighbour_energy/4+2 && y<nrow-2-neighbour_energy/4)
			condwrap = false;
		
		pos_neg = 0 ; 
		
		
		// State of neighbours.
		int nb_nei_id = 0; // nombre de voisins identiques au pixel central
		if (condwrap) {
			for (int k=0 ; k < neighbour_copy ; k++) {
				if (state[PeriodicWrap(y+ny[k],nrow)][PeriodicWrap(x+nx[k],ncol)] == pixel) {
					nb_nei_id += 1;
				}
			}
		}
		else {
			for (int k=0 ; k < neighbour_copy ; k++) {
				if (state[y+ny[k]][x+nx[k]] == pixel) {
					nb_nei_id += 1;
				}
			}
		}
		if (nb_nei_id < neighbour_copy) { //on ne perd pas de temps si tous les voisins sont identiques au central (pixel non situé à une interface)
			for (int k=0 ; k<neighbour_copy ; k++) {
//				possible_state_copy[k][0] = mediumcell-1;
				state_copy[k]=-1;
			}
			int s;
			//pos_neg contient le nombre de pixels identiques au pixel central dans le voisinage de copie
			if (condwrap) {
				//state_copy[0]=state[PeriodicWrap(y+ny[0], nrow)][PeriodicWrap(x+nx[0], ncol)];
				for (int iposs=0 ; iposs<neighbour_copy ; iposs++) { 
					s = state[PeriodicWrap(y+ny[iposs], nrow)][PeriodicWrap(x+nx[iposs], ncol)];
					for( int jposs=0; jposs<pos_neg+1; jposs++) {
						if (s == state_copy[jposs]) //valeur déjà répertoriée
							break;
						if (state_copy[jposs] == -1){ //valeur à ajouter
							state_copy[jposs] = s;
							pos_neg++;
							break;
						}
					}
					//AddIfNotIn(possible_state_copy, s, &pos_neg);
				}
			}
			else {
				//state_copy[0]=state[y+ny[0]][x+nx[0]];
				for (int iposs=0 ; iposs<neighbour_copy ; iposs++) {
					s = state[y+ny[iposs]][x+nx[iposs]];
					for( int jposs=0; jposs<pos_neg+1; jposs++) {
						if (s == state_copy[jposs]) //valeur déjà répertoriée
							break;
						if (state_copy[jposs] == -1){ //valeur à ajouter
							state_copy[jposs] = s;
							pos_neg++;
							break;
						}
					}
					//AddIfNotIn(possible_state_copy, s, &pos_neg);
				}
			}
			//flag with value true if local connectivity (so, can try a metropolis step).
			nei_flag = true;
			if (pixel != mediumcell) {
			//pixel != mediumcell => mediumcell peut se fragmenter. Nécessaire pour la résorption aux premiers instants. 
			//Egalement nécessaire pour un système non confluent avec cellules pouvant s'agréger. 
				//on teste la connexité locale de la bulle candidate:
				nei_flag = connected_4(state, nb_nei_id, condwrap, pixel, ncol, nrow, x, y);
			}
			if (nei_flag) {						
				double delta_E = 0;
				int delta_surf_E=0;
				int delta_perimeter=0;
				icandidate=state_copy[(int)(aleatoire(num)*pos_neg)];
				if (icandidate != pixel) { //else, delta_E = 0 and we don't make calcul.
					if (condwrap) {
						for(int j=0;j<neighbour_energy;j++){
							ineighbour = state[PeriodicWrap(y+ny[j],nrow)][PeriodicWrap(x+nx[j],ncol)];
							if(pixel != ineighbour) {
								delta_E -= Jarray[cells.celltype[pixel]][cells.celltype[ineighbour]];//pixel never saved.
								delta_perimeter--;
							}
							if(icandidate != ineighbour) {
								delta_E += Jarray[cells.celltype[icandidate]][cells.celltype[ineighbour]];
								delta_perimeter++;
							}
						}
					}
					else {
						for(int j=0;j<neighbour_energy;j++){
							ineighbour = state[y+ny[j]][x+nx[j]];
							if(pixel != ineighbour) {
								delta_E -= Jarray[cells.celltype[pixel]][cells.celltype[ineighbour]];//pixel never saved.
								delta_perimeter--;
							}
							if(icandidate != ineighbour) {
								delta_E += Jarray[cells.celltype[icandidate]][cells.celltype[ineighbour]];
								delta_perimeter++;
							}
						}						
					}
					delta_surf_E=(int)(delta_E); //on stocke la variation d'énergie de surface		
					if(cells.targetarea[pixel] > 0){
						delta_E+=area_constraint/cells.targetarea[pixel]*(-2*(cells.area[pixel]-cells.targetarea[pixel])+1);
					}
					if(cells.targetarea[icandidate] > 0){
						delta_E+=area_constraint/cells.targetarea[icandidate]*(2*(cells.area[icandidate]-cells.targetarea[icandidate])+1);
					}
					sub_flag = false; // flag qui devient true si le changement de valeur du pixel est autorisé par le metropolis step.	
					if ( delta_E < 0. || aleatoire(num) < exp(-delta_E/temperature) ){
						sub_flag = true;
					}

					if(sub_flag){
						//on teste maintenant la connexité locale de la bulle "cible". Si celle-ci ne l'était pas, la modification de la valeur du pixel entrainerait
						//la création d'une arche, et donc celle-ci deviendrait multiplement connexe, ce qu'on veut empêcher.
						//sub_flag=false; // flag qui devient true si la valeur du pixel est effectivement modifiée.
						bool update_flag=false;
						int voisins_identiques = 0;
						if (condwrap) {
							for(int j=0;j<neighbour_connected;j++) {
		                        if (state[PeriodicWrap(y+ny[j], nrow)][PeriodicWrap(x+nx[j], ncol)] == icandidate)
		                            voisins_identiques += 1;
		                    }
		                }	                
		                else {
							for(int j=0;j<neighbour_connected;j++) {
		                        if (state[y+ny[j]][x+nx[j]] == icandidate)
									voisins_identiques += 1;
								}
		                }
	                    if (pixel != mediumcell){
							update_flag=connected_4(state, voisins_identiques, condwrap, icandidate, ncol, nrow, x, y);
						}
						else { //si pixel appartient au médium, on veut qu'il puisse se résorber. Inutile pour un système confluent.
		                    if (connected_4(state, nb_nei_id, condwrap, pixel, ncol, nrow, x, y)){
		                    	update_flag=connected_4(state, voisins_identiques, condwrap, icandidate, ncol, nrow, x, y);	
		                    }
							else { //workaround pour limiter l'apparition de fragments de médium dans les cellules
								update_flag = connected_8(state, voisins_identiques, condwrap, icandidate, ncol, nrow, x, y);
								//workaround alternatif:
								//update_flag = !(test_only_two(state,condwrap, 36, pixel, icandidate, ncol, nrow, x, y));
							}
						}	
						
						if (update_flag==true){
							cells.area[pixel]--;
							cells.area[icandidate]++;
							cells.surf_energy[pixel] += delta_surf_E;
							cells.surf_energy[icandidate] += delta_surf_E;
							cells.perimeter[pixel] += delta_perimeter;
							cells.perimeter[icandidate] += delta_perimeter;
							//cells.area[mediumcell]=0;							
							sum_E += delta_E;
							state[y][x]=icandidate;
						}
					}
				}
			}
		}
	} 
	return sum_E;
}


bool test_only_two(TYPE **state, bool condwrap, int neighbour_copy, int pixel, int icandidate, int ncol, int nrow, int x, int y) {
	//check that there are only two values in the neighborhood
	bool only_two = true;
	if (condwrap) {
		for (int k=0 ; k < neighbour_copy ; k++) {
			if ((state[PeriodicWrap(y+ny[k],nrow)][PeriodicWrap(x+nx[k],ncol)] != pixel) || (state[PeriodicWrap(y+ny[k],nrow)][PeriodicWrap(x+nx[k],ncol)] != icandidate)) {
				only_two = false;
				break;
			}
		}
	}
	else {
		for (int k=0 ; k < neighbour_copy ; k++) {
			if ((state[y+ny[k]][x+nx[k]] != pixel) || (state[y+ny[k]][x+nx[k]] != icandidate)) {
				only_two = false;
				break;
			}
		}
	}
	return only_two;	
}

bool connected_4(TYPE **state, int nb_nei_id, bool condwrap, int pixel, int ncol, int nrow, int x, int y) {
	//return true; //-> enlève la connexité.
	bool nei_flag = true;
	if (nb_nei_id > 1) {
		bool N, E, S, O, NE, NO, SO, SE;
		if (condwrap) {
			E=(state[y][PeriodicWrap(x+1, ncol)] == pixel);
			O=(state[y][PeriodicWrap(x-1, ncol)] == pixel);
			S=(state[PeriodicWrap(y+1, nrow)][x] == pixel);
			N=(state[PeriodicWrap(y-1, nrow)][x] == pixel);
			NE=(state[PeriodicWrap(y-1, nrow)][PeriodicWrap(x+1, ncol)] == pixel);
			NO=(state[PeriodicWrap(y-1, nrow)][PeriodicWrap(x-1, ncol)] == pixel);
			SE=(state[PeriodicWrap(y+1, nrow)][PeriodicWrap(x+1, ncol)] == pixel);
			SO=(state[PeriodicWrap(y+1, nrow)][PeriodicWrap(x-1, ncol)] == pixel);
		} else {
			E=(state[y][x+1] == pixel);
			O=(state[y][x-1] == pixel);
			S=(state[y+1][x] == pixel);
			N=(state[y-1][x] == pixel);
			NE=(state[y-1][x+1] == pixel);
			NO=(state[y-1][x-1] == pixel);
			SE=(state[y+1][x+1] == pixel);
			SO=(state[y+1][x-1] == pixel);
		}
		/* Ici, avoir le même état sous-entend « que le nœuds central considéré ».
		 * Cas à 1 voisins : forcement connexe ;
		 * Cas à 4 voisins : non considéré : pas de changement possible ;
		 * Cas à 2 voisins : soit ils sont opposés, donc ce n'est pas localement connexe,
		 * 					 soit ils sont proches, alors si le nœud diagonal entre eux a le même état, c'est connexe. Sinon, non.
		 * Cas à 3 voisins : on regarde quel voisin n'a pas le même état. Si les deux nœuds diagonaux opposés ont le même état,
		 * 					 c'est connexe. Sinon, non.
		 * On cherche les cas **non** connexes pour leur attribuer nei_flag = false.
		 * Comme il est plus facile de regarder les cas connexes, on s'assure que l'on est **pas** dans un cas connexe.
		 * Retour sur le cas à 3 voisins : l'un (et un seul) des quatres cas ne respecte pas la condition par le voisinage direct (E, O, N ou S).
		 * 		  Alors, s'il la respecte par les diagnonaux, c'est connexe, on ne rentre **pas** dans le « if » (d'où le « ! » avant la condition).
		 * 		  S'il ne la respecte pas, c'est bien **non** connexe, on rentre dans le « if ».
		 * */
		if ( (nb_nei_id == 2 && !( (N&&E&&NE) || (N&&O&&NO) || (S&&E&&SE) || (S&&O&&SO) ) ) || (nb_nei_id == 3 && !( (S||(NE&&NO)) && (E||(NO&&SO)) && (N||(SE&&SO)) && (O||(SE&&NE)) ) )) {
			nei_flag = false;
		}
	}
	return nei_flag;
}

bool connected_8(TYPE **state, int nb_nei_id, bool condwrap, int pixel, int ncol, int nrow, int x, int y) {
	//Check connectivity on Moore neighborhood
	bool nei_flag = true;
	if (nb_nei_id > 1) {
		bool N, E, S, O, NE, NO, SO, SE;
		if (condwrap) {
			E=(state[y][PeriodicWrap(x+1, ncol)] == pixel);
			O=(state[y][PeriodicWrap(x-1, ncol)] == pixel);
			S=(state[PeriodicWrap(y+1, nrow)][x] == pixel);
			N=(state[PeriodicWrap(y-1, nrow)][x] == pixel);
			NE=(state[PeriodicWrap(y-1, nrow)][PeriodicWrap(x+1, ncol)] == pixel);
			NO=(state[PeriodicWrap(y-1, nrow)][PeriodicWrap(x-1, ncol)] == pixel);
			SE=(state[PeriodicWrap(y+1, nrow)][PeriodicWrap(x+1, ncol)] == pixel);
			SO=(state[PeriodicWrap(y+1, nrow)][PeriodicWrap(x-1, ncol)] == pixel);
		} else {
			E=(state[y][x+1] == pixel);
			O=(state[y][x-1] == pixel);
			S=(state[y+1][x] == pixel);
			N=(state[y-1][x] == pixel);
			NE=(state[y-1][x+1] == pixel);
			NO=(state[y-1][x-1] == pixel);
			SE=(state[y+1][x+1] == pixel);
			SO=(state[y+1][x-1] == pixel);
		}
		//if ( (nb_nei_id == 1 && !( (N && !(SO)) || (N && !(SE)) || (S && !(NO)) || (S && !(NE)) || (O && !(NE)) || (O && !(SE)) || (E && !(NO)) || (E && !(SO)) )  ) || (nb_nei_id == 2 && !( (N&&E&&NE && !(SO)) || (N&&O&&NO && !(SE)) || (S&&E&&SE && !(NO)) || (S&&O&&SO && !(NE)) ||  (N&&S && !(SO)) || (N&&S && !(SE)) || (N&&S && !(NO)) || (N&&S && !(NE)) || (O&&E && !(NO)) || (O&&E && !(SO)) || (O&&E && !(NE)) || (O&&E && !(SE)) ) ) || (nb_nei_id == 3 && !( (S||(NE&&NO)) && (E||(NO&&SO)) && (N||(SE&&SO)) && (O||(SE&&NE)) ) )) {
		//if ( (nb_nei_id == 1 && ( (N && (SO)) || (N && (SE)) || (S && (NO)) || (S && (NE)) || (O && (NE)) || (O && (SE)) || (E && (NO)) || (E && (SO)) )  ) || (nb_nei_id == 2 && !( (N&&E&&NE && !(SO)) || (N&&O&&NO && !(SE)) || (S&&E&&SE && !(NO)) || (S&&O&&SO && !(NE)) ||  (N&&S && !(SO) && !(SE) && !(NO) && !(NE)) || (O&&E && !(NO) && !(SO) && !(NE) && !(SE)) ) ) || (nb_nei_id == 3 && !( (S||(NE&&NO)) && (E||(NO&&SO)) && (N||(SE&&SO)) && (O||(SE&&NE)) ) )) {
		if ( (nb_nei_id == 1 && !( (N && !(SO) && !(SE) ) || (S && !(NO) && !(NE) ) || (O && !(NE) && !(SE) ) || (E && !(NO) && !(SO) ) )  ) || (nb_nei_id == 2 && !( (N&&E&&NE && !(SO)) || (N&&O&&NO && !(SE)) || (S&&E&&SE && !(NO)) || (S&&O&&SO && !(NE)) ||  (N&&S && !(SO) && !(SE) && !(NO) && !(NE)) || (O&&E && !(NO) && !(SO) && !(NE) && !(SE)) ) ) || (nb_nei_id == 3 && !( (S||(NE&&NO)) && (E||(NO&&SO)) && (N||(SE&&SO)) && (O||(SE&&NE)) ) )) {
			nei_flag = false;

			
		}
	}
	return nei_flag;
}


void ComputePerimeter(int maxcells, Cell cells, int ncol, int nrow, TYPE **state, int mediumcell, int neighbour_energy){
	//Calcule le périmètre de chaque bulle.
	//Pour avoir le périmètre réel en pixel, il faut diviser par le paramètre line_to_area correspondant au 
	//neighbour_energy choisi (11.3 si neighbour_enery=20)
	int k,ii;

	for(k=0;k<maxcells;k++){
		//cells.oldperimeter[k]=cells.perimeter[k];
		cells.perimeter[k]=0;
	}

	PLANE(	//The PLANE() macro defines a loop over the whole field, i.e., i=1, 2, .., nrow and j=1, 2, .., ncol.
	if(cells.celltype[(k=state[i][j])]>mediumcell){
		for(ii=0;ii<neighbour_energy;ii++){
			if(state[PeriodicWrap(i+ny[ii],nrow)][PeriodicWrap(j+nx[ii],ncol)]!=k)
				//cells.perimeter[k]=cells.perimeter[k];
				cells.perimeter[k]++;
		}
	}
	);
}

double ComputeEnergy(int maxcells, Cell cells, int ncol, int nrow, TYPE **state, int neighbour_energy, int** Jarray, double area_constraint){
	//Calcule l'énergie du système.
//	
	double boundary_energy = 0;
	int k, cible, ii;
	double total_energy=0;
	PLANE( //The PLANE() macro defines a loop over the whole field, i.e., i=1, 2, .., nrow and j=1, 2, .., ncol.
		k=state[i][j];
		for(ii=0;ii<neighbour_energy;ii++){
			//Si on trouve un voisins différents avec un type différent, c'est une frontière.
			cible = state[PeriodicWrap(i+ny[ii],nrow)][PeriodicWrap(j+nx[ii],ncol)];
			if(cible!=k){
				boundary_energy += Jarray[cells.celltype[cible]][cells.celltype[k]];
				//fprintf(stderr,"%d\t",Jarray[cells.celltype[cible]][cells.celltype[k]]%1600);
			}
		}
	);
	if((int)(boundary_energy)%2 != 0){
		fprintf(stderr,"Error, Check of Parity of twice Boundary Energy failed = %d\n", boundary_energy);
		exit(EXIT_FAILURE);
	}
	
	boundary_energy=boundary_energy/2; //on divise par deux car chaque couplage est compté deux fois
	total_energy=boundary_energy;
	//fprintf(stderr,"%lf\t%lf\n",boundary_energy,total_energy);
	
	for(k=1;k<maxcells;k++) {//on ajoute l'énergie de compression
		if(cells.targetarea[k]!=0) {
			total_energy+=area_constraint*pow(cells.targetarea[k]-cells.area[k],2)/cells.targetarea[k];
		}
	}
	
	return total_energy;

}

void ComputeCenterCoords(Cell cells, int ncol, int nrow, TYPE **state, int nbrcells, int mediumcell) {
//Calcule les centres géométriques des cellules. Contrairement à la méthode itérative à chaque MCS, cette fonction ne
//peut donner que les coordonnées modulo ncol et nrow, et n'est donc pas adaptée à l'étude de la diffusion.

//Il vaudrait mieux définir Area*xcoord: car ce sont toujours des entiers, contrairement aux coords qui sont des doubles.	
	int airehautgauche[nbrcells], airehautdroite[nbrcells], airebasgauche[nbrcells], airebasdroite[nbrcells];
	int xgauche[nbrcells], xdroite[nbrcells], ybas[nbrcells], yhaut[nbrcells];
	memset(airehautgauche, 0, sizeof(airehautgauche));
	memset(airehautdroite, 0, sizeof(airehautdroite));
	memset(airebasgauche, 0, sizeof(airebasgauche));
	memset(airebasdroite, 0, sizeof(airebasdroite));
	memset(xgauche, 0, sizeof(xgauche));
	memset(xdroite, 0, sizeof(xdroite));
	memset(ybas, 0, sizeof(ybas));
	memset(yhaut, 0, sizeof(yhaut));
	int i, j, k, airegauche, airedroite, airehaut, airebas;
	for(i=1;i<=nrow/2;i++){
		for(j=1;j<=ncol/2;j++){
			k=state[i][j];
			xgauche[k] += j;
			yhaut[k] += i;
			airehautgauche[k]++;			
		}
		for(j=ncol/2+1;j<=ncol;j++){
			k=state[i][j];
			xdroite[k] += j;
			yhaut[k] += i;
			airehautdroite[k]++;			
		}		
	}
	for(i=nrow/2+1;i<=nrow;i++){
		for(j=1;j<=ncol/2;j++){
			k=state[i][j];
			xgauche[k] += j;
			ybas[k] += i;
			airebasgauche[k]++;			
		}
		for(j=ncol/2+1;j<=ncol;j++){
			k=state[i][j];
			xdroite[k] += j;
			ybas[k] += i;
			airebasdroite[k]++;			
		}		
	}
	for(k=mediumcell+1;k<nbrcells;k++){ //A cause des conditions aux limites périodiques, il faut regrouper les domaines cellulaires partagés.
		airehaut=airehautgauche[k]+airehautdroite[k];
		airebas=airebasgauche[k]+airebasdroite[k];
		airegauche=airehautgauche[k]+airebasgauche[k];
		airedroite=airehautdroite[k]+airebasdroite[k];
		if(airehaut + airebas != cells.area[k] || cells.area[k]==0){
			fprintf(stderr,"error: il y a une fuite d'aire de la cellule n°%d !\n",k);
			//exit(EXIT_FAILURE);
		}
		if (2*xdroite[k]*airegauche - 2*xgauche[k]*airedroite > airegauche*airedroite*ncol){
		//if (xdroite[k]/airedroite - xgauche[k]/airegauche > ncol/2){ //critère pour déterminer si la cellule est à cheval sur les bords.
			if(airegauche > airedroite)//on déplace la plus petite des deux moitiés
				xdroite[k]=xdroite[k]-ncol*airedroite;
			else
				xgauche[k]=xgauche[k]+ncol*airegauche;
		}
		if (2*ybas[k]*airehaut - 2*yhaut[k]*airebas > airehaut*airebas*nrow){
		//if (ybas[k]/airebas - yhaut[k]/airehaut > nrow/2){
			if(airehaut > airebas)
				ybas[k]=ybas[k]-nrow*airebas;
			else
				yhaut[k]=yhaut[k]+nrow*airehaut;
		}
		cells.xcoord[k]=(double)(xgauche[k]+xdroite[k])/cells.area[k]; //il faudra reitrer la division par cells.area pour ne travailler qu'avec des entiers
		cells.ycoord[k]=(double)(ybas[k]+yhaut[k])/cells.area[k];
	}
}

int ComputeBoundary(Cell cells, int ncol, int nrow, TYPE **state, int neighbour_energy) {
	//Calcule la longueur de l'interface entre les deux types cellulaires. Le renvoie.
	//Pour avoir la longueur réelle en pixel, il faut diviser par le paramètre line_to_area correspondant au 
	//neighbour_energy choisi (11.3 si neighbour_energy=20).
	int interface = 0, k, cible, ii;
	PLANE(
		k=state[i][j];
		for(ii=0;ii<neighbour_energy;ii++){
			//Si on trouve un voisins différents avec un type différent, c'est une frontière.
			cible = state[PeriodicWrap(i+ny[ii],nrow)][PeriodicWrap(j+nx[ii],ncol)];
			if(cible!=k){
				if(cells.celltype[cible] != cells.celltype[k] ){
					interface ++;
					//break;
				}
			}
		}
	);
	return interface;
}

void FindNeighbours(int maxcells, Cell cells, int ncol, int nrow, TYPE **state, int mediumcell, int neighbour_connected, int maxneighbours, int* side_interf12, int* side_interf10, int* side_interf20) {
	
	//En toute rigueur neighbour_connected devrait correpondre au voisinage d'adjacence, z=4. Mais du fait de la discrétisation, deux noeuds trivalents sont parfois fusionnés
	// en un seul neud quadrivalent. On a alors un problème: certains cotés on une longueur nulle et ne sont plus comptabilisés. On trouve alors qu'il y a plus de cellules
	// à 5 côtés qu'à 7 côtés ! Pour éviter ce pb, on étend la def de voisins aux 8 pixels (z=8) les plus proches.
	int k,ii,iii,nouv,neigh;
	for(k=0;k<maxcells;k++) {
		cells.nneighbours[k] = 0;
		cells.temp[k] = 0;
	}
	//*side_interf = 0;

	PLANE(	//The PLANE() macro defines a loop over the whole field, i.e., i=1, 2, .., nrow and j=1, 2, .., ncol.
	if(cells.celltype[(k=state[i][j])]>mediumcell){
		for(ii=0;ii<neighbour_connected;ii++){
			if((neigh=state[PeriodicWrap(i+ny[ii],nrow)][PeriodicWrap(j+nx[ii],ncol)])!=k){
				nouv=1;
				for(iii=0;iii<cells.nneighbours[k];iii++){//on vérifie que ce voisin n'est pas déjà compté
					if(neigh==cells.neighbours[k][iii]){
						nouv=0;
						break;
					}
				}
				if(nouv==1){
					if (neigh>mediumcell)
						cells.temp[k] += 1./(3./3.72*(sqrt(cells.area[k])-sqrt(cells.area[neigh]))*(sqrt(cells.area[k])-sqrt(cells.area[neigh]))/(sqrt(cells.area[k])*sqrt(cells.area[neigh])*(sqrt(cells.area[k])+sqrt(cells.area[neigh]))));
					
					cells.nneighbours[k]++;
					if(cells.nneighbours[k]>=maxneighbours){
						fprintf(stderr,"Error, FindNeighbours: increase MAXNEIGHBOURS\n");
						exit(EXIT_FAILURE);
					}
					cells.neighbours[k][iii]=neigh;
					//Calcul de la longueur d'interface entre différents types cellulaires mesurée en nombres de côtés. Il faut diviser par 2 le résultat (qui doit donc être pair) pour avoir le nbr de  bord de Plateua.
					/*//On ne tient pas compte de l'interface avec le mediumcell.
					if((cells.celltype[k] != cells.celltype[neigh]) && (cells.celltype[neigh]>mediumcell)) {
						*side_interf12=*side_interf12 + 1;
					}*/
					if(cells.celltype[k] ==1) {
						if(cells.celltype[neigh]==2) {
							*side_interf12=*side_interf12 + 1;
						}
						else if (cells.celltype[neigh]==mediumcell) {
							*side_interf10=*side_interf10 + 1;
						}
					}
					else if ((cells.celltype[k] ==2) && (cells.celltype[neigh]==mediumcell)) {
						*side_interf20=*side_interf20 + 1;
					}
										
				}
			}
		}
	}
	);
}


void AffichageCouleurs(int affichage, Cell cells, int ncol, int nrow, char *subdirname, char *subdirnameRAW ,TYPE **state, TYPE **nstate) {
	//couleurs 
	//affichage=0 for white monochrome, 1 for color with number of neighbours, 2 for color with celltypes, 3 for five different colors (large with 6, 7 neighbours, small with 5, 6 neighbours + default).
		 // Default : try (but easiliy fail!) to make a different color for each bubble.
	int n = 0;
	if (affichage == 1) {
		//Use of Cash plane, need i and j are no define variable.
		PLANE(
			n = cells.nneighbours[state[i][j]];
			if(n<3 || n>9)
				nstate[i][j]=0;
			else
				nstate[i][j]=n-2;
		);
	}
	else if (affichage == 2) {
		PLANE(
			if(state[i][j]!=0){
				if(cells.celltype[state[i][j]]==1)
					nstate[i][j]=4;
				else
					nstate[i][j]=8;
			}
			else
				nstate[i][j]=1;
		);
	}
	else if (affichage == 3) {
		PLANE(
			n = cells.nneighbours[state[i][j]];
			if(state[i][j]!=0){
				if (cells.celltype[state[i][j]]==1 && n == 6)
					nstate[i][j]=4;
				else if (cells.targetarea[state[i][j]]==2 && n == 6)
					nstate[i][j]=7;
				else if (cells.targetarea[state[i][j]]==1 && (n == 5 || n == 7))
					nstate[i][j]=9;
				else if (cells.targetarea[state[i][j]]==2 && (n == 5 || n == 7))
					nstate[i][j]=5;
				else
					nstate[i][j]=2;
			}
			else
				nstate[i][j]=1;
		);
	}
	else if (affichage == 0) { //monochrome
				PLANE(
					nstate[i][j]=WHITE;
				);
			}
	else {
		PLANE(
			nstate[i][j]=state[i][j]+1;
		);
	}
	//plan affichage, plan cellules, yoffset, xoffset, coloroffset
	CellPlaneDisplay(nstate,state,0,0,0);
	//CellPlaneRAW2(subdirnameRAW,state,state,0);
	PlaneRAW2(subdirnameRAW,state,state,0);
	CellPlanePNG2(subdirname,nstate,state,0); //nstate définit dans quelle couleur une bulle apparaitra, tandis que state définit la position des bulles
}
