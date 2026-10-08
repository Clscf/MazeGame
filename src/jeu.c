#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>

void game(maze_t *maze){
    char move = '\0';
    int win = -1;
    displayMaze(maze);
    do {
        scanf("%c", &move);
        win = movement(move, maze);
    } while (win != 1);
    displayMaze(maze);
}

int movement(char move, maze_t *maze){
    displayMaze(maze);
    int player[2] = {0, 1};
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            if (maze->labyrinthe[i][j] == 'o'){
                player[0] = i;
                player[1] = j; 
            }
        }
    }
    switch (move)
    {
    case 'z':
        if (player[0]-1 >= 0){
            if (maze->labyrinthe[player[0]-1][player[1]] == '#' || player[0]-1 < 0){
                return -1;
            }
            maze->labyrinthe[player[0]-1][player[1]] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
        }
        break;
    case 'q':
        if (player[1]-1 >= 0){
            if (maze->labyrinthe[player[0]][player[1]-1] == '#'){
                return -1;
            }
            maze->labyrinthe[player[0]][player[1]-1] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
        }
        break;
    case 'd':
        if (player[1]+1 < maze->largeur){
            if (maze->labyrinthe[player[0]][player[1]+1] == '#'){
                return -1;
            }
            maze->labyrinthe[player[0]][player[1]+1] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
        }
        break;
    case 's':
        if (player[0]+1 < maze->hauteur){
            if (maze->labyrinthe[player[0]+1][player[1]] == '#'){
                return -1;
            }
            else if (maze->labyrinthe[player[0]+1][player[1]] == '-'){
                maze->labyrinthe[player[0]+1][player[1]] = 'o';
                maze->labyrinthe[player[0]][player[1]] = ' ';
                return 1;
            }
            maze->labyrinthe[player[0]+1][player[1]] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
            }
        break;
    default:
        return -1;
    }
    return 0;
}