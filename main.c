#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fonctions.h"

int main(){
    srand(time(NULL));
    int choix;
    maze_t maze;
    do{
        choix = menu();
        switch (choix)
        {
        case 1:
            maze = createMaze();
            displayMaze(maze);
            break;
        case 2:
            maze = loadMaze();
            displayMaze(maze);
            break;
        case 3:
            game(maze);
            break;
        case 4:
            exit(-1);
            break;
        default:
            break;
        }
    } while (choix != 4);
    return 0;
}