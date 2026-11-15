#include "generation.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateMaze(maze_t *maze){
    int compteur = 1;
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            if (j%2 == 0 && (i != 0 && i != maze->hauteur-1) && (i%2 != 0)){
                maze->labyrinthe[i][j] = 0;
            }
            else if (i == 0 || i == maze->hauteur-1 || i%2 == 0){
                maze->labyrinthe[i][j] = 0;
            }
            else{
                maze->labyrinthe[i][j] = compteur;
                compteur++;
            }
        }
    }
    maze->labyrinthe[0][1] = -1;
    maze->labyrinthe[maze->hauteur - 1][maze->largeur - 2] = -2;
}

void mazePath(maze_t *maze){
    int regions = 0;

    for (int i = 1; i < maze->hauteur - 1; i += 2){
        for (int j = 1; j < maze->largeur - 1; j += 2){
            if (maze->labyrinthe[i][j] > 0){
                regions++;
            }
        }
    }

    while (regions > 1){
        int mursValides = 0;

        for (int i = 1; i < maze->hauteur - 1; i++){
            for (int j = 1; j < maze->largeur - 1; j++){
                int vertical = 0;
                int horizontal = 0;
                if (i % 2 == 0 && j % 2 == 1 && maze->labyrinthe[i][j] == 0 &&
                    maze->labyrinthe[i - 1][j] > 0 && maze->labyrinthe[i + 1][j] > 0 &&
                    maze->labyrinthe[i - 1][j] != maze->labyrinthe[i + 1][j]){
                        vertical = 1;
                    }
                if (i % 2 == 1 && j % 2 == 0 && maze->labyrinthe[i][j] == 0 &&
                    maze->labyrinthe[i][j - 1] > 0 && maze->labyrinthe[i][j + 1] > 0 &&
                    maze->labyrinthe[i][j - 1] != maze->labyrinthe[i][j + 1]){
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

        for (int i = 1; i < maze->hauteur - 1 && x == -1; i++){
            for (int j = 1; j < maze->largeur - 1; j++){
                int vertical = 0;
                int horizontal = 0;
                if (i % 2 == 0 && j % 2 == 1 && maze->labyrinthe[i][j] == 0 &&
                    maze->labyrinthe[i - 1][j] > 0 && maze->labyrinthe[i + 1][j] > 0 &&
                    maze->labyrinthe[i - 1][j] != maze->labyrinthe[i + 1][j]){
                        vertical = 1;
                    }
                if (i % 2 == 1 && j % 2 == 0 && maze->labyrinthe[i][j] == 0 &&
                    maze->labyrinthe[i][j - 1] > 0 && maze->labyrinthe[i][j + 1] > 0 &&
                    maze->labyrinthe[i][j - 1] != maze->labyrinthe[i][j + 1]){
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
        int vertical = maze->labyrinthe[x][y] == 0 &&
            maze->labyrinthe[x - 1][y] > 0 && maze->labyrinthe[x + 1][y] > 0 &&
            maze->labyrinthe[x - 1][y] != maze->labyrinthe[x + 1][y];

        if (vertical){
            nouvelleRegion = maze->labyrinthe[x - 1][y];
            ancienneRegion = maze->labyrinthe[x + 1][y];
        }
        else{
            nouvelleRegion = maze->labyrinthe[x][y - 1];
            ancienneRegion = maze->labyrinthe[x][y + 1];
        }

        maze->labyrinthe[x][y] = nouvelleRegion;

        for (int i = 1; i < maze->hauteur - 1; i++){
            for (int j = 1; j < maze->largeur - 1; j++){
                if (maze->labyrinthe[i][j] == ancienneRegion){
                    maze->labyrinthe[i][j] = nouvelleRegion;
                }
            }
        }

        regions--;
    }
}

void removeWalls(maze_t *maze){
    int hauteur = maze->hauteur;
    int largeur = maze->largeur;

    int region = maze->labyrinthe[1][1];

    int maxCandidats = hauteur * largeur;
    int *candidatsX = malloc((size_t) maxCandidats * sizeof *candidatsX);
    int *candidatsY = malloc((size_t) maxCandidats * sizeof *candidatsY);
    if (candidatsX == NULL || candidatsY == NULL){
        free(candidatsX);
        free(candidatsY);
        return;
    }

    int nbCandidats = 0;
    for (int i = 1; i < hauteur - 1; i++){
        for (int j = 1; j < largeur - 1; j++){
            if (maze->labyrinthe[i][j] != 0){
                continue;
            }
            int vertical = (i % 2 == 0 && j % 2 == 1 &&
                            maze->labyrinthe[i - 1][j] > 0 && maze->labyrinthe[i + 1][j] > 0);
            int horizontal = (i % 2 == 1 && j % 2 == 0 &&
                              maze->labyrinthe[i][j - 1] > 0 && maze->labyrinthe[i][j + 1] > 0);
            if (vertical || horizontal){
                candidatsX[nbCandidats] = i;
                candidatsY[nbCandidats] = j;
                nbCandidats++;
            }
        }
    }

    int nbCellules = ((hauteur - 1) / 2) * ((largeur - 1) / 2);
    int aRetirer = nbCellules / 5;
    if (aRetirer < 1){
        aRetirer = 1;
    }
    if (aRetirer > nbCandidats){
        aRetirer = nbCandidats;
    }

    for (int k = 0; k < aRetirer; k++){
        int r = k + rand() % (nbCandidats - k);
        int tmpX = candidatsX[k];
        int tmpY = candidatsY[k];
        candidatsX[k] = candidatsX[r];
        candidatsY[k] = candidatsY[r];
        candidatsX[r] = tmpX;
        candidatsY[r] = tmpY;

        maze->labyrinthe[candidatsX[k]][candidatsY[k]] = region;
    }

    free(candidatsX);
    free(candidatsY);
}

void normalizeMaze(maze_t *maze){
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            switch (maze->labyrinthe[i][j])
            {
            case 0:  maze->labyrinthe[i][j] = '#'; break;
            case -1: maze->labyrinthe[i][j] = 'o'; break;
            case -2: maze->labyrinthe[i][j] = '-'; break;
            case -3: maze->labyrinthe[i][j] = '@'; break;
            case -4: maze->labyrinthe[i][j] = '?'; break;
            case -5: maze->labyrinthe[i][j] = 'X'; break;
            default: maze->labyrinthe[i][j] = ' '; break;
            }
        }
    }
}

void generateItems(maze_t *maze){
    int freecase = 0;
    for (int i = 1; i < maze->hauteur-1; i++){
        for (int j = 1; j <maze->largeur-1; j++){
            freecase++;
        }
    }
    int x1, y1, x2, y2, x3, y3;
    int libres = 0;
    for (int i = 1; i < maze->hauteur-1; i++){
        for (int j = 1; j < maze->largeur-1; j++){
            if (maze->labyrinthe[i][j] >= 1){
                libres++;
            }
        }
    }
    if (libres == 0){
        return;
    }
    do
    {
        x3 = (rand()%maze->hauteur-1) + 1;
        y3 = (rand()%maze->largeur-1) + 1;
    } while (maze->labyrinthe[x3][y3] < 1);
    maze->labyrinthe[x3][y3] = -3;
    libres--;
    for (int k = 0; k < freecase/20+1 && libres > 0; k++){
        do
        {
            x1 = (rand()%maze->hauteur-1) + 1;
            y1 = (rand()%maze->largeur-1) + 1;
        } while (maze->labyrinthe[x1][y1] < 1);
        maze->labyrinthe[x1][y1] = -5;
        libres--;
        if (libres == 0){
            break;
        }
        do
        {
            x2 = (rand()%maze->hauteur-1) + 1;
            y2 = (rand()%maze->largeur-1) + 1;
        } while (maze->labyrinthe[x2][y2] < 1);
        maze->labyrinthe[x2][y2] = -4;
        libres--;
    }
}