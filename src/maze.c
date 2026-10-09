#include "maze.h"
#include "generation.h"
#include "fichier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

maze_t *createMaze(void){
    int coordonnees[2];
    char name[100];
    do {
        printf("Saisissez le nom du labyrinthe\n");
        scanf("%99s", name);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    } while (name[0] == '\0');
    printf("Veuillez saisir la hauteur puis la largeur du labyrinthe (minimum 3*3), ces coordonnées doivent être impair:\n");
    for (int i = 0; i < 2; i++){
        do{
            scanf("%d", &coordonnees[i]);
            int c;
            while ((c = getchar()) != '\n' && c != EOF){};
        }while (coordonnees[i] < 3 || (coordonnees[i]%2) == 0);
    }
    int **laby = NULL;
    laby = malloc(coordonnees[0] * sizeof(int *));
    for (int i = 0; i < coordonnees[0]; i++) {
        laby[i] = malloc(coordonnees[1] * sizeof(int));
    }
    maze_t *newMaze = malloc(sizeof *newMaze);
    if (newMaze == NULL) {
        return NULL;
    }

    newMaze->hauteur = coordonnees[0];
    newMaze->largeur = coordonnees[1];
    newMaze->labyrinthe = laby;
    newMaze->nom = malloc(strlen(name) + 1);
    if (newMaze->nom == NULL) {
        return newMaze;
    }
    strcpy(newMaze->nom, name);

    generateMaze(newMaze);
    mazePath(newMaze);
    generateItems(newMaze);
    fileWrite(newMaze, replaceMaze(newMaze));
    return newMaze;
}