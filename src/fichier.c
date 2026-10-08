#include "fichier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void freeMazeText(char **mazeText, int hauteur){
    if (mazeText == NULL){
        return;
    }

    for (int i = 0; i < hauteur; i++){
        free(mazeText[i]);
    }
    free(mazeText);
}

char **replaceMaze(const maze_t *maze){
    char** newMaze;
    newMaze = malloc((size_t) maze->hauteur * sizeof *newMaze);
    for (int i = 0; i < maze->hauteur; i++) {
        newMaze[i] = malloc((size_t) maze->largeur + 1);
    }
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            switch (maze->labyrinthe[i][j])
            {
            case 0:
                newMaze[i][j] = '#';
                break;
            case -1:
                newMaze[i][j] = 'o';
                break;
            case -2:
                newMaze[i][j] = '-';
                break;
            default:
                newMaze[i][j] = ' ';
                break;
            }
        }
        newMaze[i][maze->largeur] = '\0';
    }
    return newMaze;
}

maze_t *loadMaze(void){
    char name[100];
    char chemin[150];
    maze_t *maze = calloc(1, sizeof *maze);
    if (maze == NULL) {
        return NULL;
    }

    do {
        printf("Saisissez le nom du labyrinthe\n");
        scanf("%99s", name);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    } while (name[0] == '\0');

    snprintf(chemin, sizeof chemin, "Labyrinthes/%s.cfg", name);
    FILE *file = fopen(chemin, "r");
    if (file == NULL) {
        printf("Le fichier n'existe pas\n");
        return maze;
    }

    int hauteur = 0;
    int largeur = 0;
    int largeurLigne = 0;
    int caractere;

    while ((caractere = fgetc(file)) != EOF) {
        if (caractere == '\n') {
            if (largeurLigne > largeur) {
                largeur = largeurLigne;
            }
            hauteur++;
            largeurLigne = 0;
        } else if (caractere != '\r') {
            largeurLigne++;
        }
    }

    if (largeurLigne > 0) {
        if (largeurLigne > largeur) {
            largeur = largeurLigne;
        }
        hauteur++;
    }

    if (hauteur == 0 || largeur == 0) {
        fclose(file);
        return maze;
    }

    maze->hauteur = hauteur;
    maze->largeur = largeur;
    maze->labyrinthe = malloc((size_t) hauteur * sizeof *maze->labyrinthe);
    if (maze->labyrinthe == NULL) {
        fclose(file);
        maze->hauteur = 0;
        maze->largeur = 0;
        return maze;
    }

    for (int i = 0; i < hauteur; i++) {
        maze->labyrinthe[i] = malloc((size_t) largeur * sizeof *maze->labyrinthe[i]);
        if (maze->labyrinthe[i] == NULL) {
            for (int ligne = 0; ligne < i; ligne++) {
                free(maze->labyrinthe[ligne]);
            }
            free(maze->labyrinthe);
            fclose(file);
            maze->labyrinthe = NULL;
            maze->hauteur = 0;
            maze->largeur = 0;
            return maze;
        }
    }

    rewind(file);
    int i = 0;
    int j = 0;
    while ((caractere = fgetc(file)) != EOF && i < hauteur) {
        if (caractere == '\n') {
            i++;
            j = 0;
        } else if (caractere != '\r' && j < largeur) {
            maze->labyrinthe[i][j] = caractere;
            j++;
        }
    }

    maze->nom = malloc(strlen(name) + 1);
    if (maze->nom != NULL) {
        strcpy(maze->nom, name);
    }

    fclose(file);
    return maze;
}

void fileWrite(const maze_t *maze, char **newMaze){
    char chemin[150];

    snprintf(chemin, sizeof(chemin), "Labyrinthes/%s.cfg", maze->nom);

    FILE *newFile = fopen(chemin, "w");

    if (newFile == NULL){
        printf("Impossible d'ouvrir le fichier\n");
        freeMazeText(newMaze, maze->hauteur);
        return;
    }

    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            fprintf(newFile, "%c", newMaze[i][j]);
        }
        fprintf(newFile, "\n");
    }
    fclose(newFile);
    freeMazeText(newMaze, maze->hauteur);
}