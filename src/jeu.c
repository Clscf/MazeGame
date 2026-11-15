#include "jeu.h"
#include "monstre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void saveScore(const maze_t *maze, const stats_t *stats){
    score_entry_t scores[SCORE_COUNT];
    int scoreCount = 0;
    char chemin[150];
    FILE *file;

    snprintf(chemin, sizeof chemin, "scores/%s.score", maze->nom);
    file = fopen(chemin, "r");
    if (file != NULL){
        while (scoreCount < SCORE_COUNT &&
               fscanf(file, "%49s %d", scores[scoreCount].pseudo,
                      &scores[scoreCount].score) == 2){
            scoreCount++;
        }
        fclose(file);
    }

    if (scoreCount == SCORE_COUNT && stats->score <= scores[scoreCount - 1].score){
        return;
    }

    score_entry_t newScore;
    printf("Nouveau score ! Entrez votre pseudo : ");
    if (scanf("%49s", newScore.pseudo) != 1){
        return;
    }
    newScore.score = stats->score;

    int position = 0;
    while (position < scoreCount && scores[position].score >= newScore.score){
        position++;
    }

    int newCount = scoreCount < SCORE_COUNT ? scoreCount + 1 : SCORE_COUNT;
    for (int i = newCount - 1; i > position; i--){
        scores[i] = scores[i - 1];
    }
    scores[position] = newScore;

    file = fopen(chemin, "w");
    if (file == NULL){
        printf("Impossible d'enregistrer le score\n");
        return;
    }
    for (int i = 0; i < newCount; i++){
        fprintf(file, "%s %d\n", scores[i].pseudo, scores[i].score);
    }
    fclose(file);
}

static void findPlayer(const maze_t *maze, int *x, int *y){
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            if (maze->labyrinthe[i][j] == 'o' || maze->labyrinthe[i][j] == -1){
                *x = i;
                *y = j;
                return;
            }
        }
    }
}

void game(maze_t *maze, stats_t *stats){
    char move = '\0';
    int win = -1;
    resetMonsters(maze);
    displayMaze(maze);
    printf("Score: %d\n", stats->score);
    do {
        scanf(" %c", &move);
        win = movement(move, maze, stats);
        if (win == 0 && maze->nbMonstres > 0){
            int joueurX = 0;
            int joueurY = 1;
            findPlayer(maze, &joueurX, &joueurY);
            stats->score -= monsterCollisions(maze, joueurX, joueurY);
            moveMonsters(maze, joueurX, joueurY);
            stats->score -= monsterCollisions(maze, joueurX, joueurY);
        }
        displayMaze(maze);
        printf("Score: %d\n", stats->score);
    } while (win != 1);
    saveScore(maze, stats);
}

int movement(char move, maze_t *maze, stats_t *stats){
    int player[2] = {0, 1};
    for (int i = 0; i < maze->hauteur; i++){
        for (int j = 0; j < maze->largeur; j++){
            if (maze->labyrinthe[i][j] == 'o' ||
                maze->labyrinthe[i][j] == -1){
                player[0] = i;
                player[1] = j; 
            }
        }
    }
    switch (move)
    {
    case 'z':
        if (player[0]-1 >= 0){
            stats->score = stats->score - MOVE;
            switch (maze->labyrinthe[player[0]-1][player[1]])
            {
            case 0:
            case '#':
                return -1;
            case -3:
            case '@':
                stats->key = 1;
                break;
            case -5:
            case 'X':
                stats->score = stats->score - TRAP;
                break;
            case -4:
            case '?':
                stats->score = stats->score + TREASURE;
                break;
            default:
                break;
            }
            maze->labyrinthe[player[0]-1][player[1]] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
        }
        break;
    case 'q':
        if (player[1]-1 >= 0){
            stats->score = stats->score - MOVE;
            switch (maze->labyrinthe[player[0]][player[1]-1])
            {
            case 0:
            case '#':
                return -1;
            case -3:
            case '@':
                stats->key = 1;
                break;
            case -5:
            case 'X':
                stats->score = stats->score - TRAP;
                break;
            case -4:
            case '?':
                stats->score = stats->score + TREASURE;
                break;
            default:
                break;
            }
            maze->labyrinthe[player[0]][player[1]-1] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
        }
        break;
    case 'd':
        if (player[1]+1 < maze->largeur){
            stats->score = stats->score - MOVE;
            switch (maze->labyrinthe[player[0]][player[1]+1])
            {
            case 0:
            case '#':
                return -1;
            case -3:
            case '@':
                stats->key = 1;
                break;
            case -5:
            case 'X':
                stats->score = stats->score - TRAP;
                break;
            case -4:
            case '?':
                stats->score = stats->score + TREASURE;
                break;
            default:
                break;
            }
            maze->labyrinthe[player[0]][player[1]+1] = 'o';
            maze->labyrinthe[player[0]][player[1]] = ' ';
        }
        break;
    case 's':
        if (player[0]+1 < maze->hauteur){
            stats->score = stats->score - MOVE;
            switch (maze->labyrinthe[player[0]+1][player[1]])
            {
            case 0:
            case '#':
                return -1;
            case -3:
            case '@':
                stats->key = 1;
                break;
            case -5:
            case 'X':
                stats->score = stats->score - TRAP;
                break;
            case -4:
            case '?':
                stats->score = stats->score + TREASURE;
                break;
            case -2:
            case '-':
                if (stats->key == 1){
                    maze->labyrinthe[player[0]+1][player[1]] = 'o';
                    maze->labyrinthe[player[0]][player[1]] = ' ';
                    return 1;
                }
                return -1;
            default:
                break;
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
