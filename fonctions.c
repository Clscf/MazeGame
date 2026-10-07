#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fonctions.h"

void generateMaze(maze_t maze){
    int compteur = 1;
    for (int i = 0; i < maze.hauteur; i++){
        for (int j = 0; j < maze.largeur; j++){
            if (j%2 == 0 && (i != 0 && i != maze.hauteur-1) && (i%2 != 0)){
                maze.labyrinthe[i][j] = 0;
            }
            else if (i == 0 || i == maze.hauteur-1 || i%2 == 0){
                maze.labyrinthe[i][j] = 0;
            }
            else{
                maze.labyrinthe[i][j] = compteur;
                compteur++;
            }
        }
    }
    maze.labyrinthe[0][1] = -1;
    maze.labyrinthe[maze.hauteur - 1][maze.largeur - 2] = -2;
}

void mazePath(maze_t maze){
    int regions = 0;

    for (int i = 1; i < maze.hauteur - 1; i += 2){
        for (int j = 1; j < maze.largeur - 1; j += 2){
            if (maze.labyrinthe[i][j] > 0){
                regions++;
            }
        }
    }

    while (regions > 1){
        int mursValides = 0;

        for (int i = 1; i < maze.hauteur - 1; i++){
            for (int j = 1; j < maze.largeur - 1; j++){
                int vertical = 0;
                int horizontal = 0;
                if (i % 2 == 0 && j % 2 == 1 && maze.labyrinthe[i][j] == 0 &&
                    maze.labyrinthe[i - 1][j] > 0 && maze.labyrinthe[i + 1][j] > 0 &&
                    maze.labyrinthe[i - 1][j] != maze.labyrinthe[i + 1][j]){
                        vertical = 1;
                    }
                if (i % 2 == 1 && j % 2 == 0 && maze.labyrinthe[i][j] == 0 &&
                    maze.labyrinthe[i][j - 1] > 0 && maze.labyrinthe[i][j + 1] > 0 &&
                    maze.labyrinthe[i][j - 1] != maze.labyrinthe[i][j + 1]){
                        horizontal = 1;
                    }
                if (vertical || horizontal){
                    mursValides++;
                }
            }
        }

        if (mursValides == 0){
            break;
        }

        int choix = rand() % mursValides;
        int x = -1;
        int y = -1;

        for (int i = 1; i < maze.hauteur - 1 && x == -1; i++){
            for (int j = 1; j < maze.largeur - 1; j++){
                int vertical = 0;
                int horizontal = 0;
                if (i % 2 == 0 && j % 2 == 1 && maze.labyrinthe[i][j] == 0 &&
                    maze.labyrinthe[i - 1][j] > 0 && maze.labyrinthe[i + 1][j] > 0 &&
                    maze.labyrinthe[i - 1][j] != maze.labyrinthe[i + 1][j]){
                        vertical = 1;
                    }
                if (i % 2 == 1 && j % 2 == 0 && maze.labyrinthe[i][j] == 0 &&
                    maze.labyrinthe[i][j - 1] > 0 && maze.labyrinthe[i][j + 1] > 0 &&
                    maze.labyrinthe[i][j - 1] != maze.labyrinthe[i][j + 1]){
                        horizontal = 1;
                    }
                if (vertical || horizontal){
                    if (choix == 0){
                        x = i;
                        y = j;
                        break;
                    }
                    choix--;
                }
            }
        }

        int nouvelleRegion;
        int ancienneRegion;
        int vertical = maze.labyrinthe[x][y] == 0 &&
            maze.labyrinthe[x - 1][y] > 0 && maze.labyrinthe[x + 1][y] > 0 &&
            maze.labyrinthe[x - 1][y] != maze.labyrinthe[x + 1][y];

        if (vertical){
            nouvelleRegion = maze.labyrinthe[x - 1][y];
            ancienneRegion = maze.labyrinthe[x + 1][y];
        }
        else{
            nouvelleRegion = maze.labyrinthe[x][y - 1];
            ancienneRegion = maze.labyrinthe[x][y + 1];
        }

        maze.labyrinthe[x][y] = nouvelleRegion;

        for (int i = 1; i < maze.hauteur - 1; i++){
            for (int j = 1; j < maze.largeur - 1; j++){
                if (maze.labyrinthe[i][j] == ancienneRegion){
                    maze.labyrinthe[i][j] = nouvelleRegion;
                }
            }
        }

        regions--;
    }
}

void displayMaze(maze_t maze){
    for (int i = 0; i < maze.hauteur; i++){
        for (int j = 0; j < maze.largeur; j++){
            switch (maze.labyrinthe[i][j])
            {
            case 0:
                printf("#");
                break;
            case -1:
                printf("o");
                break;
            case -2:
                printf("-");
                break;
            case '#':
                printf("#");
                break;
            case 'o':
                printf("o");
                break;
            case '-':
                printf("-");
                break;
            default:
                //printf("%d", maze.labyrinthe[i][j]);
                printf(" ");
                break;
            }
        }
        printf("\n");
    }
}

char** replaceMaze(maze_t maze){
    char** newMaze;
    newMaze = malloc(maze.hauteur * sizeof(int *));
    for (int i = 0; i < maze.hauteur; i++) {
        newMaze[i] = malloc(maze.largeur * sizeof(int));
    }
    for (int i = 0; i < maze.hauteur; i++){
        for (int j = 0; j < maze.largeur; j++){
            switch (maze.labyrinthe[i][j])
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
        newMaze[i][maze.largeur] = '\0';
    }
    return newMaze;
    free(newMaze);
}

int menu(){
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

maze_t createMaze(){
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
    maze_t newMaze = {coordonnees[0], coordonnees[1], laby, name};
    generateMaze(newMaze);
    mazePath(newMaze);
    fileWrite(newMaze, replaceMaze(newMaze));
    return newMaze;
}

void fileWrite(maze_t maze ,char** newMaze){
    char chemin[150];

    snprintf(chemin, sizeof(chemin), "Labyrinthes/%s.cfg", maze.nom);

    FILE *newFile = fopen(chemin, "w");

    if (newFile == NULL){
        printf("Impossible d'ouvrir le fichier\n");
        return;
    }

    for (int i = 0; i < maze.hauteur; i++){
        for (int j = 0; j < maze.largeur; j++){
            fprintf(newFile, "%c", newMaze[i][j]);
        }
        fprintf(newFile, "\n");
    }
    fclose(newFile);
}

maze_t loadMaze(){
    char name[100];
    char chemin[150];
    maze_t maze = {0, 0, NULL, NULL};

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

    maze.hauteur = hauteur;
    maze.largeur = largeur;
    maze.labyrinthe = malloc((size_t) hauteur * sizeof *maze.labyrinthe);
    if (maze.labyrinthe == NULL) {
        fclose(file);
        maze.hauteur = 0;
        maze.largeur = 0;
        return maze;
    }

    for (int i = 0; i < hauteur; i++) {
        maze.labyrinthe[i] = malloc((size_t) largeur * sizeof *maze.labyrinthe[i]);
        if (maze.labyrinthe[i] == NULL) {
            for (int ligne = 0; ligne < i; ligne++) {
                free(maze.labyrinthe[ligne]);
            }
            free(maze.labyrinthe);
            fclose(file);
            maze.labyrinthe = NULL;
            maze.hauteur = 0;
            maze.largeur = 0;
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
            maze.labyrinthe[i][j] = caractere;
            j++;
        }
    }

    maze.nom = malloc(strlen(name) + 1);
    if (maze.nom != NULL) {
        strcpy(maze.nom, name);
    }

    fclose(file);
    return maze;
}

void game(maze_t maze){
    char move = '\0';
    int win = -1;
    displayMaze(maze);
    do {
        scanf("%c", &move);
        win = movement(move, maze);
    } while (win != 1);
    displayMaze(maze);
}

int movement(char move, maze_t maze){
    displayMaze(maze);
    int player[2];
    for (int i = 0; i < maze.hauteur; i++){
        for (int j = 0; j < maze.largeur; j++){
            if (maze.labyrinthe[i][j] == 'o'){
                player[0] = i;
                player[1] = j; 
            }
        }
    }
    switch (move)
    {
    case 'z':
        if (maze.labyrinthe[player[0]-1][player[1]] == '#' || player[0]-1 < 0){
            return -1;
        }
        maze.labyrinthe[player[0]-1][player[1]] = 'o';
        maze.labyrinthe[player[0]][player[1]] = ' ';
        break;
    case 'q':
        if (maze.labyrinthe[player[0]][player[1]-1] == '#'){
            return -1;
        }
        maze.labyrinthe[player[0]][player[1]-1] = 'o';
        maze.labyrinthe[player[0]][player[1]] = ' ';
        break;
    case 'd':
        if (maze.labyrinthe[player[0]][player[1]+1] == '#'){
            return -1;
        }
        maze.labyrinthe[player[0]][player[1]+1] = 'o';
        maze.labyrinthe[player[0]][player[1]] = ' ';
        break;
    case 's':
        if (maze.labyrinthe[player[0]+1][player[1]] == '#' || player[0]+1 > maze.hauteur){
            return -1;
        }
        else if (maze.labyrinthe[player[0]+1][player[1]] == '-'){
            maze.labyrinthe[player[0]+1][player[1]] = 'o';
            maze.labyrinthe[player[0]][player[1]] = ' ';
            return 1;
        }
        maze.labyrinthe[player[0]+1][player[1]] = 'o';
        maze.labyrinthe[player[0]][player[1]] = ' ';
        break;
    default:
        return -1;
    }
    return 0;
}
