#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"
#include <time.h>

void generateMaze(int maze[HAUTEUR][LARGEUR]){
    int compteur = 1;
    for (int i = 0; i < HAUTEUR; i++){
        for (int j = 0; j < LARGEUR; j++){
            if (j%2 == 0 && (i != 0 && i != HAUTEUR-1) && (i%2 != 0)){
                maze[i][j] = 0;
            }
            else if (i == 0 || i == HAUTEUR-1 || i%2 == 0){
                maze[i][j] = 0;
            }
            else{
                maze[i][j] = compteur;
                compteur++;
            }
        }
    }
    maze[0][1] = -1;
    maze[HAUTEUR - 1][LARGEUR - 2] = -2;
}

void mazePath(int maze[HAUTEUR][LARGEUR]){
    int regions = 0;

    for (int i = 1; i < HAUTEUR - 1; i += 2){
        for (int j = 1; j < LARGEUR - 1; j += 2){
            if (maze[i][j] > 0){
                regions++;
            }
        }
    }

    while (regions > 1){
        int mursValides = 0;

        for (int i = 1; i < HAUTEUR - 1; i++){
            for (int j = 1; j < LARGEUR - 1; j++){
                int vertical = 0;
                int horizontal = 0;
                if (maze[i][j] == 0 &&
                    maze[i - 1][j] > 0 && maze[i + 1][j] > 0 &&
                    maze[i - 1][j] != maze[i + 1][j]){
                        vertical = 1;
                    }
                if (maze[i][j] == 0 &&
                    maze[i][j - 1] > 0 && maze[i][j + 1] > 0 &&
                    maze[i][j - 1] != maze[i][j + 1]){
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

        for (int i = 1; i < HAUTEUR - 1 && x == -1; i++){
            for (int j = 1; j < LARGEUR - 1; j++){
                int vertical = 0;
                int horizontal = 0;
                if (maze[i][j] == 0 &&
                    maze[i - 1][j] > 0 && maze[i + 1][j] > 0 &&
                    maze[i - 1][j] != maze[i + 1][j]){
                        vertical = 1;
                    }
                if (maze[i][j] == 0 &&
                    maze[i][j - 1] > 0 && maze[i][j + 1] > 0 &&
                    maze[i][j - 1] != maze[i][j + 1]){
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

        if (maze[x - 1][y] > 0){
            nouvelleRegion = maze[x - 1][y];
            ancienneRegion = maze[x + 1][y];
        }
        else{
            nouvelleRegion = maze[x][y - 1];
            ancienneRegion = maze[x][y + 1];
        }

        maze[x][y] = nouvelleRegion;

        for (int i = 1; i < HAUTEUR - 1; i += 2){
            for (int j = 1; j < LARGEUR - 1; j += 2){
                if (maze[i][j] == ancienneRegion){
                    maze[i][j] = nouvelleRegion;
                }
            }
        }

        regions--;
    }
}

void displayMaze(int maze[HAUTEUR][LARGEUR]){
    for (int i = 0; i < HAUTEUR; i++){
        for (int j = 0; j < LARGEUR; j++){
            switch (maze[i][j])
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
            default:
                //printf("%d", maze[i][j]);
                printf(" ");
                break;
            }
        }
        printf("\n");
    }
}
