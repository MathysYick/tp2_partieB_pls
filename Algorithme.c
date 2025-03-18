#include "Algorithme.h"
#include "plaque.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "t_pile.h"

#define partie1 1
void initialiser_arbre(const t_plaque plaque_depart, int py_0, int px_0, t_liste_noeuds liste_noeuds, t_tab_ptr_niveaux niveaux) {
	t_noeud racine;
	memcpy(racine.plaque, plaque_depart, sizeof(t_plaque)); // Copie la plaque initiale

	//// Initialiser la position du zéro
	trouver_Valeur(racine.plaque, 0, &racine.posx_0, &racine.posy_0);

	racine.cout = evaluer_plaque(plaque_depart); 
	racine.profondeur = 0;
	racine.parent = -1; // Pas de parent car c'est la racine
	racine.terminal = 0;
	racine.dir = NULLE;
	racine.parent_index = -1;

	// Placer la racine dans la liste des nœuds
	liste_noeuds[0] = racine;

	//Initialise les niveaux
	niveaux[0] = 0;
	for (int i = 1;i < MAXNOEUDS;i++) {
		niveaux[i] = -1;
	}


}


int trouver_solution(t_liste_noeuds liste_noeuds, t_tab_ptr_niveaux niveaux, int max_cout, int* posfin) {
	bool terminal_exist = true;
	bool new_child = true;
	int nd_terminal = 0;
	int niveau = 0; // Dernier niveau (Losrque le niveau n'est plus égale à -1

	t_noeud child;

	child = liste_noeuds[0];		//Récupère la racine
	child.terminal = 1;
	liste_noeuds[0] = child;		

	while (terminal_exist || new_child) {
		//printf("\n\nDébut d'un nouveau noeud\n\n");
		new_child = false;
		terminal_exist = false;

		for (int i = MAXNOEUDS - 1; i > 0; i--) {  //Trouver le niveau le plus haut
			if (niveaux[i] != -1) {
				niveau = i;
				break;
			}
		}

		printf("Cout MAX %d #################################################\n\n\n", max_cout);
		printf("Plaque originale\n");
		afficher_plaque(liste_noeuds[0].plaque);

		//printf("dernier noeuds est à %d tab niveau %d\n", niveau, niveaux[niveau]);


		//Étape 2-4
		for (int i = MAXNOEUDS - 1; i >= 0; i--) {
			child = liste_noeuds[i];
			if (child.terminal == 1) { //Trouver le dernier noeud terminal
				nd_terminal = i;
				terminal_exist = true;
				child.terminal = 0;
				liste_noeuds[i] = child; // Mettre à jour le noeud (type terminal)
				break;
			}

		}
	

		t_noeud parent;
		if (child.parent == -1) { //copie de l'enfant dans le parent puisque c'est le noeud originale
			parent = child;
		}
		else {
			parent = liste_noeuds[child.parent]; // Copie le parent de l'enfant
		}

		//Étape 3
		if (!terminal_exist) { // Si aucun noeud terminal n'est trouvé, reset des noeuds
			for (int i = MAXNOEUDS - 1; i > 0; i--) {
				niveaux[i] = -1;
			}
			return 0;
		}


		//Étape 5-6
		if (evaluer_plaque(child.plaque) == 0) { // Si la plaque est la solution, on arrête

			for (int i = 0; i < MAXNOEUDS; i++) { //Trouve le niveau du noeud terminal
				if (niveaux[i] != -1) {
					niveau = i;
				}
			}
			niveau++;
			niveaux[niveau] = nd_terminal;
			*posfin = nd_terminal;
			return 1;
		}
		/*else {
			printf("plaque n'est pas la solution\n");
		}*/

		//Étape 7
		//printf("\n\n\nEtape 7 \n");

		//printf("Génération des enfants\n");
		int parent_index = nd_terminal;
		parent = liste_noeuds[nd_terminal]; // Copie l'enfant du parent
		for (int i = 0; i < 4;i++) {

			child = parent; // Copie l'enfant du parent
			//printf("\nEnfant %d du parent %d\n", i, child.parent);

			if (changer_plaque(child.plaque, i, &child.posx_0, &child.posy_0)) {// Modifie la plaque selon la direction
				if ((child.posx_0 != parent.posx_0) || (child.posy_0 != parent.posy_0)) { //Prévient en retour en arrière en comparant la position de la case vide
					//printf("Le noeud qu'on explore est le %d, parent %d\n", nd_terminal,liste_noeuds[nd_terminal].parent);

					child.cout = evaluer_plaque(child.plaque) + child.profondeur; // Calcul du cout

					if (child.cout < max_cout) { // Si le cout est inférieur au cout max, le noeud est valide
						child.parent = nd_terminal; // Le parent est le noeud terminal
						child.terminal = 1;	//Noeud n'est pas encore exploré
						child.dir = i; // Direction du mouvement
						child.profondeur++; // Augmente la profondeur

						if (nd_terminal == 0) {
							nd_terminal = 1;
						}
						niveaux[niveau + i] = nd_terminal + i; // Ajoute le noeud à la liste des niveaux
						memcpy(&liste_noeuds[nd_terminal + i], &child, sizeof(t_noeud)); // Copie le noeud dans la liste des noeuds
						new_child = true; // Nouveau noeud ajouté, donc on continue avec le meme cout max

					}
					/*else {
						printf("Cout trop elever (%d> %d)\n", child.cout, max_cout);
						printf("ENFANT %d NON AJOUTÉ\n", i);
					}*/
				}
				/*else {
					printf("RETOUR AU PARENT\n");
					printf("ENFANT %d NON AJOUTÉ\n", i);
				}*/
			}
			/*else {
				printf("MOUVEMENT IMPOSSIBLE\n");
				printf("ENFANT %d NON AJOUTÉ\n", i);
			}*/

		}
	}
	return 0;
}

void afficher_solution(const t_liste_noeuds liste_noeuds, int posfin) {
	t_pile arbre = creer_pile(MAXNOEUDS);

	//// Remonter les parents jusqu'à l'état initial
	t_element coup_act = { 0 };
	//printf("On part de la solution finale et on remonte...\n");
	while (posfin != -1) {
		//delay(1);
		coup_act.direction = liste_noeuds[posfin].dir;
		coup_act.niveau = liste_noeuds[posfin].profondeur;


		push_pile(&arbre, coup_act); // Stocker le mouvement
		posfin = liste_noeuds[posfin].parent; // Remonter au parent pour la prochaine itération
		//printf("ele %d\n", get_nb_elements(&arbre));
		//afficher_plaque(liste_noeuds[posfin].plaque);

		if ((liste_noeuds[posfin].posx_0 == liste_noeuds[liste_noeuds[posfin].parent].posx_0) && (liste_noeuds[liste_noeuds[posfin].parent].posy_0 == liste_noeuds[posfin].posy_0)) { //Prévient en retour en arrière en comparant la position de la case vide
			printf("plaque deviennent récusive\n");
			break;
		}
	}


	//}

	// Affichage des mouvements dans l'ordre correct
	printf("Solution: \n");
	while (!pile_vide(&arbre)) {
		pop_pile(&arbre, &coup_act);
		char dir = ' ';
		switch (coup_act.direction) {
		case 0:
			dir = 'H';
			break;
		case 1:
			dir = 'G';
			break;

		case 2:
			dir = 'B';
			break;
		case 3:
			dir = 'D';
			break;
		}
		printf("%d -> %c \n", coup_act.niveau, dir); // Dépiler pour afficher dans l'ordre correct
	}
	printf("Avec profondeur %d\n\n", liste_noeuds[posfin].profondeur);

	printf("Cout initial = %d, Final = %d, nb. Iterations = %d\n", liste_noeuds[0].cout, liste_noeuds[posfin].cout, liste_noeuds[posfin].cout - liste_noeuds[posfin].profondeur);

	liberer_pile(&arbre);
}