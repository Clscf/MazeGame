#include <stdio.h>
#include <stdlib.h>

#include "fonctions.h"

int main(){
    int maze[HAUTEUR][LARGEUR];
    generateMaze(maze);
    mazePath(maze);
    displayMaze(maze);
    return 0;
}