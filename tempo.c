//on teste maintenant la connexité locale de la bulle "cible". Si celle-ci ne l'était pas, la modification de la valeur du pixel entrainerait
				//la création d'une arche, et donc celle-ci deviendrait multiplement connexe, ce qu'on veut empêcher.
				//sub_flag=false; // flag qui devient true si la valeur du pixel est effectivement modifiée.
				icandidate=possible_state_copy[(int)(aleatoire(num)*pos_neg)][0];
                if ( (pixel == mediumcell) || (icandidate == mediumcell) || (icandidate == pixel)) {
					//Pas de problèmes d'arche ici, ou bien calculs inutiles.
					sub_flag=true;
                   
                } else {
                    //On compte simplement le nombre de voisins (voisinage de connexité) dont l'état est identique au nouvel état du site.
                    int voisins_identiques = 0;
                    if (condwrap) {
	                    for(int j=0;j<neighbour_connected;j++) {
	                        if (state[PeriodicWrap(y+ny[j], nrow)][PeriodicWrap(x+nx[j], ncol)] == icandidate)
	                            voisins_identiques += 1;
	                    }
	                } else {
						for(int j=0;j<neighbour_connected;j++) {
	                        if (state[y+ny[j]][x+nx[j]] == icandidate)
	                            voisins_identiques += 1;
	                    }
	                }
                    //ÇA NE MARCHE QUE POUR UN RÉSEAU DE COPIE 4 ÉCRIT COMME ÇA.
                    sub_flag=connected_4(state, voisins_identiques, condwrap, icandidate, ncol, nrow, x, y);
                    //Cas "normal" : pas de bulle en entourant une autre (ou bien mediumcell).
                    
                }
			
				if(sub_flag){
					
					//-------------------------------------------------------------------------------------------------------------
					
					

								icandidate = possible_state_copy[pos][0];
								sum_E = delta_E[pos];
								cells.xcoord[pixel]=(cells.xcoord[pixel]*cells.area[pixel]-round((cells.xcoord[pixel]-x)/ncol)*ncol-x)/(cells.area[pixel]-1);
								cells.ycoord[pixel]=(cells.ycoord[pixel]*cells.area[pixel]-round((cells.ycoord[pixel]-y)/nrow)*nrow-y)/(cells.area[pixel]-1);
								cells.xcoord[icandidate]=(cells.xcoord[icandidate]*cells.area[icandidate]+round((cells.xcoord[icandidate]-x)/ncol)*ncol+x)/(cells.area[icandidate]+1);
								cells.ycoord[icandidate]=(cells.ycoord[icandidate]*cells.area[icandidate]+round((cells.ycoord[icandidate]-y)/nrow)*nrow+y)/(cells.area[icandidate]+1);
								cells.area[pixel]--;
								cells.area[icandidate]++;
								//cells.area[mediumcell]=0;
								
								//sum_E = variations_E;
								state[y][x]=icandidate;
								break;
