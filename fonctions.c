#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"
#include <time.h>

void generateMaze(int maze[HAUTEUR][LARGEUR]){
    int compteur = 1;
    for (int i = 0; i < HAUTEUR; i++){
        for (int j = 0; j < LARGEUR; j++){
            if (j%2 == 0 && (i != 0 || i != HAUTEUR-1) && (i%2 != 0)){
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
    int fini = 1;
    int x, y;
    int value[4];
    while (fini != 0){
        fini = 0;
        do{
            x = (rand()%(HAUTEUR-2))+1;
            y = (rand()%(LARGEUR-2))+1;
        }while (maze[x][y] != 0 && maze[x-1][y] == maze[x+1][y] && maze[x][y-1] == maze[x][y+1]);

        value[0] = maze[x-1][y];
        value[1] = maze[x+1][y];
        value[2] = maze[x][y-1];
        value[3] = maze[x][y+1];
        
        if (maze[x-1][y] != 0){
            maze[x][y] = maze[x-1][y];
        }
        else if(maze[x+1][y] != 0){
            maze[x][y] = maze[x+1][y];
        }
        else if(maze[x][y-1] != 0){
            maze[x][y] = maze[x][y-1];
        }
        else if(maze[x][y+1] != 0){
            maze[x][y] = maze[x][y+1];
        }
        int rempli = maze[1][1];
        for (int i = 0; i < HAUTEUR; i++){
            for (int j = 0; j < LARGEUR; j++){
                if ((maze[i][j] != maze[x][y]) && (maze[i][j] == value[0] || maze[i][j] == value[1] || maze[i][j] == value[2] || maze[i][j] == value[3])){
                    maze[i][j] = maze[x][y];
                }
                if (j%2 == 1 && (i != 0 || i != HAUTEUR-1) && (i%2 != 0)){
                    if (maze[i][j] != rempli){
                        fini++;
                    }
                }
            }
        }
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
                printf("%d", maze[i][j]);
                break;
            }
        }
        printf("\n");
    }
}
