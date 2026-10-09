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
    maze_t *maze = NULL;
    stats_t stats = {0, 0};
    do{
        choix = menu();
        switch (choix)
        {
        case 1:
            if (maze != NULL){
                free(maze);
            }
            maze = createMaze();
            displayMaze(maze);
            break;
        case 2:
            if (maze != NULL){
                free(maze);
            }
            maze = loadMaze();
            displayMaze(maze);
            break;
        case 3:
            if (maze == NULL){
                printf("Veuillez charger un labyrinthe avant de jouer\n");
            }
            else{
                game(maze, &stats);
            }
            break;
        case 4:
            if (maze == NULL){
                printf("Veuillez charger un labyrinthe avant de visualiser les scores\n");
            }
            else{
                showScores(maze);
            }
            break;
        case 5:
            exit(-1);
            break;
        default:
            break;
        }
    } while (choix != 5);
    return 0;
}