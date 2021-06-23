#include "allocate.h"

Cell AllocateCells(int n, int maxneighbour) {
	int i;
	Cell cells;

	cells = *(Cell *)malloc(sizeof(Cell));
	if((cells.celltype=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.xcoord=(double *)calloc((size_t)n,sizeof(double)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.ycoord=(double *)calloc((size_t)n,sizeof(double)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.temp=(double *)calloc((size_t)n,sizeof(double)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.targetarea=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.area=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.perimeter=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}/*
	if((cells.oldperimeter=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}*/
	if((cells.surf_energy=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.nneighbours=(int *)calloc((size_t)n,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	if((cells.neighbours=(int **)calloc((size_t)n,sizeof(int *)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
	}
	for(i=0;i<n;i++){
		if((cells.neighbours[i]=(int *)calloc((size_t)maxneighbour,sizeof(int)))==NULL) {
		fprintf(stderr,"error in memory allocation\n");
		exit(EXIT_FAILURE);
		}
	}
	return cells;
}

void FreeCells(Cell cells, int n)
{
	int i;

	for(i=0;i<n;i++){
		free(cells.neighbours[i]);
	}
	free(cells.neighbours);
	free(cells.nneighbours);
	free(cells.celltype);
	free(cells.targetarea);
	free(cells.area);
	free(cells.perimeter);
	free(cells.surf_energy);
	//free(cells.oldperimeter);
	free(cells.xcoord);
	free(cells.ycoord);
	free(cells.temp);
}

void Duplicate(Cell copie, Cell original, int maxcells)
{
	int k,i;
	for(k=0;k<maxcells;k++){
		copie.celltype[k]=original.celltype[k];
		copie.targetarea[k]=original.targetarea[k];
		copie.area[k]=original.area[k];
		copie.perimeter[k]=original.perimeter[k];
		copie.surf_energy[k]=original.surf_energy[k];
		//copie.oldperimeter[k]=original.oldperimeter[k];
		copie.xcoord[k]=original.xcoord[k];
		copie.ycoord[k]=original.ycoord[k];
		copie.temp[k]=original.temp[k];
		copie.nneighbours[k]=original.nneighbours[k];
		for(i=0;i<original.nneighbours[k];i++)
			copie.neighbours[k][i]=original.neighbours[k][i];
	}
}

int PutCell(TYPE **plane, int y, int x, TYPE m, int ncol, int nrow, int side1, int side2) {
	const int xval[9] = { 1, 1,-1,-1,-1, 1, 0, 0, 0};
	const int yval[9] = { 0,-1,-1, 0, 1, 1,-1, 1, 0};
	int l,p,present;
	present=0;
	if(side1==3 && side2==3){ //graines de 3x3 pixels
		for(l=0;l<9;l++)
			present += plane[PeriodicWrap(y+yval[l],nrow)][PeriodicWrap(x+xval[l],ncol)];
		if(!present){
			for(l=0;l<9;l++)
				plane[PeriodicWrap(y+yval[l],nrow)][PeriodicWrap(x+xval[l],ncol)] = m;
			return 1;
		}
		else
			return 0;
	}
	else { //graines à touche-touche pavant le plan
		for(l=0;l<side2;l++){
			for(p=0;p<side1;p++){
				present += plane[PeriodicWrap(y+l-side2/2,nrow)][PeriodicWrap(x+p-side1/2,ncol)];
			}
		}
		if(!present){
			for(l=0;l<side2;l++){
				for(p=0;p<side1;p++){
					plane[PeriodicWrap(y+l-side2/2,nrow)][PeriodicWrap(x+p-side1/2,ncol)] = m;
				}
			}
			return 1;
		}
		else
			return 0;
	}
	
}

void InitBubblePlane(int init_config, float fillfactor,int nrow,int ncol, int target_area, double a1, double a2, TYPE **state, Cell cells, int sliding) {
	//rq: étrange, si fillfactor est déclaré en double, plutôt que float, alors target_area/fillfactor donne des résulats étrange: par exemple, si target_area=110 et fillfactor=1.1, le rapport des deux obtenus est 99 !!
	int i,j,k;
	int cellarea=9; //taille par défaut de la graine de chaque bulle
	int arete;
	
	
	int number=(int)(fillfactor*nrow*ncol/target_area);
		
	k=1;
	while(k<1+number){

		switch (init_config)
		{
		//for a unique bubble:
		case 0:
			//printf("number= %d\n", number);
			number=1;
			a1=3, a2=a1; //dimensions par défaut de la graine d'une cellule
			i=(int)(nrow/2);
			j=(int)(ncol/2);
			break;
		
		//for a random, biperiodic foam:
		case 1:
			a1=3, a2=a1; //dimensions par défaut de la graine d'une cellule
			i=(int)(aleatoire(0)*nrow)+1;
			j=(int)(aleatoire(0)*ncol)+1;
			break;
		
		//for a random, squared free cluster:
		case 2:
			a1=3, a2=a1; //dimensions par défaut de la graine d'une cellule	
			i=(int)(aleatoire(0)*nrow*2/3)+(int)(nrow/6)+1;
			j=(int)(aleatoire(0)*ncol*2/3)+(int)(ncol/6)+1;		
			//if(i>0 && i<nrow && j>0 && j<ncol){
			break;

		// for a random rounded free cluster
		case 3:
			a1=3, a2=a1; //dimensions par défaut de la graine d'une cellule
			do {
				i=(int)(aleatoire(0)*nrow);
				j=(int)(aleatoire(0)*ncol);	
			} while ( pow(i-nrow/2,2)+pow(j-ncol/2,2) > 0.8*number*target_area/3.14);
			break;

/*
// for a squared, ordered periodic cluster:	
		int arete=(int)(sqrt(fillfactor*nrow*ncol/target_area)); //racine carrée de Maxcell: nombre de cellules par ligne (ou par colonne)
		int a1=(int)(ncol/sqrt(fillfactor*nrow*ncol/target_area)); //dimension d'une cellule (réseau carré)
		int a2=a1;
		j=a1*((k-1) % arete)+1;
		i=a2*(int)((double)(k-1) / arete) +(int)(a2/2*(k%2))+1;//rq: il faut que arete soit paire pour que le pavage soit bien périodique !
		cellarea=a1*a2;
		int area= target_area/fillfactor; //en fait cellarea et area devraient toujours être identiques. On peut en supprimer un.
		if( (area != (int)pow((int)sqrt(area),2)) || ((ncol % (int)sqrt(area)) != 0) || ((nrow % (int)sqrt(area)) != 0) ){
			// area doit former un carré parfait, et ncol et nrow doivent être des multiples de sqrt(area)
			fprintf(stderr,"error: values of ncol, nrow, target_area and fillfactor are not compatible for squared, periodic, regular lattice\n");
			printf("(area=%d,(int)pow((int)sqrt(area),2)=%d,ncol=%d,nrow=%d)\n",(int)area,(int)pow((int)sqrt(area),2),ncol,nrow);
			exit(EXIT_FAILURE);
		}

		printf("(a=%d,a*arete=%d,k=%d;%d,%d)\n",a1,a1*arete,k,j,i);
		if(i>0 && i<nrow && j>0 && j<ncol){
*/

		// for a rectangular, ordered periodic cluster with aspect ratio 2/sqrt(3) (for periodic hexagonal tilings):
		case 4:
			//arete=(int)(sqrt(fillfactor*nrow*ncol/target_area)); //racine carrée de Maxcell: nombre de cellules par ligne (ou par colonne)
			arete=(int)(ncol/a1);		//nombre de cellules par ligne (et par colonne)
			number=(int)((nrow*ncol)/(a1*a2));	//petite correction pour éviter les erreurs dues aux arrondis successifs.
			//number=(ncol*ncol)/(a1*a1);
//			if( (ncol*a2 != nrow*a1) || (((ncol/a1) % 2) != 0) || (((nrow/a2) % 2) != 0) ){
			/*
			if( (((ncol/a1) % 2) != 0) || (((nrow/a2) % 2) != 0) ){
				fprintf(stderr,"Attention: values of ncol, rx, nrow, ry in the configuration file are not compatible with rectangular, periodic, regular lattice\n");
				exit(EXIT_FAILURE);
			}
			*/
			j=(int)(a1*((k-1) % arete)+1);
			i=(int)(a2*((k-1) / arete) +(a2/2*(k%2))+1);//rq: idéalement il faudrait que arete soit paire pour que l'agencement soit parfaitement périodique !
			cellarea=a1*a2;
			//if(i>0 && i<nrow && j>0 && j<ncol){
			break;
				
// for a rectangular, ordered periodic cluster with aspect ratio 2/sqrt(3) under shear strain:
		case 5:
			//arete=(int)(sqrt(fillfactor*nrow*ncol/target_area)); //racine carrée de Maxcell: nombre de cellules par ligne (ou par colonne)
			arete=(int)(ncol/a1);		//nombre de cellules par ligne (et par colonne)
			number=(int)((nrow*ncol)/(a1*a2));	//petite correction pour éviter les erreurs dues aux arrondis successifs.
			//number=(ncol*ncol)/(a1*a1);
//			if( (ncol*a2 != nrow*a1) || (((ncol/a1) % 2) != 0) || (((nrow/a2) % 2) != 0) ){
			/*
			if( (((ncol/a1) % 2) != 0) || (((nrow/a2) % 2) != 0) ){
				fprintf(stderr,"Attention: values of ncol, rx, nrow, ry in the configuration file are not compatible with rectangular, periodic, regular lattice\n");
				exit(EXIT_FAILURE);
			}
			*/
			//sliding est la variable qui donne le décalage entre la première et la dernière colonne
			j=(int)(a1*((k-1) % arete)+1);
			double offset= sliding*((double)a2/(double)arete)*((k-1) % arete);
			i=(int)(a2*((k-1) / arete) +(a2/2*(k%2))+1+offset);//rq: idéalement il faudrait que arete soit paire pour que l'agencement soit parfaitement périodique !
			cellarea=a1*a2;
			//if(i>0 && i<nrow && j>0 && j<ncol){
			break;
// for a unique bubble with initial area comparable to target area (workaround to avoid collapse when starting from the 3x3 seed with low bulk modulus):
		case 6:
			a1=(int)sqrt(target_area); a2=a1;
			number=1;
			i=(int)(nrow/2);
			j=(int)(ncol/2);
			cellarea=(int)(a1*a2);
			break;
		}			
		

		if(PutCell(state,i,j,k, ncol, nrow, (int)a1, (int)a2) ){
			cells.xcoord[k]=j;
			cells.ycoord[k]=i;
			cells.area[k]=cellarea;
			cells.celltype[k]=1;
			cells.targetarea[k]=target_area;
			k++;
		}
		
	}
}

int AssignNormalTargetarea(int mean, double mu2adim, int minimum) {
       int area=0;
       while(area<minimum)
               area=(int)normal((double)mean,sqrt(mu2adim*mean*mean));
       return area;
}

int GeneratePolydispersity(int polydispersity, int blob, int maxcells, double fillfactor, int nrow, int ncol, int target_area, double targetareamu2, int target_area2, double alpha, Cell cells) { 
	int ksigma, meantargetarea, totaltargetarea=0, delet=0;
	double randnum;
	meantargetarea=target_area;
	switch (polydispersity) {
	//monodisperse:
	case 1:
		for(ksigma=1;ksigma<maxcells;ksigma++){
			if(cells.area[ksigma]>0){
				cells.targetarea[ksigma]=meantargetarea;
				totaltargetarea+=cells.targetarea[ksigma];
				if(totaltargetarea-meantargetarea>fillfactor*nrow*ncol) {
				 	cells.targetarea[ksigma]=0;
					delet++;
				}
			}
		}
		break;
	//bidisperse:
	case 2:
		//target_area=(int)(meantargetarea*(1.+targetareamu2));
		//target_area2=(int)(meantargetarea*(1.-targetareamu2));
		target_area=(int)(meantargetarea*(1.+targetareamu2*sqrt(alpha/(1-alpha)))); //grosses bulles
		target_area2=(int)(meantargetarea*(1.-targetareamu2*sqrt((1-alpha)/alpha))); //petites bulles
		for(ksigma=1;ksigma<maxcells;ksigma++){
			if(cells.area[ksigma]>0){
				switch (blob) {
				//frontière verticale entre les 2 types de bulles:
				case 1:
					if(D_PeriodicWrap(cells.xcoord[ksigma],ncol)<(1-alpha)*ncol-sqrt(meantargetarea/3.14))
						cells.targetarea[ksigma]=target_area;
					else {
						cells.targetarea[ksigma]=target_area2;
						cells.celltype[ksigma]=2;
					}
					//cells.targetarea[ksigma]=(D_PeriodicWrap(cells.xcoord[ksigma],ncol)<(1-alpha)*ncol-sqrt(meantargetarea/3.14))?target_area:target_area2;
					break;
				//blob central de forme carrée:
				case 2:
					if(D_PeriodicWrap(cells.xcoord[ksigma],ncol)>ncol/2*(1-sqrt(1-alpha))+sqrt(meantargetarea/3.14) && D_PeriodicWrap(cells.xcoord[ksigma],ncol)<ncol/2*(1+sqrt(1-alpha))-sqrt(meantargetarea/3.14) && D_PeriodicWrap(cells.ycoord[ksigma],nrow)>nrow/2*(1-sqrt(1-alpha))+sqrt(meantargetarea/3.14) && D_PeriodicWrap(cells.ycoord[ksigma],nrow)<nrow/2*(1+sqrt(1-alpha))-sqrt(meantargetarea/3.14))
						cells.targetarea[ksigma]=target_area;
					else {
						cells.targetarea[ksigma]=target_area2;
						cells.celltype[ksigma]=2;
					}
					break;
				//blob central de forme circulaire:
				case 3:
					if(sqrt(pow(D_PeriodicWrap(cells.xcoord[ksigma],ncol)-ncol/2,2)+pow(D_PeriodicWrap(cells.ycoord[ksigma],nrow)-nrow/2,2))<sqrt((1-alpha)*ncol*nrow/3.14)-sqrt(meantargetarea/3.14))
						cells.targetarea[ksigma]=target_area;
					else {
						cells.targetarea[ksigma]=target_area2;
						cells.celltype[ksigma]=2;
					}
					break;
				//répartition aléatoire des petites et grosses bulles:
				default:
					if(aleatoire(0)<(1-alpha))
						cells.targetarea[ksigma]=target_area;
					else {
						cells.targetarea[ksigma]=target_area2;
						cells.celltype[ksigma]=2;
					}
				}
				totaltargetarea+=cells.targetarea[ksigma];
				if(totaltargetarea-meantargetarea>fillfactor*nrow*ncol){
					cells.targetarea[ksigma]=0;
					delet++;
				}
			}
		}
		break;
	//tridisperse:
	case 3:
		target_area=(int)(meantargetarea*(1.+targetareamu2));
		target_area2=(int)(meantargetarea*(1.-targetareamu2));
		for(ksigma=1;ksigma<maxcells;ksigma++){
			if(cells.area[ksigma]>0){
				randnum=aleatoire(0);
				if(randnum<1./3.)
					cells.targetarea[ksigma] = target_area;
				else if(randnum<2./3.)
					cells.targetarea[ksigma] = meantargetarea;
				else
					cells.targetarea[ksigma] = target_area2;
				totaltargetarea+=cells.targetarea[ksigma];
				if(totaltargetarea-meantargetarea>fillfactor*nrow*ncol){
					cells.targetarea[ksigma]=0;
					delet++;
				}
			}
		}
		break;
	//polydisperse:
	default:
		for(ksigma=1;ksigma<maxcells;ksigma++){
			if(cells.area[ksigma]>0){
				cells.targetarea[ksigma]=AssignNormalTargetarea(meantargetarea,targetareamu2,30);
				totaltargetarea+=cells.targetarea[ksigma];
				if(totaltargetarea-meantargetarea>fillfactor*nrow*ncol){
					cells.targetarea[ksigma]=0;
					delet++;
				}
			}
		}
	}
	return delet;
}
