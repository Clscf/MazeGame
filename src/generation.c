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