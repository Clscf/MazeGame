/*
 * Tests unitaires du MazeGame (framework MinUnit).
 *
 * Lancement : make test
 *
 * Les tests s'executent dans un dossier temporaire (contenant Labyrinthes/
 * et scores/) pour ne jamais toucher aux fichiers du projet.
 * Les saisies utilisateur sont simulees en redirigeant stdin vers un fichier.
 */
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE /* mkdtemp sous macOS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "minunit.h"
#include "maze.h"
#include "generation.h"
#include "fichier.h"
#include "jeu.h"
#include "affichage.h"
#include "monstre.h"

int tests_run = 0;

/* ====================================================================== */
/*                               Outils                                    */
/* ====================================================================== */

/* Simule une saisie clavier : le texte devient le contenu de stdin */
static void feedInput(const char *texte){
    FILE *f = fopen("stdin.txt", "w");
    fputs(texte, f);
    fclose(f);
    if (freopen("stdin.txt", "r", stdin) == NULL){
        perror("freopen");
        exit(EXIT_FAILURE);
    }
}

static void writeFile(const char *chemin, const char *contenu){
    FILE *f = fopen(chemin, "w");
    fputs(contenu, f);
    fclose(f);
}

static int fileExists(const char *chemin){
    FILE *f = fopen(chemin, "r");
    if (f == NULL){
        return 0;
    }
    fclose(f);
    return 1;
}

/* Lit un fichier de scores ; renvoie le nombre de lignes lues */
static int readScores(const char *chemin, char pseudos[][PSEUDO_LENGTH], int scores[], int max){
    FILE *f = fopen(chemin, "r");
    if (f == NULL){
        return 0;
    }
    int n = 0;
    while (n < max && fscanf(f, "%49s %d", pseudos[n], &scores[n]) == 2){
        n++;
    }
    fclose(f);
    return n;
}

/* Construit un labyrinthe a partir de lignes de texte (format .cfg) */
static maze_t *makeMaze(const char *nom, const char *lignes[], int hauteur){
    maze_t *m = calloc(1, sizeof *m);
    m->hauteur = hauteur;
    m->largeur = (int) strlen(lignes[0]);
    m->labyrinthe = malloc((size_t) hauteur * sizeof *m->labyrinthe);
    for (int i = 0; i < hauteur; i++){
        m->labyrinthe[i] = malloc((size_t) m->largeur * sizeof **m->labyrinthe);
        for (int j = 0; j < m->largeur; j++){
            m->labyrinthe[i][j] = lignes[i][j];
        }
    }
    m->nom = malloc(strlen(nom) + 1);
    strcpy(m->nom, nom);
    return m;
}

/* Labyrinthe vierge pour tester les fonctions de generation */
static maze_t *allocMaze(int hauteur, int largeur){
    maze_t *m = calloc(1, sizeof *m);
    m->hauteur = hauteur;
    m->largeur = largeur;
    m->labyrinthe = malloc((size_t) hauteur * sizeof *m->labyrinthe);
    for (int i = 0; i < hauteur; i++){
        m->labyrinthe[i] = calloc((size_t) largeur, sizeof **m->labyrinthe);
    }
    m->nom = malloc(5);
    strcpy(m->nom, "test");
    return m;
}

/* Meme chaine de generation que createMaze, sans saisie */
static maze_t *generatedMaze(int hauteur, int largeur, int difficile){
    maze_t *m = allocMaze(hauteur, largeur);
    generateMaze(m);
    mazePath(m);
    if (difficile){
        removeWalls(m);
    }
    generateItems(m);
    normalizeMaze(m);
    if (difficile){
        generateMonsters(m);
    }
    return m;
}

/* Nombre de murs internes ouverts (cases entre deux cellules) - grille entiere */
static int countOpenings(const maze_t *m){
    int n = 0;
    for (int i = 1; i < m->hauteur - 1; i++){
        for (int j = 1; j < m->largeur - 1; j++){
            if ((i + j) % 2 == 1 && m->labyrinthe[i][j] > 0){
                n++;
            }
        }
    }
    return n;
}

/* Toutes les cases libres internes sont-elles atteignables depuis (1,1) ?
 * libre(v) indique si une valeur de case est un passage */
static int isConnected(const maze_t *m, int (*libre)(int)){
    int h = m->hauteur, w = m->largeur;
    char *vu = calloc((size_t) (h * w), 1);
    int *file = malloc((size_t) (h * w) * sizeof *file);
    int debut = 0, fin = 0, atteintes = 0, total = 0;

    for (int i = 1; i < h - 1; i++){
        for (int j = 1; j < w - 1; j++){
            if (libre(m->labyrinthe[i][j])){
                total++;
            }
        }
    }
    file[fin++] = 1 * w + 1;
    vu[1 * w + 1] = 1;
    while (debut < fin){
        int c = file[debut++];
        int x = c / w, y = c % w;
        atteintes++;
        const int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};
        for (int d = 0; d < 4; d++){
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 1 && nx < h - 1 && ny >= 1 && ny < w - 1 &&
                !vu[nx * w + ny] && libre(m->labyrinthe[nx][ny])){
                vu[nx * w + ny] = 1;
                file[fin++] = nx * w + ny;
            }
        }
    }
    free(vu);
    free(file);
    return atteintes == total;
}

static int libreEntier(int v){ return v != 0; }      /* grille de generation */
static int libreCaractere(int v){ return v != '#'; } /* grille normalisee */

static int countValue(const maze_t *m, int valeur){
    int n = 0;
    for (int i = 0; i < m->hauteur; i++){
        for (int j = 0; j < m->largeur; j++){
            if (m->labyrinthe[i][j] == valeur){
                n++;
            }
        }
    }
    return n;
}

static int manhattan(int x1, int y1, int x2, int y2){
    return abs(x1 - x2) + abs(y1 - y2);
}

/* ====================================================================== */
/*                     1. Verification des saisies                         */
/* ====================================================================== */

static char *test_menu_rejette_saisies_invalides(void){
    feedInput("0\n9\nabc\n-3\n3\n");
    mu_assert("menu : doit ignorer 0, 9, 'abc', -3 et renvoyer 3", menu() == 3);
    return 0;
}

static char *test_menu_accepte_bornes(void){
    feedInput("1\n");
    mu_assert("menu : 1 doit etre accepte", menu() == 1);
    feedInput("5\n");
    mu_assert("menu : 5 doit etre accepte", menu() == 5);
    return 0;
}

/* Nom deja existant, dimensions paires / trop petites / non numeriques,
 * difficulte invalide, confirmation invalide : tout doit etre redemande */
static char *test_createMaze_valide_les_saisies(void){
    writeFile("Labyrinthes/existe.cfg", "###\n#o#\n###\n");
    feedInput("existe\n"        /* nom deja pris -> redemande */
              "nouveau\n"
              "2\n4\nabc\n7\n"  /* hauteur : 2 (<3), 4 (pair), abc -> 7 */
              "1\n9\n"          /* largeur : 1 (<3) -> 9 */
              "0\n3\n1\n"       /* difficulte : 0, 3 invalides -> 1 */
              "x\ny\n");        /* confirmation : x invalide -> y */
    maze_t *m = createMaze();
    mu_assert("createMaze : doit renvoyer un labyrinthe", m != NULL);
    mu_assert("createMaze : hauteur attendue 7", m->hauteur == 7);
    mu_assert("createMaze : largeur attendue 9", m->largeur == 9);
    mu_assert("createMaze : nom attendu 'nouveau'", strcmp(m->nom, "nouveau") == 0);
    mu_assert("createMaze : facile => aucun monstre", m->nbMonstres == 0);
    mu_assert("createMaze : fichier .cfg non cree", fileExists("Labyrinthes/nouveau.cfg"));
    freeMaze(m);
    return 0;
}

static char *test_createMaze_refus_enregistrement(void){
    feedInput("refus\n7\n7\n1\nn\n");
    maze_t *m = createMaze();
    mu_assert("createMaze : 'n' doit renvoyer NULL", m == NULL);
    mu_assert("createMaze : 'n' ne doit pas creer de fichier", !fileExists("Labyrinthes/refus.cfg"));
    return 0;
}

static char *test_createMaze_petit_labyrinthe(void){
    /* 3x3 : une seule case libre, la generation ne doit pas boucler */
    feedInput("mini\n3\n3\n2\ny\n");
    maze_t *m = createMaze();
    mu_assert("createMaze 3x3 : doit renvoyer un labyrinthe", m != NULL);
    mu_assert("createMaze 3x3 : la cle doit etre presente", countValue(m, '@') == 1);
    freeMaze(m);
    return 0;
}

/* ====================================================================== */
/*                       2. Gestion des fichiers                           */
/* ====================================================================== */

static char *test_fichier_aller_retour_difficile(void){
    feedInput("dur\n15\n21\n2\ny\n");
    maze_t *cree = createMaze();
    mu_assert("aller-retour : creation echouee", cree != NULL);
    mu_assert("aller-retour : un labyrinthe difficile doit avoir des monstres", cree->nbMonstres > 0);

    /* Premier nom inexistant -> redemande, puis le bon nom */
    feedInput("absent\ndur\n");
    maze_t *charge = loadMaze();
    mu_assert("aller-retour : chargement echoue", charge != NULL);
    mu_assert("aller-retour : hauteur differente", charge->hauteur == cree->hauteur);
    mu_assert("aller-retour : largeur differente", charge->largeur == cree->largeur);
    mu_assert("aller-retour : nom different", strcmp(charge->nom, "dur") == 0);
    for (int i = 0; i < cree->hauteur; i++){
        for (int j = 0; j < cree->largeur; j++){
            mu_assert("aller-retour : grille differente",
                      charge->labyrinthe[i][j] == cree->labyrinthe[i][j]);
        }
    }
    /* La difficulte est conservee via les monstres ecrits dans le fichier */
    mu_assert("aller-retour : nombre de monstres different", charge->nbMonstres == cree->nbMonstres);
    for (int k = 0; k < charge->nbMonstres; k++){
        const monster_t *a = &charge->monstres[k];
        const monster_t *b = monsterAt(cree, a->x, a->y);
        mu_assert("aller-retour : monstre absent apres rechargement", b != NULL);
        mu_assert("aller-retour : sorte de monstre differente", a->type == b->type);
    }
    freeMaze(cree);
    freeMaze(charge);
    return 0;
}

static char *test_loadMaze_fin_de_ligne_windows(void){
    writeFile("Labyrinthes/crlf.cfg", "#o###\r\n#  ?#\r\n###-#\r\n");
    feedInput("crlf\n");
    maze_t *m = loadMaze();
    mu_assert("CRLF : hauteur attendue 3", m->hauteur == 3);
    mu_assert("CRLF : largeur attendue 5 (sans \\r)", m->largeur == 5);
    mu_assert("CRLF : contenu mal lu", m->labyrinthe[1][3] == '?' && m->labyrinthe[2][3] == '-');
    mu_assert("CRLF : pas de monstre => niveau facile", m->nbMonstres == 0);
    freeMaze(m);
    return 0;
}

static char *test_loadMaze_recupere_monstres(void){
    writeFile("Labyrinthes/monstres.cfg",
              "#o#######\n"
              "#F  C  G#\n"
              "#######-#\n");
    feedInput("monstres\n");
    maze_t *m = loadMaze();
    mu_assert("monstres : 3 monstres attendus", m->nbMonstres == 3);
    mu_assert("monstres : le symbole doit etre remplace par une case libre",
              m->labyrinthe[1][1] == ' ' && m->labyrinthe[1][4] == ' ' && m->labyrinthe[1][7] == ' ');
    mu_assert("monstres : fantome mal place", monsterAt(m, 1, 1)->type->symbole == 'F');
    mu_assert("monstres : chasseur mal place", monsterAt(m, 1, 4)->type->symbole == 'C');
    mu_assert("monstres : ogre mal place", monsterAt(m, 1, 7)->type->symbole == 'G');
    freeMaze(m);
    return 0;
}

static char *test_replaceMaze_ecrit_les_monstres(void){
    const char *lignes[] = {"#o#####", "# F ?C#", "#####-#"};
    maze_t *m = makeMaze("rep", lignes, 3);
    loadMonsters(m);
    m->monstres[0].x = 1; m->monstres[0].y = 3; /* le fantome a bouge */
    char **texte = replaceMaze(m);
    mu_assert("replaceMaze : ligne 0 incorrecte", strcmp(texte[0], "#o#####") == 0);
    mu_assert("replaceMaze : monstres a leur position de depart", strcmp(texte[1], "# F ?C#") == 0);
    mu_assert("replaceMaze : ligne 2 incorrecte", strcmp(texte[2], "#####-#") == 0);
    fileWrite(m, texte); /* libere texte */
    mu_assert("fileWrite : fichier non cree", fileExists("Labyrinthes/rep.cfg"));
    freeMaze(m);
    return 0;
}

static char *test_dossier_labyrinthes_absent(void){
    mkdir("vide", 0700);
    if (chdir("vide") != 0){
        return "impossible d'entrer dans le dossier 'vide'";
    }
    feedInput("x\n");
    maze_t *m = createMaze();
    const char *lignes[] = {"#o#", "# #", "#-#"};
    maze_t *t = makeMaze("t", lignes, 3);
    fileWrite(t, replaceMaze(t)); /* doit afficher une erreur, pas planter */
    int ecrit = fileExists("Labyrinthes/t.cfg");
    freeMaze(t);
    if (chdir("..") != 0){
        return "impossible de revenir au dossier de test";
    }
    mu_assert("dossier absent : createMaze doit renvoyer NULL", m == NULL);
    mu_assert("dossier absent : fileWrite ne doit rien ecrire", !ecrit);
    return 0;
}

static char *test_scores_tries(void){
    const char *lignes[] = {"#o#", "# #", "#-#"};
    maze_t *m = makeMaze("tri", lignes, 3);
    stats_t s = {50, 1};
    feedInput("alice\n"); saveScore(m, &s);
    s.score = 100; feedInput("bob\n"); saveScore(m, &s);
    s.score = 20;  feedInput("carol\n"); saveScore(m, &s);

    char p[SCORE_COUNT][PSEUDO_LENGTH];
    int sc[SCORE_COUNT];
    int n = readScores("scores/tri.score", p, sc, SCORE_COUNT);
    mu_assert("scores : 3 lignes attendues", n == 3);
    mu_assert("scores : 1er = bob 100", strcmp(p[0], "bob") == 0 && sc[0] == 100);
    mu_assert("scores : 2e = alice 50", strcmp(p[1], "alice") == 0 && sc[1] == 50);
    mu_assert("scores : 3e = carol 20", strcmp(p[2], "carol") == 0 && sc[2] == 20);
    freeMaze(m);
    return 0;
}

static char *test_scores_limite_a_dix(void){
    const char *lignes[] = {"#o#", "# #", "#-#"};
    maze_t *m = makeMaze("plein", lignes, 3);
    writeFile("scores/plein.score",
              "p0 1000\np1 900\np2 800\np3 700\np4 600\n"
              "p5 500\np6 400\np7 300\np8 200\np9 100\n");

    /* Score trop faible : aucune saisie demandee, fichier inchange */
    stats_t s = {50, 1};
    feedInput("");
    saveScore(m, &s);
    char p[SCORE_COUNT + 1][PSEUDO_LENGTH];
    int sc[SCORE_COUNT + 1];
    int n = readScores("scores/plein.score", p, sc, SCORE_COUNT + 1);
    mu_assert("scores pleins : score faible ne doit pas etre ajoute", n == 10 && sc[9] == 100);

    /* Bon score : insere a sa place, le dernier est supprime */
    s.score = 950;
    feedInput("zed\n");
    saveScore(m, &s);
    n = readScores("scores/plein.score", p, sc, SCORE_COUNT + 1);
    mu_assert("scores pleins : toujours 10 lignes", n == 10);
    mu_assert("scores pleins : zed 950 en 2e position", strcmp(p[1], "zed") == 0 && sc[1] == 950);
    mu_assert("scores pleins : p9 doit avoir disparu", strcmp(p[9], "p8") == 0);
    freeMaze(m);
    return 0;
}

/* ====================================================================== */
/*                           3. Generation                                 */
/* ====================================================================== */

static char *test_labyrinthe_parfait(void){
    maze_t *m = allocMaze(11, 15);
    generateMaze(m);
    mu_assert("generation : entree attendue en (0,1)", m->labyrinthe[0][1] == -1);
    mu_assert("generation : sortie attendue en (h-1,w-2)", m->labyrinthe[10][13] == -2);
    mazePath(m);
    int cellules = 5 * 7;
    /* Un labyrinthe parfait est un arbre : exactement cellules-1 ouvertures */
    mu_assert("parfait : nombre d'ouvertures != cellules - 1", countOpenings(m) == cellules - 1);
    mu_assert("parfait : toutes les cases doivent etre reliees", isConnected(m, libreEntier));
    for (int i = 0; i < m->hauteur; i++){
        mu_assert("parfait : bord gauche/droit doit etre un mur",
                  m->labyrinthe[i][0] == 0 && m->labyrinthe[i][m->largeur - 1] == 0);
    }
    freeMaze(m);
    return 0;
}

static char *test_removeWalls_cree_des_boucles(void){
    maze_t *m = allocMaze(11, 15);
    generateMaze(m);
    mazePath(m);
    int avant = countOpenings(m);
    removeWalls(m);
    /* Au moins un mur retire, et jamais plus que de cellules */
    mu_assert("difficile : des murs supplementaires doivent etre ouverts", countOpenings(m) > avant);
    mu_assert("difficile : trop de murs retires", countOpenings(m) <= avant + 35);
    mu_assert("difficile : toujours connexe", isConnected(m, libreEntier));
    mu_assert("difficile : le bord ne doit pas etre ouvert", m->labyrinthe[1][0] == 0 && m->labyrinthe[0][3] == 0);
    freeMaze(m);
    return 0;
}

static char *test_generateItems_quantites(void){
    maze_t *m = allocMaze(11, 15);
    generateMaze(m);
    mazePath(m);
    generateItems(m);
    int attendu = (9 * 13) / 20 + 1;
    mu_assert("objets : une seule cle", countValue(m, -3) == 1);
    mu_assert("objets : nombre de tresors incorrect", countValue(m, -4) == attendu);
    mu_assert("objets : nombre de pieges incorrect", countValue(m, -5) == attendu);
    freeMaze(m);
    return 0;
}

static char *test_normalizeMaze(void){
    maze_t *m = allocMaze(3, 7);
    int codes[7] = {0, -1, -2, -3, -4, -5, 35}; /* 35 = region qui vaut '#' */
    for (int j = 0; j < 7; j++){
        m->labyrinthe[1][j] = codes[j];
    }
    normalizeMaze(m);
    const char *attendu = "#o-@?X ";
    for (int j = 0; j < 7; j++){
        mu_assert("normalizeMaze : conversion incorrecte", m->labyrinthe[1][j] == attendu[j]);
    }
    freeMaze(m);
    return 0;
}

static char *test_labyrinthe_genere_jouable(void){
    for (int essai = 0; essai < 20; essai++){
        maze_t *m = generatedMaze(15, 21, essai % 2);
        mu_assert("jouable : il faut exactement une cle", countValue(m, '@') == 1);
        mu_assert("jouable : toutes les cases doivent etre accessibles", isConnected(m, libreCaractere));
        freeMaze(m);
    }
    return 0;
}

/* ====================================================================== */
/*                       4. Deplacements du joueur                         */
/* ====================================================================== */

static char *test_movement_objets_et_sortie(void){
    const char *lignes[] = {
        "#o###",
        "# ?X#",
        "#@  #",
        "#   #",
        "###-#",
    };
    maze_t *m = makeMaze("mv", lignes, 5);
    stats_t s = {0, 0};

    mu_assert("mouvement : touche invalide", movement('x', m, &s) == -1);
    mu_assert("mouvement : bas vers case vide", movement('s', m, &s) == 0 && m->labyrinthe[1][1] == 'o');
    mu_assert("mouvement : cout d'un pas", s.score == -MOVE);
    movement('d', m, &s);
    mu_assert("mouvement : tresor", s.score == -2 * MOVE + TREASURE);
    movement('d', m, &s);
    mu_assert("mouvement : piege", s.score == -3 * MOVE + TREASURE - TRAP);
    mu_assert("mouvement : mur bloquant", movement('d', m, &s) == -1 && m->labyrinthe[1][3] == 'o');
    movement('q', m, &s);
    movement('q', m, &s);
    mu_assert("mouvement : gauche", m->labyrinthe[1][1] == 'o');
    movement('s', m, &s);
    mu_assert("mouvement : cle ramassee", s.key == 1);
    movement('d', m, &s);
    movement('d', m, &s);
    movement('s', m, &s);
    mu_assert("mouvement : sortie avec cle => victoire", movement('s', m, &s) == 1);
    freeMaze(m);
    return 0;
}

static char *test_movement_sortie_sans_cle(void){
    const char *lignes[] = {"#o#", "# #", "#-#"};
    maze_t *m = makeMaze("sc", lignes, 3);
    stats_t s = {0, 0};
    movement('s', m, &s);
    mu_assert("sortie sans cle : doit etre refusee", movement('s', m, &s) == -1);
    mu_assert("sortie sans cle : le joueur ne bouge pas", m->labyrinthe[1][1] == 'o');
    freeMaze(m);
    return 0;
}

static char *test_movement_gauche_bloque_par_mur(void){
    /* Regression : 'q' testait la mauvaise case (player[1-1]) */
    const char *lignes[] = {"#####", "# #o#", "#####"};
    maze_t *m = makeMaze("q", lignes, 3);
    stats_t s = {0, 0};
    mu_assert("gauche : un mur doit bloquer", movement('q', m, &s) == -1);
    mu_assert("gauche : le mur ne doit pas etre ecrase", m->labyrinthe[1][2] == '#');
    freeMaze(m);
    return 0;
}

/* ====================================================================== */
/*                              5. Monstres                                */
/* ====================================================================== */

static char *test_generateMonsters(void){
    maze_t *m = generatedMaze(15, 21, 1);
    int attendu = (7 * 10) / 15 + 3;
    mu_assert("monstres : nombre incorrect", m->nbMonstres == attendu);
    for (int k = 0; k < m->nbMonstres; k++){
        const monster_t *mo = &m->monstres[k];
        mu_assert("monstres : doivent apparaitre sur une case libre", m->labyrinthe[mo->x][mo->y] == ' ');
        mu_assert("monstres : trop pres de l'entree", manhattan(mo->x, mo->y, 1, 1) >= 4);
        mu_assert("monstres : etendue = penalite / 10",
                  mo->etendue == mo->type->penalite / PENALTY_PER_RANGE);
    }
    freeMaze(m);
    return 0;
}

static char *test_fantome_traverse_les_murs(void){
    const char *lignes[] = {
        "###########", "###########", "###########", "###########",
        "###########", "#####F#####", "###########", "###########",
        "###########", "###########", "###########",
    };
    maze_t *m = makeMaze("f", lignes, 11);
    loadMonsters(m);
    m->labyrinthe[5][5] = '#'; /* le fantome est entoure de murs uniquement */
    mu_assert("fantome : non charge", m->nbMonstres == 1);
    monster_t *f = &m->monstres[0];
    int aBouge = 0;
    for (int t = 0; t < 200; t++){
        moveMonsters(m, 0, 1);
        if (f->x != 5 || f->y != 5){
            aBouge = 1;
        }
        mu_assert("fantome : sort de son territoire", manhattan(f->x, f->y, 5, 5) <= f->etendue);
        mu_assert("fantome : sort du labyrinthe",
                  f->x >= 1 && f->x <= 9 && f->y >= 1 && f->y <= 9);
    }
    mu_assert("fantome : doit traverser les murs", aBouge);
    freeMaze(m);
    return 0;
}

static char *test_monstres_restent_dans_les_couloirs(void){
    maze_t *m = generatedMaze(21, 31, 1);
    for (int t = 0; t < 500; t++){
        moveMonsters(m, 0, 1);
        for (int k = 0; k < m->nbMonstres; k++){
            const monster_t *mo = &m->monstres[k];
            mu_assert("monstres : jamais sur le bord",
                      mo->x >= 1 && mo->x < m->hauteur - 1 && mo->y >= 1 && mo->y < m->largeur - 1);
            mu_assert("monstres : territoire depasse",
                      manhattan(mo->x, mo->y, mo->centreX, mo->centreY) <= mo->etendue);
            if (mo->type->symbole != 'F'){
                mu_assert("monstres : seul le fantome traverse les murs",
                          m->labyrinthe[mo->x][mo->y] != '#');
            }
        }
    }
    freeMaze(m);
    return 0;
}

static char *test_chasseur_poursuit_et_collision(void){
    const char *lignes[] = {"##########", "#o  C    #", "##########"};
    maze_t *m = makeMaze("c", lignes, 3);
    loadMonsters(m);
    monster_t *c = &m->monstres[0];
    moveMonsters(m, 1, 1);
    mu_assert("chasseur : doit se rapprocher (colonne 3)", c->x == 1 && c->y == 3);
    moveMonsters(m, 1, 1);
    moveMonsters(m, 1, 1);
    mu_assert("chasseur : doit atteindre le joueur", c->y == 1);
    mu_assert("collision : penalite du chasseur", monsterCollisions(m, 1, 1) == c->type->penalite);
    mu_assert("collision : retour au point de depart", c->x == 1 && c->y == 4);
    mu_assert("collision : pas de double penalite", monsterCollisions(m, 1, 1) == 0);
    freeMaze(m);
    return 0;
}

static char *test_ogre_garde_les_tresors(void){
    const char *lignes[] = {"############", "#  G?      #", "############"};
    maze_t *m = makeMaze("g", lignes, 3);
    loadMonsters(m);
    monster_t *g = &m->monstres[0];
    moveMonsters(m, 0, 1);
    mu_assert("ogre : doit aller sur le tresor voisin", g->y == 4);
    mu_assert("ogre : doit garder le tresor", g->garde == 1 && g->centreY == 4);
    for (int t = 0; t < 100; t++){
        moveMonsters(m, 0, 1);
        mu_assert("ogre : doit rester proche du tresor", abs(g->y - 4) <= g->etendue / 2);
    }
    m->labyrinthe[1][4] = ' '; /* le joueur ramasse le tresor */
    moveMonsters(m, 0, 1);
    mu_assert("ogre : redevient libre sans tresor", g->garde == 0);
    freeMaze(m);
    return 0;
}

static char *test_resetMonsters(void){
    const char *lignes[] = {"#######", "#  F  #", "#######"};
    maze_t *m = makeMaze("r", lignes, 3);
    loadMonsters(m);
    m->monstres[0].x = 1;
    m->monstres[0].y = 5;
    resetMonsters(m);
    mu_assert("reset : monstre replace au depart", m->monstres[0].x == 1 && m->monstres[0].y == 3);
    freeMaze(m);
    return 0;
}

/* ====================================================================== */

static char *all_tests(void){
    /* Saisies */
    mu_run_test(test_menu_rejette_saisies_invalides);
    mu_run_test(test_menu_accepte_bornes);
    mu_run_test(test_createMaze_valide_les_saisies);
    mu_run_test(test_createMaze_refus_enregistrement);
    mu_run_test(test_createMaze_petit_labyrinthe);
    /* Fichiers */
    mu_run_test(test_fichier_aller_retour_difficile);
    mu_run_test(test_loadMaze_fin_de_ligne_windows);
    mu_run_test(test_loadMaze_recupere_monstres);
    mu_run_test(test_replaceMaze_ecrit_les_monstres);
    mu_run_test(test_dossier_labyrinthes_absent);
    mu_run_test(test_scores_tries);
    mu_run_test(test_scores_limite_a_dix);
    /* Generation */
    mu_run_test(test_labyrinthe_parfait);
    mu_run_test(test_removeWalls_cree_des_boucles);
    mu_run_test(test_generateItems_quantites);
    mu_run_test(test_normalizeMaze);
    mu_run_test(test_labyrinthe_genere_jouable);
    /* Jeu */
    mu_run_test(test_movement_objets_et_sortie);
    mu_run_test(test_movement_sortie_sans_cle);
    mu_run_test(test_movement_gauche_bloque_par_mur);
    /* Monstres */
    mu_run_test(test_generateMonsters);
    mu_run_test(test_fantome_traverse_les_murs);
    mu_run_test(test_monstres_restent_dans_les_couloirs);
    mu_run_test(test_chasseur_poursuit_et_collision);
    mu_run_test(test_ogre_garde_les_tresors);
    mu_run_test(test_resetMonsters);
    return 0;
}

int main(void){
    srand(42);

    /* Dossier de travail temporaire */
    char dossier[] = "/tmp/mazegame_tests_XXXXXX";
    if (mkdtemp(dossier) == NULL || chdir(dossier) != 0){
        perror("dossier temporaire");
        return EXIT_FAILURE;
    }
    mkdir("Labyrinthes", 0700);
    mkdir("scores", 0700);

    /* Les fonctions testees affichent beaucoup : on masque stdout */
    fflush(stdout);
    int stdoutOriginal = dup(STDOUT_FILENO);
    int devNull = open("/dev/null", O_WRONLY);
    dup2(devNull, STDOUT_FILENO);
    close(devNull);

    char *resultat = all_tests();

    fflush(stdout);
    dup2(stdoutOriginal, STDOUT_FILENO);
    close(stdoutOriginal);

    if (resultat != 0){
        printf("ECHEC : %s\n", resultat);
    }
    else{
        printf("TOUS LES TESTS SONT PASSES\n");
    }
    printf("Tests executes : %d\n", tests_run);

    char commande[100];
    snprintf(commande, sizeof commande, "rm -rf %s", dossier);
    if (system(commande) != 0){
        fprintf(stderr, "Nettoyage de %s impossible\n", dossier);
    }

    return resultat != 0;
}
