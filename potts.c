#include "potts.h"



/*
Written by Marc Durand (marc.durand@u-paris.fr)

compile: make potts; run: potts <entree> <sortie>
a sequence of pictures is generated in the directory <sortie>


For debugging, add "-ggdb" to the cc command in the cash makefile
Try electric fence as well: "-lefence"
*/



#define MAXNEIGHBOURS 20
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	clock_t start, end;
	double cpu_time_used;
	double nmoyen_corrected;
	int k, totaltime=10000, deleted, target_area2, ksigma, nbrsides, nbrbulles, nbrlarge, nbrsmall, affichage, load, side_interface12, side_interface10, side_interface20;
	//double proba_swap_temp;
	double pression, pressionvoisine, curvature, mfacurvature, largecurvature, smallcurvature, largetemperature, smalltemperature, largemuprime, smallmuprime, largenumberofsides, smallnumberofsides, actual_temperature;
	char Jval[200], output[200], loadname[200];
	char neighbourfile[200], coordinatefile[200], subsystemsfile[200], voisinsfile[200], swapfile[200], acceptancefile[200], savefile[250], loadfile[250], moviefile[200];//,systcmd[200], parfile[200]; No used. See output part.
	int movie = 1,movieinterval=50, writinginterval=50, saveinterval=50, annealing=0, annealing_time=20, ttime=1, time_load=1, dispersetime=100, temperinginterval = 100;
	int heat_bath = 0, nb_temperature;
	//int *targetarea, *cellarea, *perimeter, *celltype, *oldperimeter, *targetperimeter; long largenumberofsides,smallnumberofsides;
	int **Jarray; //a 2D array for the interaction energies between cells
	double area_constraint = 3000.;
	double temperature_min=10., temperature_max=50.;
	double tmp_energie=0.;
	int mediumcell = 0; //the highest cellnumber without area conservation
	extern int scale, nrow, ncol; //the parameters of cash we would like to change
	extern int cellinterface;
	int maxtypes=3,maxcells=100,target_area=100,meantargetarea=100;
	double rx=0,ry=0; //dimensions d'une cellule (pour pavage régulier et périodique)
	int neighbour_energy = 20, neighbour_copy = 4, neighbour_connected = 4; //number of neighbours for a pixel as a function of what we want.
	double line_to_area=11.3, fillfactor; //fillfactor représente le rapport entre l'aire qu'occuperait le gaz des bulles si non encapsulé et l'aire de la mousse (ncol*nrow)
	int polydispersity=12;
	double targetareamu2=0.2; // Différence de taille entre les bulles (ex. : bidisperse + targetareamu2=0.2 => bulles de taille target_area*1.2 et target_area*0.8
	double alpha=0.5; //proportion de petites bulles. Ne marche que pour mousses bidisperses (polydispersity=2) distribuées aléatoirement (blob != 1,2,3)
	int init_config; //si =0, une bulle unique; si =1, mousse désordonnée bi-périodique; si=2 cluster libre carrée; si =3 cluster libre rond; si=4 pavage régulier et périodique. 
	int blob=2; //si blob=1, frontière verticale entre petites et grosses bulles, si blob=2, blob central carré, si blob=3, blob central circulaire, et sinon les petites et grosses bulles sont distribuées aléatoirement
	//bool wait = false;
	int sliding=0;
	long unsigned int seed=123456789;
	
	
	if (argc!=3) {
		fprintf(stderr,"usage: %s parameterfile output\n",argv[0]);
		return 1;
	}

	//reading the parameters from a file
	ReadOptions(argv[1]);
	InDat("%lu","seed",&seed);
	InDat("%d","dispersetime",&dispersetime);
	InDat("%d","movie",&movie);
	InDat("%d","affichage",&affichage);
	InDat("%d","movieinterval",&movieinterval);
	InDat("%d","writinginterval",&writinginterval);
	InDat("%d","saveinterval",&saveinterval);
	InDat("%d","load",&load);
	InDat("%s","loadname",&loadname);
	InDat("%d","maxtypes",&maxtypes);
	InDat("%d","scale",&scale);
	InDat("%d","init_config",&init_config);
	InDat("%lf","rx",&rx);
	InDat("%lf","ry",&ry);
	InDat("%d","target_area",&target_area);
	InDat("%d","sliding",&sliding);
	InDat("%lf","fillfactor",&fillfactor);
	if ( (init_config==4 || init_config==5) && (rx !=0) && (ry!=0) ){
		target_area=(int)(rx*ry*fillfactor); //si on ne lit pas de valeurs pour rx et ry dans le fichier, on ne modifie pas la valeur de target_area
		printf("target area = %d\n",target_area);
	}
	InDat("%lf","targetareamu2",&targetareamu2);
	InDat("%lf","alpha",&alpha);
	InDat("%d","neighbour_energy",&neighbour_energy);
	//InDat("%d","neighbour_copy",&neighbour_copy);
	//InDat("%d","neighbour_connected",&neighbour_connected);
	InDat("%lf","area_constraint",&area_constraint);
	InDat("%d", "temperinginterval",&temperinginterval);
	InDat("%d","nb_temperature",&nb_temperature);
	InDat("%d","annealing",&annealing);
	if (annealing) {
		//actual_temperature = temperature[0];
		//temperature[0] = annealing_temp;
		nb_temperature = 1;
		InDat("%d","annealing_time",&annealing_time);
	}
	else {
		annealing_time = 0;
	}
//	InDat("%d","annealing_temp",&annealing_temp);
	
	
	TYPE **state[nb_temperature], **nstate, **copystate, **copynstate; //a 2D array. TYPE = int. For speeding up calculations, TYPE can be set in cash.h
	Cell cells[nb_temperature], copycells;
	double temperature[nb_temperature], energie[nb_temperature];
	int trial[nb_temperature-1], acceptance[nb_temperature-1];
	char subdirectory[nb_temperature][200], subdirectoryRAW[nb_temperature][200];
	InDat("%lf","temperature_min",&temperature_min);
	InDat("%lf","temperature_max",&temperature_max);
	InDat("%d","polydispersity",&polydispersity);
	InDat("%d","blob",&blob);
	InDat("%lf","line_to_area",&line_to_area);
	
	InDat("%d","totaltime",&totaltime);
	InDat("%d","heat_bath",&heat_bath);
		
	if (movieinterval==0 || movieinterval%writinginterval) {
		printf("Attention: movieinterval doit être un multiple de writinginterval. On prend movieinterval = writinginterval = %d\n",writinginterval);
		movieinterval=writinginterval;
	}
	
	if ( annealing_time > movieinterval) {
		printf("Attention: annealing_time est plus grand que l'intervalle entre deux images (movieinterval). Est-ce volontaire ?\n");
	}
	
//	if (dispersetime < writinginterval <= (dispersetime+annealing_time) ) {
	if ((dispersetime < writinginterval) && (writinginterval <= (dispersetime+annealing_time)) ) {
		printf("Attention: on doit avoir writinginterval > (dispersetime + annealing_time) ou writinginterval <= dispersetime.\n");
		//dispersetime = writinginterval+100;
		//printf("Modification de dispersetime: dispersetime= %d\n",dispersetime);
	}
		
	

	//Temperatures multiples.
	temperature[0]=temperature_min;
	if (nb_temperature > 1) {
		temperature[nb_temperature-1]=temperature_max;
		for (int i=1;i<nb_temperature-1;i++) {
			//intervalles constants entre les températures inverses:
			temperature[i] = 1./(1./temperature[0] + i * (1./temperature[nb_temperature-1]-1./temperature[0]) / (double)(nb_temperature-1));
			//températures (ou leurs inverses) distribuées selon une suite géométrique:
			//temperature[i] = temperature[i-1]*pow((double)(temperature[nb_temperature-1]/temperature[0]),1./(double)(nb_temperature-1));
			printf("%lf \n",temperature[i]);
		}
	}
	if (temperature[0] < 1e-10 || temperature[nb_temperature-1] < 1e-10) {
		printf("Temperature trop faible ! %lg. Arrêt.\n", temperature[0]);
		return EXIT_FAILURE;
	}
	if (!annealing && nb_temperature>1 && (temperature_max - temperature_min < 1e-10) ){
		printf ("Attention: températures min et max sont égales: les données de systèmes différents vont être mélangées. Arrêt.\n");
		return EXIT_FAILURE;
	}
	if(!annealing && nb_temperature==1){
		temperature[0]=temperature_max;//on change momentanément la valeur de temperature[0]: c'est juste pour que les fichiers de sauvegarde
		//aient la valeur de temperature_max. Mais on restera à temperature_min jusqu'à dispersetime dans les simuls.
	}
/* Cette partie ne sert que si l'on veut tirer au hasard les temps pour les swaps
	if (nb_temperature==1) {
		proba_swap_temp = 0.;
	} else if (nb_temperature==2) {
		proba_swap_temp = 1./temperinginterval;
	} else {
		proba_swap_temp = (nb_temperature-1.)/(2.*temperinginterval);
	}
*/

	snprintf(moviefile,200,"%s",argv[2]);
	int res = mkdir(moviefile, 0777);
		
	//set up output
	FILE *swapfp, *moviefp, *savefp, *loadfp, *acceptancefp;
	snprintf(output,200,"%s",argv[2]); //argv[2] est le 2eme argument passé au programme. Il s'agit donc du nom du dossier de la simul
	snprintf(savefile,200,"%s/%s.save",argv[2],argv[2]);
	snprintf(loadfile,200,"%s/%s.save",argv[2],loadname);
	snprintf(swapfile,200,"%s/%s.swap",argv[2],argv[2]);
	snprintf(acceptancefile,200,"%s/%s.trials",argv[2],argv[2]);
	swapfp = fopen(swapfile,"w");
	fclose(swapfp);
	



	FILE *neighbourfp[nb_temperature], *coordfp[nb_temperature], *subsystemsfp[nb_temperature], *voisinsfp[nb_temperature];
	for (int i=0;i<nb_temperature;i++) {
		snprintf(neighbourfile,200,"%s/%s.neighbours.%g",argv[2],argv[2], temperature[i]);
		neighbourfp[i] = fopen(neighbourfile,"w");
		snprintf(coordinatefile,200,"%s/%s.coords.%g",argv[2],argv[2], temperature[i]);
		coordfp[i] = fopen(coordinatefile,"w");
		snprintf(subsystemsfile,200,"%s/%s.subsystems.%g",argv[2],argv[2], temperature[i]);
		subsystemsfp[i] = fopen(subsystemsfile,"w");
		snprintf(voisinsfile,200,"%s/%s.voisins.%g",argv[2],argv[2], temperature[i]);
		voisinsfp[i] = fopen(voisinsfile,"w");

		fprintf(neighbourfp[i],"#cell\ttarget_area\tarea\tpressure\tnumber_of_neighbours\tcurvature\tpsi_x\tpsi_y\tcelltype\tperimeter\n");
		fprintf(coordfp[i],"time\txcoord\tycoord\n");
		fprintf(subsystemsfp[i],"time\tnbr_large_bubbles\tnbr_small_bubbles\tlarge_mean_nbr_of_sides\tsmall_mean_nbr_of_sides\tlarge_mean_curvature\tsmall_mean_curvature\tlarge_temperature\tsmall_temperature\tlarge_muprime\tsmall_muprime\tBoundary_length\tSide_boundary_length12\tSide_boundary_length10\tSide boundary_length20\t<n>\tCPU_time\tEnergie\n");
		fprintf(voisinsfp[i],"#cell\t#cell_voisines\n");
		
		fclose(neighbourfp[i]);
		fclose(coordfp[i]);
		fclose(subsystemsfp[i]);
		fclose(voisinsfp[i]);
	}
	/*
	if (annealing)
		temperature[0] = temperature_max;
	*/

	if (load && !(strcmp(savefile, loadfile))) {
		fprintf(stderr, "You will erase previous data, because the name of old and new is the same. Stop program.\n");
		return EXIT_FAILURE;
	}
	
	for (int i=0;i<nb_temperature;i++) {//génération de graines pour générer des nombres aléatoires pour chaque thread
		InitAlea(i, (uint32_t)(seed*(i+1)));
	}
//----------------------------------génération de graines pour nombre aléatoires-------------------------	
	/*
	Si on génère la gaine localement:
	sfmt_t essai;
	sfmt_init_gen_rand(&essai, (uint32_t)(seed));
	sfmt_t * ess= &essai;
	
	
	//Si on génère la graine en faisant appel à une fonction:
	sfmt_t essai;
	sfmt_t * ess= &essai;	
	InitAlea2(ess, seed);
	
	//On vérifie que ça marche bien:
	for(int c=0;c<5;c++){
		printf ("%d: %f\t%f.\n",c, aleatoire2(ess), aleatoire2(ess));
	}
	
	//Même chose cette fois pour un tableau de graines:
	sfmt_t seeds[nb_temperature];
	
	for (int i=0;i<nb_temperature;i++) {
		InitAlea2(&seeds[i], (uint32_t)(seed*(i+1)));
		InitAlea(i, (uint32_t)(seed*(i+1)));
	}
	for(int c=0;c<5;c++){
		for (int i=0;i<nb_temperature;i++) {
			printf ("%d: %f\t%f.\n",c, aleatoire2(&seeds[i]), aleatoire(i));
		}
		printf ("\n");
	}
//	return EXIT_FAILURE;
	*/
//---------------------------------------------------------------------------------

	maxcells = (int)(ncol*nrow*fillfactor/target_area)+1;
	if (init_config==0 || init_config==6){
		maxcells=2; //si l'on veut qu'une seule bulle
	}
	if ( (init_config==4 || init_config==5) && (rx !=0) && (ry!=0) ){
		maxcells = (int)(ncol*nrow/(rx*ry))+1; //correction pour éviter les erreurs dues aux arrondis successifs
		printf("maxcells=%d\n", maxcells);
	}
	
	if (affichage>3)
		//ColorRandom(0,maxcells); //maxcells ne doit pas dépasser 451 (???) pour que cette fonction marche
		for (int i=1;i<maxcells+1;i++) {
			ColorRGB(i,(int)(0+254*aleatoire(0)),(int)(1+255*aleatoire(0)),(int)(1+255*aleatoire(0))); //crée des niveaux de 1 à 255 pour chaque canal, tout en excluant le noir (cellinterface) et le blanc (mediumcell).
			//ne pas se fier à l'affichage vidéo: le nombre de couleurs en sortie vidéo est limitée à 256, mais dans pour les fichiers png.
		}
	else {
		ColorTable(1,10,WHITE,GREEN,GREEN,YELLOW,YELLOW,GREY,BLUE,BLUE,RED,RED); //colors for graphical output: 1=white, 2=green, etc
		ColorRGB(5,200,200,255);
		ColorRGB(8,50,50,255);
	}
	cellinterface=0; //couleur des interfaces
	ColorRGB(cellinterface,0,0,1);//on le prend presque noir, pour pouvoir le distinguer du fond dans l'image raw.
	
	if (movie && neighbour_connected != 6) {
		OpenDisplay(output,nrow,ncol);//open an output window
		for (int i=0;i<nb_temperature;i++) {
			snprintf(subdirectory[i],200,"%s/T=%g",argv[2], temperature[i]);
			OpenCellPNG(subdirectory[i],nrow,ncol);//if we want to save a movie, make a directory
			snprintf(subdirectoryRAW[i],200,"%s/RAW-T=%g",argv[2], temperature[i]);
			OpenCellPNG(subdirectoryRAW[i],nrow,ncol);//if we want to save a movie, make a directory
			//OpenRAW(subdirectoryRAW[i],nrow,ncol);//if we want to save a movie, make a directory
		}
	} else if (movie) {
		snprintf(moviefile,200,"%s/%s",argv[2],argv[2]);
		int res = mkdir(moviefile, 0777);
	}

	
	// initialising and allocations

	for (int i=0;i<nb_temperature;i++) {
		state[i] = New();//allocate a field of nrow x ncol
		cells[i] = AllocateCells(maxcells, MAXNEIGHBOURS);
//		energie[i] = 0;
	}
	for (int i=0;i<nb_temperature-1;i++) {
		trial[i]=0;
		acceptance[i]=0;
	}
	if (annealing) {
		copynstate = New();
		if (!load)
			temperature[0]=temperature_min; //on ne garde cette température que jusqu'à dispersetime.
		else
			temperature[0]=temperature_max;
	}
	if(!annealing && nb_temperature==1){
		if (!load)
			temperature[0]=temperature_min; //on ne garde cette température que jusqu'à dispersetime.
		else
			temperature[0]=temperature_max;
	}

	nstate = New() ; copystate = New();
	copycells = AllocateCells(maxcells, MAXNEIGHBOURS);
	if((Jarray=(int **)calloc((size_t)maxtypes,sizeof(int *)))==NULL) {
		fprintf(stderr,"error in memory allocation Jarray\n");
		exit(EXIT_FAILURE);
	}
	for(int i=0;i<maxtypes;i++){
		if((Jarray[i]=(int *)calloc((size_t)maxtypes,sizeof(int)))==NULL) {
			fprintf(stderr,"error in memory allocation Jarray[i]\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i=0;i<maxtypes;i++){
		for(int j=i;j<maxtypes;j++){
			snprintf(Jval,200,"J%d%d",i,j);
			InDat("%d",Jval,&Jarray[i][j]);
			Jarray[j][i]=Jarray[i][j];
		}
	}

	//InitBubblePlane(1,(int)(fillfactor*(double)(nrow*ncol)/ (double)target_area),nrow,ncol, target_area, state[0], cells[0]);
	InitBubblePlane(init_config, fillfactor, nrow, ncol, target_area, rx, ry, state[0], cells[0],sliding);

	for (int i = 1;i<nb_temperature;i++) {
		Duplicate(cells[i], cells[0], maxcells);
		Copy(state[i],state[0]);
	}
	if (load) {
		//time
		loadfp = fopen(loadfile,"rb"); size_t nb = 0;
		nb = fread(&time_load, sizeof(int), 1, loadfp);
		if ((int)nb != 1) {
			fprintf(stderr, "%d, 1 attempted ; load error with time. Stop program.\n", (int)nb);
			return EXIT_FAILURE;
		}
		//state
		nb = 0;
		for (int d1=1;d1<=ncol;d1++) {
			for (int d2=1;d2<=nrow;d2++) {
				for (int d3=0;d3<nb_temperature;d3++) {
					nb += fread(&state[d3][d2][d1], sizeof(int), 1, loadfp);
				}
			}
		}
		if ((int)nb != ncol*nrow*nb_temperature) {
			fprintf(stderr, "%d, %d attempted ; load error with state. Stop program.\n", (int)nb, ncol*nrow*nb_temperature);
			return EXIT_FAILURE;
		}
		//cells
		nb = 0;
		for (int d=0;d<nb_temperature;d++) {
			for (int d2=0;d2<maxcells;d2++) {
				nb += fread(&cells[d].area[d2], sizeof(int), 1, loadfp);
				nb += fread(&cells[d].targetarea[d2], sizeof(int), 1, loadfp);
				nb += fread(&cells[d].celltype[d2], sizeof(int), 1, loadfp);
				nb += fread(&cells[d].perimeter[d2], sizeof(int), 1, loadfp);
				nb += fread(&cells[d].surf_energy[d2], sizeof(int), 1, loadfp);
			}
		}
		if ((int)nb != 4*maxcells*nb_temperature) {
			fprintf(stderr, "%d, %d attempted ; load error with cells. Stop program.\n", (int)nb, 4*maxcells*nb_temperature);
			return EXIT_FAILURE;
		}
		//energie
		nb = 0;
		for (int d=0;d<nb_temperature;d++) {
			nb += fread(&energie[d], sizeof(double), 1, loadfp);
			//Je préfère recalculer l'énergie proprement car il y avait des erreurs dans les simuls précédentes (cf git):
			energie[d]=ComputeEnergy(maxcells, cells[d], ncol, nrow, state[d], neighbour_energy, Jarray, area_constraint);
		}
		if ((int)nb != nb_temperature) {
			fprintf(stderr, "%d, %d attempted ; load error with energie. Stop program.\n", (int)nb, nb_temperature);
			return EXIT_FAILURE;
		}
		//trial
		nb = 0;
		for (int d=0;d<nb_temperature-1;d++) {
			nb += fread(&trial[d], sizeof(int), 1, loadfp);
		}
		if ((int)nb != nb_temperature-1) {
			fprintf(stderr, "%d, %d attempted ; load error with trial. Stop program.\n", (int)nb, nb_temperature-1);
			return EXIT_FAILURE;
		}
		//acceptance
		nb = 0;
		for (int d=0;d<nb_temperature-1;d++) {
			nb += fread(&acceptance[d], sizeof(int), 1, loadfp);
		}
		if ((int)nb != nb_temperature-1) {
			fprintf(stderr, "%d, %d attempted ; load error with acceptance. Stop program.\n", (int)nb, nb_temperature-1);
			return EXIT_FAILURE;
		}
		fread(&tmp_energie, sizeof(double), 1, loadfp);
		if (annealing) {
			temperature[0] = temperature_min;
			Duplicate(copycells,cells[0], maxcells);
			Copy(copystate,state[0]); //sauvegarde de la configuration avant recuît
			//Copy(copynstate,nstate);
		}
		dispersetime = 0;
		fclose(loadfp);
	}
	
//************************************************************************************************************************************
// Fin de l'initialisation
//------------------------------------------------------------------------------------------------------------------------------------
// Début de l'évolution temporelle
//************************************************************************************************************************************
	
	start = clock(); //initialisation du CPU time
	for (ttime=time_load; ttime <= totaltime; ttime++) {
		if (movie  && neighbour_connected != 6 && Mouse())
			break;
		if(ttime==dispersetime){
			deleted=GeneratePolydispersity(polydispersity,blob,maxcells,fillfactor,nrow,ncol,target_area,targetareamu2,target_area2,alpha,cells[0]);
			//InDat("%lf","area_constraint",&area_constraint);
			energie[0]=ComputeEnergy(maxcells, cells[0], ncol, nrow, state[0], neighbour_energy, Jarray, area_constraint); //on réinitialise l'énergie des systèmes
			ComputePerimeter(maxcells, cells[0], ncol, nrow, state[0], mediumcell, neighbour_energy); //calcul des périmètres des bulles
			//energie[0]=0;
			for (int i = 1;i<nb_temperature;i++) {
				Duplicate(cells[i], cells[0], maxcells);
				Copy(state[i],state[0]);
				energie[i]=energie[0];
			}
			if(deleted)
				printf("Deleted %d cells because of area restriction\n",deleted);
			if(!annealing && nb_temperature==1)
				temperature[0]=temperature_max;	//avec cette condition, la temperature du syst est Tmin jusqu'à dispersetime, puis passe à Tmax.
		}
		//Écriture.
		if((ttime>dispersetime)&&(!(ttime%saveinterval))){
			//if (annealing)
			//	wait = true;//wait the first time after annealing.
			//else {
				//printf("température = %lf\n", temperature[0]);
				savefp = fopen(savefile,"wb");
				fwrite(&ttime, sizeof(int), 1, savefp);
				for (int d1=1;d1<=ncol;d1++) {
					for (int d2=1;d2<=nrow;d2++) {
						for (int d3=0;d3<nb_temperature;d3++) {
							fwrite(&state[d3][d2][d1], sizeof(int), 1, savefp);
						}
					}
				}
				for (int d=0;d<nb_temperature;d++) {
					for (int d2=0;d2<maxcells;d2++) {
						fwrite(&cells[d].area[d2], sizeof(int), 1, savefp);
						fwrite(&cells[d].targetarea[d2], sizeof(int), 1, savefp);
						fwrite(&cells[d].celltype[d2], sizeof(int), 1, savefp);
						fwrite(&cells[d].perimeter[d2], sizeof(int), 1, savefp);
						fwrite(&cells[d].surf_energy[d2], sizeof(int), 1, savefp);
					}
				}
				for (int d=0;d<nb_temperature;d++) {
					fwrite(&energie[d], sizeof(double), 1, savefp);
				}
				for (int d=0;d<nb_temperature-1;d++) {
					fwrite(&trial[d], sizeof(int), 1, savefp);
				}
				for (int d=0;d<nb_temperature-1;d++) {
					fwrite(&acceptance[d], sizeof(int), 1, savefp);
				}
				//sauve l'énergie avant le dernier recuit:
				fwrite(&tmp_energie, sizeof(double), 1, savefp);
				fclose(savefp);
				//wait = false;
			//}
		}
		if (annealing && (ttime>=dispersetime) && (!((ttime+annealing_time)%writinginterval)) ){
			temperature[0] = temperature_min;
			tmp_energie = energie[0];
			Duplicate(copycells,cells[0], maxcells);
			Copy(copystate,state[0]); //sauvegarde de la configuration avant recuît
			Copy(copynstate,nstate);
		}
		if((ttime>=dispersetime)&&(!(ttime%writinginterval))){
			//printf("%d\n",ttime);
			for (int ind=0;ind<nb_temperature;ind++) {
				snprintf(neighbourfile,200,"%s/%s.neighbours.%g",argv[2],argv[2], temperature[ind]);
				neighbourfp[ind] = fopen(neighbourfile,"a");
				snprintf(coordinatefile,200,"%s/%s.coords.%g",argv[2],argv[2], temperature[ind]);
				coordfp[ind] = fopen(coordinatefile,"a");
				snprintf(subsystemsfile,200,"%s/%s.subsystems.%g",argv[2],argv[2], temperature[ind]);
				subsystemsfp[ind] = fopen(subsystemsfile,"a");
				snprintf(voisinsfile,200,"%s/%s.voisins.%g",argv[2],argv[2], temperature[ind]);
				voisinsfp[ind] = fopen(voisinsfile,"a");
				//printf("area et target_area du medium à la température %d: %d\n", ind, cells[ind].area[0], cells[ind].targetarea[0]);
				side_interface12=0;
				side_interface10=0;
				side_interface20=0;
				FindNeighbours(maxcells, cells[ind], ncol, nrow, state[ind], mediumcell, neighbour_connected, MAXNEIGHBOURS, &side_interface12, &side_interface10, &side_interface20);
				fprintf(neighbourfp[ind],"# time: %d\n",ttime);
				fprintf(voisinsfp[ind],"# time: %d\n",ttime);
				fprintf(coordfp[ind],"%d",ttime);
				nbrsides=0; nbrbulles=0; nbrsmall=0; smallnumberofsides=0; smallcurvature=0; nbrlarge=0; largenumberofsides=0; largecurvature=0;
				//ComputeCenterCoords(cells[ind], ncol, nrow, state[ind], maxcells, mediumcell);
				for(k=mediumcell+1;k<maxcells;k++){
					if(cells[ind].area[k]){
						fprintf(voisinsfp[ind],"%d", k);
						fprintf(coordfp[ind],"\t%d\t%d",(int)round(cells[ind].xcoord[k]), (int)round(cells[ind].ycoord[k]));
						nbrsides+=cells[ind].nneighbours[k];
						nbrbulles++;
						pression=2*area_constraint*(cells[ind].targetarea[k]-cells[ind].area[k])/cells[ind].targetarea[k];
						curvature=0;
						double xcenter=cells[ind].xcoord[k];						
						double ycenter=cells[ind].ycoord[k];
						double xcentervois;
						double ycentervois;
						/*Version to keep if iterative increasing of coordinate (sor cells escape from the initial frame):
						int xcenter=(int)(cells[ind].xcoord[k])%ncol;						
						int ycenter=(int)(cells[ind].ycoord[k])%nrow;
						int xcentervois;
						int ycentervois;	
						*/					
						complex double psi=0;
						double psi_x;
						double psi_y;
						double distance;
						for(int ii=0;ii<cells[ind].nneighbours[k];ii++){
							if(cells[ind].area[cells[ind].neighbours[k][ii]]>0)
								pressionvoisine=2*area_constraint*(cells[ind].targetarea[cells[ind].neighbours[k][ii]]-cells[ind].area[cells[ind].neighbours[k][ii]])/cells[ind].targetarea[cells[ind].neighbours[k][ii]];
							else
								pressionvoisine=0;
							/*if(cells[ind].area[cells[ind].neighbours[k][ii]]==0)
								printf("Cellule perdu !");*/
							curvature+=(pressionvoisine-pression)/(line_to_area*Jarray[cells[ind].celltype[k]][cells[ind].celltype[cells[ind].neighbours[k][ii]]]);
							
							//stockage de la liste des cellules voisines de chaque cellule:
							fprintf(voisinsfp[ind],"\t%d",cells[ind].neighbours[k][ii] );
							
							/*
							xcentervois=(int)(cells[ind].xcoord[cells[ind].neighbours[k][ii]])%ncol;
							ycentervois=(int)(cells[ind].ycoord[cells[ind].neighbours[k][ii]])%nrow;
							*/
							xcentervois=cells[ind].xcoord[cells[ind].neighbours[k][ii]];
							ycentervois=cells[ind].ycoord[cells[ind].neighbours[k][ii]];
							if (2*(xcentervois - xcenter) > ncol)
							{
								xcentervois=xcentervois - ncol;
							}
							else if (2*(xcenter - xcentervois) > ncol)
							{
								xcentervois=xcentervois + ncol;
							}
							
							
							if (2*(ycentervois - ycenter) > nrow)
							{
								ycentervois=ycentervois - nrow;
							}
							else if (2*(ycenter - ycentervois) > nrow)
							{
								ycentervois=ycentervois + nrow;
							}
							distance=sqrt(pow(xcentervois - xcenter,2)+pow(ycentervois - ycenter,2));								
							psi +=  cpow( (xcentervois - xcenter)/distance+I*(ycentervois - ycenter)/distance, 6);						
							//psi_y += 6*(ycentervois - ycenter)/distance;
																					
						}
						fprintf(voisinsfp[ind],"\n");
						psi_x=creal(psi/cells[ind].nneighbours[k]);
						psi_y=cimag(psi/cells[ind].nneighbours[k]);
						//printf("n°cell=%d \t xcoord=%lf \t ycoord=%lf \t psi=%lf\n",k,cells[ind].xcoord[k],cells[ind].ycoord[k],cabs(psi/cells[ind].nneighbours[k]));
						//mfacurvature=0.2814*cells[ind].nneighbours[k]*(cells[ind].nneighbours[k]-6)/sqrt(cells[ind].area[k]); // curvature of regular cell with same number of sides and same area
						fprintf(neighbourfp[ind],"%d\t%d\t%d\t%lg\t%d\t%lf\t%lf\t%lf\t%d\t%d\n",k,cells[ind].targetarea[k],cells[ind].area[k],pression,cells[ind].nneighbours[k],curvature,psi_x,psi_y,cells[ind].celltype[k],cells[ind].perimeter[k]);
						if(cells[ind].targetarea[k]==target_area){
							nbrlarge++;
							largenumberofsides+=cells[ind].nneighbours[k];
							//largesquaredsides+=pow(cells.nneighbours[k],2);
							largecurvature+=curvature;
							//largesquaredcurvature=pow(curvature,2);
						}
						else if(cells[ind].targetarea[k]==target_area2){
							nbrsmall++;
							smallnumberofsides+=cells[ind].nneighbours[k];
							//smallsquaredsides+=pow(cells.nneighbours[k],2);
							smallcurvature+=curvature;
							//smallsquaredcurvature=pow(curvature,2);
						}
					}
				}
	//			printf("Nombre moyen de côtés: %lf. Courbure moyenne: %lf\n",(float)nbrsides/nbrbulles, (largecurvature+smallcurvature)/nbrbulles); //Baisser la température si ce nombre s'écarte trop de sa valeur théorique, 6.
				fprintf(coordfp[ind],"\n");
				largetemperature=(2*3.14/(3*3.72))*(largecurvature/nbrlarge+9/sqrt(target_area)-pow(largenumberofsides/nbrlarge-3,2)/sqrt(target_area));
				smalltemperature=(2*3.14/(3*3.72))*(smallcurvature/nbrsmall+9/sqrt(target_area2)-pow(smallnumberofsides/nbrsmall-3,2)/sqrt(target_area2));
				largemuprime=2*3.14/(3*3.72)*(largenumberofsides/nbrlarge-3)/sqrt(target_area);
				smallmuprime=2*3.14/(3*3.72)*(smallnumberofsides/nbrsmall-3)/sqrt(target_area2);
				end = clock();
				cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
				if (side_interface10+side_interface20 > 0) {//cluster libre
					nmoyen_corrected = (float)(nbrsides+side_interface10+side_interface20)/(float)(nbrbulles-1);
				}
				else {//mousse bipériodique
					nmoyen_corrected = (float)(nbrsides)/(float)(nbrbulles);
				}
				//fprintf(subsystemsfp[ind],"%d\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\n",ttime,nbrlarge,nbrsmall,largenumberofsides/nbrlarge,smallnumberofsides/nbrsmall,largecurvature/nbrlarge,smallcurvature/nbrsmall,largetemperature,smalltemperature,largemuprime,smallmuprime, ComputeBoundary(cells[ind], ncol, nrow, state[ind], neighbour_energy), side_interface12, side_interface10, side_interface20 );
				//fprintf(subsystemsfp[ind],"%d\t%lf\t%lf\t%lf\t%lf\n",nbrsmall,smallmeansides/nbrsmall,sqrt(nbrsmall*smallsquaredsides/pow(smallmeansides,2)-1),smallmeancurvature/nbrsmall,sqrt(nbrsmall*smallsquaredcurvature/pow(smallmeancurvature,2)-1));
				fprintf(subsystemsfp[ind],"%d\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\t%f\t%f\t%f\n",ttime,nbrlarge,nbrsmall,largenumberofsides/nbrlarge,smallnumberofsides/nbrsmall,largecurvature/nbrlarge,smallcurvature/nbrsmall,largetemperature,smalltemperature,largemuprime,smallmuprime, ComputeBoundary(cells[ind], ncol, nrow, state[ind], neighbour_energy), side_interface12, side_interface10, side_interface20, nmoyen_corrected , cpu_time_used, energie[ind] );
				//fprintf(stdout,"time: %d\t energie: %f\t tmp_energie: %f\t area #1: %d\t target_area #1: %d\t perimeter #1: %d\n",ttime,energie[0],tmp_energie,cells[0].area[1],cells[0].targetarea[1],cells[0].perimeter[1]);
				//fprintf(stdout,"énergie=%F \t area[0]=%d \t area[1]=%d\n",energie[ind],cells[ind].area[mediumcell],cells[ind].area[1]);
				
				fclose(neighbourfp[ind]);
				fclose(coordfp[ind]);
				fclose(subsystemsfp[ind]);
				fclose(voisinsfp[ind]);
			}
			
		}
		if (movie && neighbour_connected != 6 && (ttime>=dispersetime)&&(!(ttime%movieinterval))){
			//Affichage carré.
			for (int ind=0;ind<nb_temperature;ind++) {
				//side_interface=0;
				//FindNeighbours(maxcells, cells[ind], ncol, nrow, state[ind], mediumcell, neighbour_connected, MAXNEIGHBOURS, &side_interface);
				AffichageCouleurs(affichage, cells[ind], ncol, nrow, subdirectory[ind], subdirectoryRAW[ind], state[ind], nstate);
				//AffichageCouleurs(affichage, nb_temperature, maxcells, cells[ind], ncol, nrow, *subdirectory[ind], state[ind], nstate);
			}
		} else if (movie && neighbour_connected == 6 && (ttime>dispersetime)&&(!(ttime%movieinterval))){
			//Affichage hexagonal.
			for (int ind=0;ind<nb_temperature;ind++) {
				snprintf(moviefile,200,"%s/%s/%d_%lf",argv[2],argv[2], ttime, temperature[ind]);
				moviefp = fopen(moviefile,"w");
				for (int col=1;col<=ncol;col++) {
					for (int row=1;row<=nrow;row++) {
						fprintf(moviefp, "%d ", state[ind][col][row]);
					}
					fprintf(moviefp, "\n");
				}
				fclose(moviefp);
			}
		}
		if (annealing && (ttime>=dispersetime) && (!(ttime%writinginterval)) ) {
			temperature[0] = temperature_max; //retour à la configuration avant le recuît
			energie[0] = tmp_energie;
			Copy(state[0],copystate);
			Copy(nstate,copynstate);
			Duplicate(cells[0], copycells, maxcells);
		}

		//Changement d'état ou de temperature.
		
/*		Partie dans laquelle on choisit àléatoirement les temps où l'on fait des tentaives de swaps
 		if (ttime>=dispersetime) {
			if (aleatoire()>proba_swap_temp) {
				#pragma omp parallel for
				for (int i=0;i<nb_temperature;i++)
					energie[i] += BubbleHamiltonian(neighbour_energy, neighbour_copy, neighbour_connected, ncol, nrow, state[i], mediumcell, heat_bath, Jarray, cells[i], area_constraint , temperature[i]);
				#pragma omp barrier
			} else if (nb_temperature > 1) {
				//swap temperature (choose one randomly)
				swapfp = fopen(swapfile,"a");
				int temp_choisi = (int)(aleatoire()*(nb_temperature-1));
				double calc = (1./temperature[temp_choisi]-1./temperature[temp_choisi+1])*(energie[temp_choisi]-energie[temp_choisi+1]);
				if (calc > 0. || aleatoire() < exp(calc)) {
					Duplicate(copycells, cells[temp_choisi], maxcells); Copy(copystate,state[temp_choisi]);
					Duplicate(cells[temp_choisi], cells[temp_choisi+1], maxcells); Copy(state[temp_choisi],state[temp_choisi+1]);
					Duplicate(cells[temp_choisi+1], copycells, maxcells); Copy(state[temp_choisi+1],copystate);
					double tmp_energie = energie[temp_choisi];
					energie[temp_choisi] = energie[temp_choisi+1]; energie[temp_choisi+1] = tmp_energie;
					fprintf(swapfp, "Swap %d and %d at %d\n", temp_choisi, temp_choisi+1, ttime);
				} else {
					fprintf(swapfp, "Swap failing (Delta E is %lg) between %d and %d at %d\n", calc, temp_choisi, temp_choisi+1, ttime);
				}
				fclose(swapfp);
			}
		} else
*/
		//ici les tentatives de swaps se font à intervalles fixes
		if (ttime>=dispersetime) {
			#pragma omp parallel
			{
			//sfmt_t sfmt;
			//sfmt_init_gen_rand(&sfmt, (uint32_t)time(NULL) ^ omp_get_thread_num());
			#pragma omp for
			for (int pi=0;pi<nb_temperature;pi++) {
				//for (int j=0;j<pi+1;j++) { boucle si l'on veut que l'on fasse plus de MCS dans les sytèmes à plus hautes températures
					//#pragma omp critical
					//{
					energie[pi] += BubbleHamiltonian(ttime, dispersetime, pi, neighbour_energy, neighbour_copy, neighbour_connected, ncol, nrow, state[pi], mediumcell, heat_bath, Jarray, cells[pi], area_constraint , temperature[pi]);
					//}
				//}
			}
			}
			#pragma omp barrier
			if (ttime%temperinginterval == 0 && nb_temperature > 1) {
				int startt;
				if ( ttime%(2*temperinginterval) == 0) {
					startt = 0;//Swap even
				} else {
					startt = 1;//Swap odd
				}
				swapfp = fopen(swapfile,"a");
				
				for (int temp_choisi=startt;temp_choisi<nb_temperature-1;temp_choisi+=2) {
					trial[temp_choisi]++;
					double calc = (1./temperature[temp_choisi]-1./temperature[temp_choisi+1])*(energie[temp_choisi]-energie[temp_choisi+1]);
					if (calc > 0. || aleatoire(temp_choisi) < exp(calc)) {
						Duplicate(copycells, cells[temp_choisi], maxcells); Copy(copystate,state[temp_choisi]);
						Duplicate(cells[temp_choisi], cells[temp_choisi+1], maxcells); Copy(state[temp_choisi],state[temp_choisi+1]);
						Duplicate(cells[temp_choisi+1], copycells, maxcells); Copy(state[temp_choisi+1],copystate);
						tmp_energie = energie[temp_choisi];
						energie[temp_choisi] = energie[temp_choisi+1]; energie[temp_choisi+1] = tmp_energie;
						fprintf(swapfp, "succeeds\t%d\t%d\t%d\t%lg\n", ttime, temp_choisi, temp_choisi+1, calc);
						acceptance[temp_choisi]++;
					} else {
						fprintf(swapfp, "fails\t%d\t%d\t%d\t%lg\n", ttime, temp_choisi, temp_choisi+1, calc);
					}
				}
				
				fclose(swapfp);
			}
		} else {
			BubbleHamiltonian(ttime, dispersetime, 0, neighbour_energy, neighbour_copy, neighbour_connected, ncol, nrow, state[0], mediumcell, heat_bath, Jarray, cells[0], area_constraint , temperature[0]);
		}
	}
	//On vérifie qu'il n'y a pas de fuite d'énergie:
	for (int d=0;d<nb_temperature;d++) {
		double energie_recalculee = ComputeEnergy(maxcells, cells[d], ncol, nrow, state[d], neighbour_energy, Jarray, area_constraint);
		//if (energie[d] != energie_recalculee){
		fprintf(stdout,"Vérification de l'énergie du système à la température n° %d : \n",d);
		fprintf(stdout,"valeur par itération : %f \t valeur recalculée: %f \n", energie[d], energie_recalculee);
		//}
	}
		
	
	//sauvegarde des taux d'acceptance
	acceptancefp = fopen(acceptancefile,"w");
	for (int i=0;i<nb_temperature-1;i++) {
		fprintf(acceptancefp,"%d\t%d\n", trial[i], acceptance[i]);
	}
	fclose(acceptancefp);
	
	//neatly close everything
	if (movie && neighbour_connected != 6) {
		CloseDisplay();
		//CloseCellPNG();//utile si on veut créer une video
	}
	//each call to calloc/malloc has to be freed seperately
	for (int i=0;i<nb_temperature;i++) {
		PlaneFree(state[i]);
		FreeCells(cells[i], maxcells);
	}
	if (annealing) {
		PlaneFree(copynstate);
	}
	PlaneFree(nstate);
	FreeCells(copycells, maxcells);
	for(int i=0;i<maxtypes;i++)
		free(Jarray[i]);
	
	return EXIT_SUCCESS;
}
