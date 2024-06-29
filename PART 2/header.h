#ifndef HEADER_H
#define HEADER_H
#define width 1280
#define height 720
#define STAT_SOL 0
#define STAT_AIR 1

typedef struct
{
    int joueur; // numero du joueur qui va jouer au premier 1: si joueur, 2 si ordinateur
    SDL_Surface *image1, *image2, *image3;
    int tabsuivi[9]; // tableau representant la grille du jeu
    SDL_Rect pos;    // position d'un petit carreau a calculer pour chaque case
    int tour;        // initialement egale 0 au maximum nous avons 8 tours
    TTF_Font *comic;
    SDL_Color couleur;
    SDL_Rect posMSG;
} tic;

typedef struct
{
    SDL_Surface *img; /*!< surface image*/

} image;

typedef struct
{
    char question[100] /*!< Texte de questions et 3 reponses*/, reponse1[100] /*!< Texte de questions et 3 reponses*/, reponse2[100] /*!< Texte de questions et 3 reponses*/, reponse3[100]; /*!< Texte de questions et 3 reponses*/

    int reponse_juste /*!< nombre de la reponse juste et nombre de question dans le fichier texte*/, n_quest; /*!< nombre de la reponse juste et nombre de question dans le fichier texte*/

    SDL_Surface *surf_quest /*!<Surface image*/, *surf_rep1 /*!<Surface image*/, *surf_rep2 /*!<Surface image*/, *surf_rep3 /*!<Surface image*/, *win /*!<Surface image*/, *lose; /*!<Surface image*/

    SDL_Rect pos_q /*!< Les positions de chaque que et rep*/, pos_r1 /*!< Les positions de chaque que et rep*/, pos_r2 /*!< Les positions de chaque que et rep*/, pos_r3; /*!< Les positions de chaque que et rep*/

    SDL_Rect pos_w_l; /*!< Pos d'image victoire | defaite*/

    image tab[20]; /*!< Tableau d'animation de montre*/

} enigme;

typedef struct
{
    SDL_Surface *imgattack[19];
    SDL_Rect posBack1;
} attack;
typedef struct
{
    int status, num, dr, vi;
    float x, y;
    float vx, vy;
    SDL_Rect posperso;
    SDL_Surface *tab[20], *afficher_vie[3];
    ;
    SDL_Rect pos_vie;
    int dep, acc, vitesse;

    int vie;
    int hit;
} personne;

typedef struct
{

    SDL_Surface *texte;       /*!< surface >!*/
    SDL_Rect position;        /*!< rectangle >!*/
    char entree[100];         /*!< le temps en chaine de caracteres >!*/
    int secondesEcoulees;     /*!< le nombre de secondes écoulées>!*/
    SDL_Color couleurBlanche; /*!< couleur blanche dans le sdl >!*/
    time_t t1, t2;            /*!< t1 : temps de beginning ; t2 : temps du prochain >!*/
    int min, sec;             /*!< les minutes  ,  les secondes >!*/

} temps;

typedef struct
{
    SDL_Surface *mini_map;      /*!< surface  >!*/
    SDL_Rect position_mini_map; /*!< rectangle >!*/
    SDL_Surface *perso;         /*!< surface  >!*/
    SDL_Rect pos_perso;         /*!< rectangle >!*/
} minimap;

typedef struct
{
    float red_x; /*!< redimensionnement par rapport au axe d'abscisse >!*/
    float red_y; /*!< redimensionnement par rapport au axe d'ordonnée >!*/
} redimensionnement;

typedef struct
{
    SDL_Surface *bon;    /*!< surface >!*/
    SDL_Rect pos_joueur; /*!< rectangle >!*/
    SDL_Rect pos_joueur2;
} joueur;

typedef struct
{
    SDL_Surface *imgBack1[7];
    SDL_Rect posBack1;
    SDL_Rect scroll;

} Background;
typedef struct Ennemi
{
    SDL_Surface *sprite; /*!< Surface. */
    SDL_Rect pos;        /*!< Rectangle*/
    int direction;       /*! < derection de deplacement.*/
    SDL_Rect pos_sp;     /*!< Rectangle*/

} Ennemi;
typedef struct Ennemi2
{
    SDL_Surface *sprite2; /*!< Surface. */
    SDL_Rect pos2;        /*!< Rectangle*/
    int direction2;       /*! < derection de deplacement.*/
    SDL_Rect pos_sp2;     /*!< Rectangle*/
} Ennemi2;

void INITIALISATION();

void pos(SDL_Rect *pos, float x, float y);

void blit(SDL_Surface *what, SDL_Surface *where, SDL_Rect pos);

void MW(char name[], char icon_loc[]);

void mouse_motion(SDL_Surface *play, SDL_Surface *settings, SDL_Surface *quit, SDL_Rect pos_play, SDL_Rect pos_settings, SDL_Rect pos_quit, SDL_Event event);

void mouse_button(SDL_Surface *screen, SDL_Surface *background, SDL_Rect pos_back, SDL_Surface *play, SDL_Surface *settings, SDL_Surface *quit, SDL_Rect pos_play, SDL_Rect pos_settings, SDL_Rect pos_quit, SDL_Surface *logo, SDL_Rect pos_logo, int *c, SDL_Event event, SDL_Surface *volume, SDL_Rect pos_volume, int *vol, int *f);

void affiche_menu_fleches(int i, SDL_Surface *screen, SDL_Surface *back, SDL_Rect pos_back);

void play_menu(SDL_Surface *screen, SDL_Surface *back, SDL_Rect pos_back, SDL_Surface *play, SDL_Rect pos_play, SDL_Surface *settings, SDL_Rect pos_settings, SDL_Surface *quit, SDL_Rect pos_quit, SDL_Surface *logo, SDL_Rect pos_logo);

void settings_menu(SDL_Surface *screen, SDL_Surface *back, SDL_Rect pos_back, SDL_Surface *play, SDL_Rect pos_play, SDL_Surface *settings, SDL_Rect pos_settings, SDL_Surface *quit, SDL_Rect pos_quit, SDL_Surface *logo, SDL_Rect pos_logo, SDL_Surface *volume, SDL_Rect pos_volume, int *vol, int *f);

int singleplayer();

int multiplayer();

// enigme

int generer_entier(int min, int max);

void init_enigme(enigme *e, char *nom_fichier);

void afficher_enigme(enigme e, SDL_Surface *screen);

void animer(enigme *e, SDL_Surface *screen, SDL_Rect pos_screen, int i);

int Load_Save_score(char *fichier_score, int condition, int score);

// int charger(personnage *p, background *b, char *nomfichier);

// void sauvegarder(personnage p, background b, char *nomfichier);

/// YASSINE

void initBack(Background *b);
void initimg(Background *b);
void initnightmap(Background *b);
void afficherBack(Background b, SDL_Surface *ecran, int i);
void animateBack(Background *b);
int scrollingBack(Background *b, SDL_Rect poshero, int direction);
SDL_Color GetPixel(SDL_Surface *pSurface, int x, int y);
// int collisionPP(SDL_Rect posperso, SDL_Surface *perso, Background Masque ,int direction) ;
int collision_Parfaite(SDL_Surface *calque, joueur j, int decalage, int d);

void nom(char nomfich[], SDL_Surface *ecran, char nomh[]);
void initialiser_temps(temps *t);
void sauvegarder(int score, char nomfich[], char nomjoueur[]);
void meilleur_joueur(char nomfich[], int score, char nomjoueur[]);
void afficher_temps(temps *t, SDL_Surface *ecran);
void free_temps(temps *t);
void initminimap(minimap *m);
void MAJMinimap(SDL_Rect posJoueur, minimap *m, int p, SDL_Surface *ecran);
void afficherminimap(minimap m, SDL_Surface *screen);
void calcul_score(int *score, int direction);
void afficher_score(int score, SDL_Surface *ecran);
void afficher_meilleur_joueur(char nomjoeur[], SDL_Surface *ecran);

void initEnnemi(Ennemi *e);
void initEnnemi2(Ennemi2 *e2);
void afficherEnnemi(Ennemi e, SDL_Surface *screen);
void afficherEnnemi2(Ennemi2 e2, SDL_Surface *screen);
void animerEnnemi(Ennemi *e);
void animerEnnemi2(Ennemi2 *e2);
void deplacer(Ennemi *e);
void deplacer2(Ennemi2 *e2);
int collision(SDL_Rect E, SDL_Rect p);
int collision2(SDL_Rect E2, SDL_Rect p);
void deplacerIA(Ennemi *E, SDL_Rect posPerso);

void affichervie(personne p, SDL_Surface *ecran);
int gestionvie(personne *p, SDL_Surface *ecran);
void initperso(personne *p);
void afficherperso(personne *p, SDL_Surface *ecran);
void deplacerperso(personne *p);
void saute(personne *p, float impulsion);
void animerperso(personne *p);
void Updateperso(personne *p, Uint8 *keys, joueur *j, int scroll, SDL_Surface *a);
void init_attack(attack *atk);
void affiche_attack(attack atk, personne *p, SDL_Surface *ecran, int i);

int jeu_enigme();

// tiktak

void initialisation(tic *t);

void affichage(tic t, SDL_Surface *ecran);

int atilganer(int tabsuivi[]);

int minimax(int tabsuivi[], int joueur);

void calcul_coup(int tabsuivi[]);

void Resultat(tic t, SDL_Surface *ecran);

void liberationmemoire(tic t);

int tiktak();

#endif