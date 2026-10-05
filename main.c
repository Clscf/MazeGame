#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fonctions.h"

int main(){
    srand(time(NULL));
    int maze[HAUTEUR][LARGEUR];
    generateMaze(maze);
    mazePath(maze);
    displayMaze(maze);
    return 0;
}