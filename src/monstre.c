#include "monstre.h"
#include <stdio.h>
#include <stdlib.h>

static const int DX[4] = {-1, 1, 0, 0};
static const int DY[4] = {0, 0, -1, 1};


static int isWall(int valeur){
    return valeur == '#';
}

static int isTreasure(int valeur){
    return valeur == '?';
}

static int isInside(const maze_t *maze, int x, int y){
    return x >= 1 && x < maze->hauteur - 1 && y >= 1 && y < maze->largeur - 1;
}

static int distance(int x1, int y1, int x2, int y2){
    return abs(x1 - x2) + abs(y1 - y2);
}

static int rayon(const monster_t *monstre){
    if (monstre->garde){
        int r = monstre->etendue / 2;
        return r < 1 ? 1 : r;
    }
    return monstre->etendue;
}

static int canGo(const monster_t *monstre, const maze_t *maze,
                 int x, int y, int traverseMurs){
    if (!isInside(maze, x, y)){
        return 0;
    }
    if (!traverseMurs && isWall(maze->labyrinthe[x][y])){
        return 0;
    }
    return distance(x, y, monstre->centreX, monstre->centreY) <= rayon(monstre);
}

static void randomStep(monster_t *monstre, const maze_t *maze, int traverseMurs){
    int possibles[4];
    int n = 0;
    for (int d = 0; d < 4; d++){
        if (canGo(monstre, maze, monstre->x + DX[d], monstre->y + DY[d], traverseMurs)){
            possibles[n++] = d;
        }
    }
    if (n == 0){
        return;
    }
    int d = possibles[rand() % n];
    monstre->x += DX[d];
    monstre->y += DY[d];
}

static void moveGhost(monster_t *monstre, const maze_t *maze, int joueurX, int joueurY){
    (void) joueurX;
    (void) joueurY;
    randomStep(monstre, maze, 1);
}

static void moveOgre(monster_t *monstre, const maze_t *maze, int joueurX, int joueurY){
    (void) joueurX;
    (void) joueurY;

    if (monstre->garde && !isTreasure(maze->labyrinthe[monstre->centreX][monstre->centreY])){
        monstre->garde = 0;
    }

    int moved = 0;
    if (!monstre->garde){
        for (int d = 0; d < 4 && !moved; d++){
            int nx = monstre->x + DX[d];
            int ny = monstre->y + DY[d];
            if (canGo(monstre, maze, nx, ny, 0) && isTreasure(maze->labyrinthe[nx][ny])){
                monstre->x = nx;
                monstre->y = ny;
                moved = 1;
            }
        }
    }
    if (!moved){
        randomStep(monstre, maze, 0);
    }

    if (isTreasure(maze->labyrinthe[monstre->x][monstre->y])){
        monstre->centreX = monstre->x;
        monstre->centreY = monstre->y;
        monstre->garde = 1;
    }
}

static void moveHunter(monster_t *monstre, const maze_t *maze, int joueurX, int joueurY){
    if (distance(joueurX, joueurY, monstre->centreX, monstre->centreY) <= monstre->etendue){
        int meilleur = -1;
        int meilleureDistance = distance(monstre->x, monstre->y, joueurX, joueurY);
        for (int d = 0; d < 4; d++){
            int nx = monstre->x + DX[d];
            int ny = monstre->y + DY[d];
            int dist = distance(nx, ny, joueurX, joueurY);
            if (canGo(monstre, maze, nx, ny, 0) && dist < meilleureDistance){
                meilleur = d;
                meilleureDistance = dist;
            }
        }
        if (meilleur >= 0){
            monstre->x += DX[meilleur];
            monstre->y += DY[meilleur];
            return;
        }
    }
    randomStep(monstre, maze, 0);
}

static const monster_type_t TYPES[] = {
    {"Fantome",  'F', 30, moveGhost,  "traverse les murs"},
    {"Chasseur", 'C', 40, moveHunter, "poursuit le joueur"},
    {"Ogre",     'G', 50, moveOgre,   "garde les tresors"},
};
#define NB_TYPES ((int) (sizeof TYPES / sizeof TYPES[0]))

static const monster_type_t *typeFromSymbol(int symbole){
    for (int t = 0; t < NB_TYPES; t++){
        if (TYPES[t].symbole == symbole){
            return &TYPES[t];
        }
    }
    return NULL;
}

static void initMonster(monster_t *monstre, const monster_type_t *type, int x, int y){
    monstre->type = type;
    monstre->x = monstre->departX = monstre->centreX = x;
    monstre->y = monstre->departY = monstre->centreY = y;
    monstre->etendue = type->penalite / PENALTY_PER_RANGE;
    monstre->garde = 0;
}

void generateMonsters(maze_t *maze){
    maze->monstres = NULL;
    maze->nbMonstres = 0;

    int nbCellules = ((maze->hauteur - 1) / 2) * ((maze->largeur - 1) / 2);
    int voulu = nbCellules / 15 + NB_TYPES;

    int taille = maze->hauteur * maze->largeur;
    int *casesX = malloc((size_t) taille * sizeof *casesX);
    int *casesY = malloc((size_t) taille * sizeof *casesY);
    if (casesX == NULL || casesY == NULL){
        free(casesX);
        free(casesY);
        return;
    }

    int nbCases = 0;
    for (int distMin = 4; distMin >= 1 && nbCases == 0; distMin -= 3){
        for (int i = 1; i < maze->hauteur - 1; i++){
            for (int j = 1; j < maze->largeur - 1; j++){
                if (maze->labyrinthe[i][j] == ' ' && distance(i, j, 1, 1) >= distMin){
                    casesX[nbCases] = i;
                    casesY[nbCases] = j;
                    nbCases++;
                }
            }
        }
    }

    if (voulu > nbCases){
        voulu = nbCases;
    }
    if (voulu > 0){
        maze->monstres = malloc((size_t) voulu * sizeof *maze->monstres);
    }
    if (maze->monstres != NULL){
        for (int k = 0; k < voulu; k++){
            int r = k + rand() % (nbCases - k);
            int tmpX = casesX[k];
            int tmpY = casesY[k];
            casesX[k] = casesX[r];
            casesY[k] = casesY[r];
            casesX[r] = tmpX;
            casesY[r] = tmpY;
            initMonster(&maze->monstres[k], &TYPES[k % NB_TYPES], casesX[k], casesY[k]);
        }
        maze->nbMonstres = voulu;
    }

    free(casesX);
    free(casesY);
}

void loadMonsters(maze_t *maze){
    maze->monstres = NULL;
    maze->nbMonstres = 0;

    int nb = 0;
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            if (typeFromSymbol(maze->labyrinthe[i][j]) != NULL){
                nb++;
            }
        }
    }
    if (nb == 0){
        return;
    }

    maze->monstres = malloc((size_t) nb * sizeof *maze->monstres);
    if (maze->monstres == NULL){
        return;
    }
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            const monster_type_t *type = typeFromSymbol(maze->labyrinthe[i][j]);
            if (type != NULL){
                initMonster(&maze->monstres[maze->nbMonstres], type, i, j);
                maze->nbMonstres++;
                maze->labyrinthe[i][j] = ' ';
            }
        }
    }
}

void resetMonsters(maze_t *maze){
    for (int k = 0; k < maze->nbMonstres; k++){
        initMonster(&maze->monstres[k], maze->monstres[k].type,
                    maze->monstres[k].departX, maze->monstres[k].departY);
    }
}

void moveMonsters(maze_t *maze, int joueurX, int joueurY){
    for (int k = 0; k < maze->nbMonstres; k++){
        monster_t *monstre = &maze->monstres[k];
        monstre->type->deplacer(monstre, maze, joueurX, joueurY);
    }
}

int monsterCollisions(maze_t *maze, int joueurX, int joueurY){
    int total = 0;
    for (int k = 0; k < maze->nbMonstres; k++){
        monster_t *monstre = &maze->monstres[k];
        if (monstre->x == joueurX && monstre->y == joueurY){
            printf("Aie ! Un %s vous attaque : -%d points\n",
                   monstre->type->nom, monstre->type->penalite);
            total += monstre->type->penalite;
            initMonster(monstre, monstre->type, monstre->departX, monstre->departY);
        }
    }
    return total;
}

const monster_t *monsterAt(const maze_t *maze, int x, int y){
    for (int k = 0; k < maze->nbMonstres; k++){
        if (maze->monstres[k].x == x && maze->monstres[k].y == y){
            return &maze->monstres[k];
        }
    }
    return NULL;
}

void showMonsterLegend(const maze_t *maze){
    if (maze->nbMonstres == 0){
        return;
    }
    printf("Monstres :");
    for (int t = 0; t < NB_TYPES; t++){
        printf("  %c = %s (-%d, %s, portee %d)", TYPES[t].symbole, TYPES[t].nom,
               TYPES[t].penalite, TYPES[t].description,
               TYPES[t].penalite / PENALTY_PER_RANGE);
    }
    printf("\n");
}
