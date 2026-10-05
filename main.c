#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fonctions.h"

int main(){
    srand(time(NULL));
    createMaze();
    return 0;
}