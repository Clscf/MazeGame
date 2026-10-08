#include "affichage.h"
#include <stdio.h>

void displayMaze(const maze_t *maze){
    if (maze == NULL || maze->labyrinthe == NULL){
        return;
    }

    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            switch (maze->labyrinthe[i][j])
            {
            case 0:
            case '#':
                printf("#");
                break;
            case -1:
            case 'o':
                printf("o");
                break;
            case -2:
            case '-':
                printf("-");
                break;
            default:
                printf(" ");
                break;
            }
        }
        printf("\n");
    }
}

int menu(void){
    int choix =0;
    printf("Bonjour, que voulez-vous faire:\n"
    "- 1: Créer un labyrinthe\n"
    "- 2: Charger un labyrinthe\n"
    "- 3: Jouer\n"
    "- 4: Quitter\n");
    do{
        scanf("%d", &choix);
        int c;
        while ((c = getchar()) != '\n' && c != EOF){};
    }while (choix < 1 || choix > 4);
    
    return choix;
}