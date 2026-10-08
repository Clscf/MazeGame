#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maze.h"
#include "generation.h"
#include "fichier.h"
#include "jeu.h"
#include "affichage.h"

int main(void){
    srand(time(NULL));
    int choix;
    maze_t *maze;
    do{
        choix = menu();
        switch (choix)
        {
        case 1:
            free(maze);
            maze = createMaze();
            displayMaze(maze);
            break;
        case 2:
            maze = loadMaze();
            displayMaze(maze);
            break;
        case 3:
            if (maze == NULL){
                printf("Veuillez charger un labyrinthe avant de jouer");
            }
            else{
                game(maze);
            }
            break;
        case 4:
            exit(-1);
            break;
        default:
            break;
        }
    } while (choix != 4);
    return 0;
}