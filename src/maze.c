#include "maze.h"
#include "generation.h"
#include "fichier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

maze_t *createMaze(void){
    int coordonnees[2];
    char name[100];
    char expectedName[150];
    char confirmation;
    int existe = 0;
    do {
        existe = 0;
        printf("Saisissez le nom du labyrinthe\n");
        scanf("%99s", name);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
        DIR *directory = opendir("Labyrinthes");
        if (directory == NULL){
            printf("Impossible d'ouvrir le dossier des labyrinthes\n");
            return NULL;
        }

        struct dirent *entry;
        while ((entry = readdir(directory)) != NULL){
            const char *extension = strrchr(entry->d_name, '.');
            if (extension != NULL && strcmp(extension, ".cfg") == 0){
                snprintf(expectedName, sizeof expectedName, "%s.cfg", name);
                if (strcmp(entry->d_name, expectedName) == 0){
                    printf("Ce nom de labyrinthe existe déjà \n");
                    existe++;
                }
            }
        }
        closedir(directory);
    } while (name[0] == '\0' || existe != 0);

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
    displayMaze(newMaze);
    do {
        printf("Voulez-vous enregistrer ce labyrinthe [y/n]\n");
        scanf("%c", &confirmation);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    } while (confirmation != 'y' && confirmation != 'n');
    if (confirmation == 'y'){
        fileWrite(newMaze, replaceMaze(newMaze));
    }
    else{
        return NULL;
    }
    return newMaze;
}