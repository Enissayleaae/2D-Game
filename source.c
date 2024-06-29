#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "header.h"
#include <time.h>
#include <unistd.h>
#include <SDL/SDL_audio.h>

// tiktak

void initialisation(tic *t)
{
    int i;
    for (i = 0; i < 9; i++)
        t->tabsuivi[i] = 0;
    t->image1 = SDL_LoadBMP("fond.bmp");
    t->image2 = SDL_LoadBMP("croix.bmp");
    t->image3 = SDL_LoadBMP("rond.bmp");
    t->joueur = 1;
    t->tour = 0;
    t->comic = TTF_OpenFont("comic.ttf", 40);
    t->couleur.r = 0;
    t->couleur.g = 0;
    t->couleur.b = 0;
    t->posMSG.x = 0;
    t->posMSG.y = 0;
}

void affichage(tic t, SDL_Surface *ecran)
{
    int i;

    SDL_BlitSurface(t.image1, NULL, ecran, NULL);
    for (i = 0; i < 9; i++)
    {
        t.pos.x = 190 * (i % 3); // calculer la position d'affichgage de chaque petit carreau exemple
        t.pos.y = 190 * (i / 3);
        switch (t.tabsuivi[i]) // selon
        {
        case -1:
            SDL_BlitSurface(t.image2, NULL, ecran, &t.pos);
            break;
        case 1:
            SDL_BlitSurface(t.image3, NULL, ecran, &t.pos);
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////

void Resultat(tic t, SDL_Surface *ecran)
{
    SDL_Rect pos;
    char msg[20];

    SDL_FillRect(ecran, 0, SDL_MapRGB(ecran->format, 255, 255, 255));
    pos.x = 0;
    pos.y = 0;
    affichage(t, ecran);
    switch (atilganer(t.tabsuivi))
    {
    case 0:
        strcpy(msg, "NUL");
        break;
    case 1:
        strcpy(msg, "Defaite");
        break;
    case -1:
        strcpy(msg, "Victoire");
    }

    SDL_BlitSurface(TTF_RenderText_Solid(t.comic, msg, t.couleur), NULL, ecran, &t.posMSG);
    SDL_Flip(ecran);
    SDL_Delay(4028);
}

void liberationmemoire(tic t)
{
    SDL_FreeSurface(t.image1);
    SDL_FreeSurface(t.image2);
    SDL_FreeSurface(t.image3);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int atilganer(int tabsuivi[])
{
    // remplir cette matrice avec les combinaisons de succÃ©s
    int lignes_gagnantes[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
    int i;
    for (i = 0; i < 8; ++i)
        if (tabsuivi[lignes_gagnantes[i][0]] != 0 &&
            tabsuivi[lignes_gagnantes[i][0]] == tabsuivi[lignes_gagnantes[i][1]] &&
            tabsuivi[lignes_gagnantes[i][0]] == tabsuivi[lignes_gagnantes[i][2]])
            return tabsuivi[lignes_gagnantes[i][2]];
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int minimax(int tabsuivi[], int joueur)
{
    int gagnant = atilganer(tabsuivi);
    if (gagnant != 0)
        return gagnant * joueur;
    int coup = -1;
    int score = -2;
    int i;
    for (i = 0; i < 9; ++i)
    {
        if (tabsuivi[i] == 0)
        {
            tabsuivi[i] = joueur;
            int mmscore = -minimax(tabsuivi, joueur * -1);
            if (mmscore > score)
            {
                score = mmscore;
                coup = i;
            }
            tabsuivi[i] = 0;
        }
    }
    if (coup == -1)
        return 0;
    return score;
}
//////////////////////////////////////////////////////////////////////////////////
void calcul_coup(int tabsuivi[])
{
    int coup = -1;
    int score = -2;
    int i;
    for (i = 0; i < 9; ++i)
    {
        if (tabsuivi[i] == 0)
        {
            tabsuivi[i] = 1;
            int mmscore = -minimax(tabsuivi, -1);
            tabsuivi[i] = 0;
            if (mmscore > score)
            {
                score = mmscore;
                coup = i;
            }
        }
    }
    tabsuivi[coup] = 1;
}

int tiktak()
{
    tic t;
    int coup;
    SDL_Surface *ecran;
    TTF_Init();
    SDL_Event event;
    int continuer = 1;
    SDL_Init(SDL_INIT_VIDEO);
    ecran = SDL_SetVideoMode(552, 541, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    int joueur;
    initialisation(&t);
    int a, b;
    while (continuer)
    {
        affichage(t, ecran);
        SDL_Flip(ecran);
        if (t.tour < 9 && atilganer(t.tabsuivi) == 0)
        {
            if ((t.tour + joueur) % 2 == 0) // tour du PC
            {
                calcul_coup(t.tabsuivi);
                t.tour++;
            }
            else
            {
                SDL_WaitEvent(&event);
                switch (event.type)
                {
                case SDL_QUIT:
                    continuer = 0;
                    break;
                case SDL_MOUSEBUTTONUP:
                    a = event.button.x / 190;
                    b = event.button.y / 190;
                    coup = 3 * b + a;
                    t.tour++;
                    break;
                }

                t.tabsuivi[coup] = -1;
            }
        }
        else
        {
            Resultat(t, ecran);
            continuer = 0;
            printf("%d", t.tour);
        }
    }
    liberationmemoire(t);
    return 1;
}
/////////////////////////////////////////////////////////////////////////////////

// enigme

//////////// enigme

int generer_entier(int min, int max)
{
    int n;

    srand(time(0));

    n = (rand() % (max - min + 1)) + min;

    return n;
}

/**
 * @brief initialiser d'enigme
 * @param e enigme
 * @param nom_fichier nom fichier texte
 */
void init_enigme(enigme *e, char *nom_fichier)
{
    // LOAD ENIGME

    int n, n_enigme = 0;

    FILE *f = NULL;

    n = generer_entier(1, 4);

    fprintf(stderr, "%d", n);

    f = fopen(nom_fichier, "r");

    if (f == NULL)
        fprintf(stderr, "Erreur d'ouverture fichier \n");

    else
    {
        while (fscanf(f, "%d %s %s %s %s %d\n", &e->n_quest, e->question, e->reponse1, e->reponse2, e->reponse3, &e->reponse_juste) != EOF)
        {
            n_enigme++;

            if (n_enigme == n)
                break;
        }

        fclose(f);
    }

    // ANIMATION

    e->tab[0].img = IMG_Load("clock1.png");
    e->tab[1].img = IMG_Load("clock2.png");
    e->tab[2].img = IMG_Load("clock3.png");
    e->tab[3].img = IMG_Load("clock4.png");
    e->tab[4].img = IMG_Load("clock5.png");
    e->tab[5].img = IMG_Load("clock6.png");
    e->tab[6].img = IMG_Load("clock7.png");
    e->tab[7].img = IMG_Load("clock8.png");
    e->tab[8].img = IMG_Load("clock9.png");

    e->win = IMG_Load("win.png");
    e->lose = IMG_Load("lose.png");

    e->pos_w_l.x = (1280 / 2) - (260 / 2);
    e->pos_w_l.y = (720 / 2) - (260 / 2);
}

/**
 * @brief Afficher enigme
 * @param e enigme
 * @param screen l'ecran
 */
void afficher_enigme(enigme e, SDL_Surface *screen)
{
    SDL_Surface *back_enigme = NULL;

    SDL_Rect pos_enig;

    back_enigme = IMG_Load("back_enigm.png");

    pos_enig.x = 0;
    pos_enig.y = 0;

    // POSITION DE TEXTE

    switch (e.n_quest)
    {
    case 1:
        e.pos_q.x = 150;
        e.pos_q.y = 250;

        e.pos_r1.x = 130;
        e.pos_r1.y = 450;

        e.pos_r2.x = 559;
        e.pos_r2.y = 450;

        e.pos_r3.x = 890;
        e.pos_r3.y = 450;

        break;

    case 2:

        e.pos_q.x = 260;
        e.pos_q.y = 250;

        e.pos_r1.x = 175;
        e.pos_r1.y = 450;

        e.pos_r2.x = 520;
        e.pos_r2.y = 450;

        e.pos_r3.x = 975;
        e.pos_r3.y = 450;

        break;

    case 3:

        e.pos_q.x = 280;
        e.pos_q.y = 250;

        e.pos_r1.x = 190;
        e.pos_r1.y = 450;

        e.pos_r2.x = 590;
        e.pos_r2.y = 450;

        e.pos_r3.x = 990;
        e.pos_r3.y = 450;

        break;

    case 4:

        e.pos_q.x = 530;
        e.pos_q.y = 250;

        e.pos_r1.x = 200;
        e.pos_r1.y = 450;

        e.pos_r2.x = 600;
        e.pos_r2.y = 450;

        e.pos_r3.x = 1000;
        e.pos_r3.y = 450;

        break;
    }

    TTF_Font *font_enigme = TTF_OpenFont("OpenSans-Semibold.ttf", 40);

    SDL_Color color_q = {255, 0, 0};
    SDL_Color color_r = {255, 255, 255};

    e.surf_quest = TTF_RenderText_Blended(font_enigme, e.question, color_q);
    e.surf_rep1 = TTF_RenderText_Blended(font_enigme, e.reponse1, color_r);
    e.surf_rep2 = TTF_RenderText_Blended(font_enigme, e.reponse2, color_r);
    e.surf_rep3 = TTF_RenderText_Blended(font_enigme, e.reponse3, color_r);

    SDL_BlitSurface(back_enigme, NULL, screen, &pos_enig);
    SDL_BlitSurface(e.surf_quest, NULL, screen, &e.pos_q);
    SDL_BlitSurface(e.surf_rep1, NULL, screen, &e.pos_r1);
    SDL_BlitSurface(e.surf_rep2, NULL, screen, &e.pos_r2);
    SDL_BlitSurface(e.surf_rep3, NULL, screen, &e.pos_r3);

    SDL_Flip(screen);
    SDL_FreeSurface(back_enigme);
}

/**
 * @brief Afficher chaque image de l'animation
 * @param e enigme
 * @param screen l'ecran
 * @param pos_screen position d'ecran (0,0)
 * @param i numero d'image dans tableau image animation
 */
void animer(enigme *e, SDL_Surface *screen, SDL_Rect pos_screen, int i)
{
    SDL_BlitSurface(e->tab[i].img, NULL, screen, &pos_screen);
}

/**
 * @brief charger et sauvegarder le score avant et apres l'enigme a partir de fich texte
 * @param fichier_score fichier texte score
 * @param condition 1 = LOAD, 2 = SAVE
 * @param score score de joueur
 */
int Load_Save_score(char *fichier_score, int condition, int score)
{
    FILE *f = NULL;

    f = fopen(fichier_score, "r+");

    if (f == NULL)
    {
        printf("Erreur d'ouverture fichier score \n");
        fclose(f);
        return 0;
    }
    else
    {
        if (condition == 1) // LOAD//
        {
            fscanf(f, "%d", &score);
            fclose(f);
            return score;
        }
        else if (condition == 2) // SAVE//
        {
            fprintf(f, "%d", score);
            fclose(f);
            return score;
        }
    }
}

int jeu_enigme(int *score)
{
    personne p;
    Background b;

    int test;

    char fichier_charger[20] = "charger.txt";

    // CHARGER PERSO ET BACK

    // test = charger(&p, &b, fichier_charger); //(TEST == 1 : SUCCESS) //(TEST == 0 : FAILED)

    // VALEURS PERSO & BACK CHANGE DURANT LE JEU

    /* PERSO */
    /*p.pos_score.x = 2;
    p.pos_score.y = 2;

    p.position_personnage.x = 3;
    p.position_personnage.y = 3;

    p.score = 4;

    p.vie = 5;

    p.time = 6.123;*/

    /* BACK */
    /* b.posback.x = 7;
    b.posback.y = 7;

    b.camera.x = 8;
    b.camera.y = 8;
    */

    // INITIALISATION PROGRAMME

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "Erreur INITIALISATION %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    TTF_Init();

    if (TTF_Init() != 0)
    {
        fprintf(stderr, "Erreur TTF_INIT %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Declaration variables

    enigme e;

    char nom_fichier[20] = "fichier.txt";
    char fichier_score[20] = "score.txt";
    char st_score[20];

    int c = 1, c2 = 0, i = 0, t = 0, r = -1, j = 1, choose, c3 = 0, pm = 0, key = 1;

    SDL_Surface *screen = NULL, *button = NULL, *surf_score = NULL, *score_pannel = NULL, *pause = NULL;

    pause = IMG_Load("resume.png");

    SDL_Rect pos_screen, pos_a, pos_b, pos_c, pos_score_pannel, pos_score, pos_pause;

    pos_screen.x = 0;
    pos_screen.y = 0;

    pos_a.x = 150;
    pos_a.y = 515;

    pos_b.x = 555;
    pos_b.y = 515;

    pos_c.x = 960;
    pos_c.y = 515;

    pos_score_pannel.x = 660;
    pos_score_pannel.y = 7;

    pos_score.x = 740;
    pos_score.y = 20;

    pos_pause.x = (1280 / 2) - (pause->w / 2);
    pos_pause.y = (720 / 2) - (pause->h / 2);

    // LOAD SCORE

    // score = Load_Save_score(fichier_score, 1, score);
    fprintf(stderr, "SCORE = %d \n", *score);

    // INIT SCORE

    TTF_Font *font_score = TTF_OpenFont("Retro Gaming.ttf", 30);

    SDL_Color color_score = {0, 255, 255};

    sprintf(st_score, "%d", *score);

    surf_score = TTF_RenderText_Blended(font_score, st_score, color_score);

    // SON

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
    Mix_Chunk *win, *lose;
    win = Mix_LoadWAV("win.wav");
    lose = Mix_LoadWAV("fail.wav");

    // MW_CAPTION and WM_SETICON

    SDL_WM_SetCaption("Enigme", NULL);
    SDL_WM_SetIcon(IMG_Load("icon.jpg"), NULL);

    score_pannel = IMG_Load("score_pannel.png");

    // SET VIDEO MODE

    screen = SDL_SetVideoMode(1280, 720, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);

    if (screen == NULL)
    {
        fprintf(stderr, "Erreur Video %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    init_enigme(&e, nom_fichier);

    afficher_enigme(e, screen);
    SDL_BlitSurface(score_pannel, NULL, screen, &pos_score_pannel);

    button = IMG_Load("choose.png");

    while (c)
    {
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type)
        {
        case SDL_MOUSEBUTTONUP:
            if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 150) && (event.motion.x <= 320) && (event.motion.y >= 515) && (event.motion.y <= 685))
            {
                choose = 1;
                r = 1;
            }
            else if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 555) && (event.motion.x <= 725) && (event.motion.y >= 515) && (event.motion.y <= 685))
            {
                choose = 2;
                r = 2;
            }
            else if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 960) && (event.motion.x <= 1130) && (event.motion.y >= 515) && (event.motion.y <= 685))
            {
                choose = 3;
                r = 3;
            }
            break;

        case SDL_QUIT:
            c = 0;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                c = 0;
                break;

            case SDLK_p:

                SDL_Delay(100);

                if (pm == 0)
                    pm = 1;
                else
                    pm = 0;

                if (pm)
                {
                    c3 = 1;
                    while (c3)
                    {
                        SDL_Event event3;
                        SDL_PollEvent(&event3);

                        SDL_BlitSurface(pause, NULL, screen, &pos_pause);

                        SDL_Flip(screen);

                        switch (event3.type)
                        {
                        case SDL_MOUSEMOTION:

                            if ((event3.motion.x >= 493) && (event3.motion.x <= 788) && (event3.motion.y >= 207) && (event3.motion.y <= 276))
                            {
                                key = 1;
                            }
                            else if ((event3.motion.x >= 493) && (event3.motion.x <= 788) && (event3.motion.y >= 345) && (event3.motion.y <= 414))
                            {
                                key = 2;
                            }
                            else if ((event3.motion.x >= 493) && (event3.motion.x <= 788) && (event3.motion.y >= 490) && (event3.motion.y <= 563))
                            {
                                key = 3;
                            }

                            break;

                        case SDL_MOUSEBUTTONUP:

                            if ((event3.button.button == SDL_BUTTON_LEFT) && (event3.motion.x >= 493) && (event3.motion.x <= 788) && (event3.motion.y >= 207) && (event3.motion.y <= 276))
                            {
                                c3 = 0;
                            }

                            else if ((event3.button.button == SDL_BUTTON_LEFT) && (event3.motion.x >= 493) && (event3.motion.x <= 788) && (event3.motion.y >= 345) && (event3.motion.y <= 414))
                            {
                                *score = Load_Save_score(fichier_score, 2, *score); // save the score
                                // sauvegarder2(p, b, fichier_charger);              // save back and perso
                            }

                            else if ((event3.button.button == SDL_BUTTON_LEFT) && (event3.motion.x >= 493) && (event3.motion.x <= 788) && (event3.motion.y >= 490) && (event3.motion.y <= 563))
                            {
                                c = 0;
                                c3 = 0;
                            }

                            break;

                        case SDL_KEYDOWN:
                            switch (event3.key.keysym.sym)
                            {
                            case SDLK_r:
                                c3 = 0;
                                break;

                            case SDLK_s:
                                *score = Load_Save_score(fichier_score, 2, *score); // save the score
                                // sauvegarder2(p, b, fichier_charger);              // save back and perso
                                break;

                            case SDLK_q:
                                c = 0;
                                c3 = 0;
                                break;

                            case SDLK_DOWN:
                                SDL_Delay(100);
                                if (key == 3)
                                    key = 1;
                                else
                                    key++;
                                break;

                            case SDLK_UP:
                                SDL_Delay(100);
                                if (key == 1)
                                    key = 3;
                                else
                                    key--;
                                break;

                            case SDLK_RETURN:

                                switch (key)
                                {
                                case 1:

                                    c3 = 0;

                                    break;

                                case 2:

                                    *score = Load_Save_score(fichier_score, 2, *score); // save the score
                                    // sauvegarder2(p, b, fichier_charger);              // save back and perso

                                    break;

                                case 3:

                                    c = 0;
                                    c3 = 0;

                                    break;
                                }

                                break;
                            }
                            break;
                        }

                        switch (key)
                        {
                        case 1:
                            SDL_FreeSurface(pause);
                            pause = IMG_Load("resume.png");
                            SDL_BlitSurface(pause, NULL, screen, &pos_pause);
                            break;

                        case 2:
                            SDL_FreeSurface(pause);
                            pause = IMG_Load("save.png");
                            SDL_BlitSurface(pause, NULL, screen, &pos_pause);
                            break;

                        case 3:
                            SDL_FreeSurface(pause);
                            pause = IMG_Load("quit.png");
                            SDL_BlitSurface(pause, NULL, screen, &pos_pause);
                            break;
                        }
                    }
                }
                else
                {
                    afficher_enigme(e, screen);
                    SDL_BlitSurface(score_pannel, NULL, screen, &pos_score_pannel);
                }

                SDL_Flip(screen);

                break;

            case SDLK_a:
                choose = 1;
                r = 1;
                break;

            case SDLK_b:
                choose = 2;
                r = 2;
                break;

            case SDLK_c:
                choose = 3;
                r = 3;
                break;
            }
            break;
        }

        // CHANGEMENT DE TEMPS

        if (j && !pm)
        {
            t++;

            switch (t)
            {
            case 400:
                i = 5;
                break;

            case 800:
                i = 1;
                break;

            case 1200:
                i = 6;
                break;

            case 1600:
                i = 2;
                break;

            case 2000:
                i = 7;
                break;

            case 2400:
                i = 3;
                break;

            case 2800:
                i = 8;
                break;

            case 3200:
                i = 4;
                break;

            case 3600:
                i = 9;
                r = 0; // TIME RUN OUT
                break;
            }
        }

        if (r == 0)
        {
            *score -= 20;
            j = 0;
            Mix_PlayChannel(-1, lose, 0);
            SDL_BlitSurface(e.lose, NULL, screen, &e.pos_w_l);
            c2 = 1;
            c = 0;
        }
        else if (r == e.reponse_juste)
        {
            *score += 20;
            j = 0;
            Mix_PlayChannel(-1, win, 0);
            SDL_BlitSurface(e.win, NULL, screen, &e.pos_w_l);
            c2 = 1;
            c = 0;
        }
        else if (r != e.reponse_juste && r != 0 && r > 0)
        {
            *score -= 20;
            j = 0;
            Mix_PlayChannel(-1, lose, 0);
            SDL_BlitSurface(e.lose, NULL, screen, &e.pos_w_l);
            c2 = 1;
            c = 0;
        }

        // CLOCK ANIMATION

        animer(&e, screen, pos_screen, i);

        switch (choose)
        {
        case 1:
            SDL_BlitSurface(button, NULL, screen, &pos_a);
            break;

        case 2:
            SDL_BlitSurface(button, NULL, screen, &pos_b);
            break;

        case 3:
            SDL_BlitSurface(button, NULL, screen, &pos_c);
            break;
        }

        SDL_BlitSurface(e.tab[i].img, NULL, screen, &pos_screen); // CLOCK

        // AFFICHE SCORE

        SDL_BlitSurface(surf_score, NULL, screen, &pos_score);

        SDL_Flip(screen);
    }

    while (c2)
    {
        *score = Load_Save_score(fichier_score, 2, *score); // save the score

        // sauvegarder2(p, b, fichier_charger); // save back and perso

        SDL_BlitSurface(score_pannel, NULL, screen, &pos_score_pannel);
        *score = Load_Save_score(fichier_score, 1, *score);
        sprintf(st_score, "%d", *score);
        SDL_FreeSurface(surf_score);
        surf_score = TTF_RenderText_Blended(font_score, st_score, color_score);
        SDL_BlitSurface(surf_score, NULL, screen, &pos_score);

        SDL_Flip(screen);

        SDL_Event event2;
        SDL_PollEvent(&event2);

        switch (event2.type)
        {

        case SDL_QUIT:
            c2 = 0;
            break;

        case SDL_KEYDOWN:
            switch (event2.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                c2 = 0;
                break;
            }
            break;
        }
    }

    // FREE

    SDL_FreeSurface(surf_score);
    SDL_FreeSurface(score_pannel);
    SDL_FreeSurface(button);
    SDL_FreeSurface(pause);
    Mix_FreeChunk(lose);
    Mix_FreeChunk(win);

    return 1;
}

///////BACK.C

void initBack(Background *b)
{
    b->imgBack1[0] = IMG_Load("trymap1080.png");
    b->posBack1.x = 0;
    b->posBack1.y = 0;
    b->scroll.x = 0;
    b->scroll.y = 0;
    b->scroll.w = 12000;
    b->scroll.h = 1920;
}
void initimg(Background *b)
{
    b->imgBack1[0] = IMG_Load("trymap1080.png");
    b->imgBack1[1] = IMG_Load("trymap1080.png");
    b->imgBack1[2] = IMG_Load("trymap1080.png");
    b->imgBack1[3] = IMG_Load("trymap1080.png");
    b->imgBack1[4] = IMG_Load("trymap1080.png");
    b->imgBack1[5] = IMG_Load("trymap1080.png");
    b->imgBack1[6] = IMG_Load("trymap1080.png");
    b->imgBack1[7] = IMG_Load("trymap1080.png");
}

void initnightmap(Background *b)
{
    b->imgBack1[0] = IMG_Load("realrealnightmap.png");
    b->posBack1.x = 0;
    b->posBack1.y = 0;
    b->scroll.x = 1600;
    b->scroll.y = 100;
    b->scroll.w = 6000;
    b->scroll.h = 1894;
}
void afficherBack(Background b, SDL_Surface *ecran, int i)
{

    SDL_BlitSurface(b.imgBack1[i], &(b.scroll), ecran, NULL);
}
int scrollingBack(Background *b, SDL_Rect poshero, int direction)
{
    if (direction == 1) // droite
    {
        b->scroll.x += 10;
        b->posBack1.x += 10;
        if (b->scroll.x >= 10080)
            b->scroll.x = 10080;
        printf("scroll right abcisse cam: %d abcisse bg %d\n", b->scroll.x, b->posBack1.x);
        return 1;
    }
    else if (direction == 2) // gauche
    {
        b->scroll.x -= 10;
        b->posBack1.x -= 10;
        if (b->scroll.x <= 20)
            b->scroll.x = 20;
        printf("scroll left abcisse cam: %d abcisse bg %d\n", b->scroll.x, b->posBack1.x);
        return 1;
    }
    else if (direction == 3) // up
    {
        b->scroll.y -= 10;
        b->posBack1.y -= 10;
        if (b->scroll.y <= 0)
            b->scroll.y = 0;

        printf("scroll up abcisse cam: %d abcisse bg %d\n", b->scroll.y, b->posBack1.y);
        return 1;
    }
    else if (direction == 4) // down in case
    {
        b->scroll.y += 10;
        b->posBack1.y += 10;
        if (b->scroll.y >= 0)
            b->scroll.y = 0;
        printf("scroll down abcisse cam: %d abcisse bg %d\n", b->scroll.y, b->posBack1.y);
        return 1;
    }
}
SDL_Color GetPixel(SDL_Surface *pSurface, int x, int y)
{
    SDL_Color color;
    Uint32 col = 0;
    char *pPosition = (char *)pSurface->pixels;

    // offset by y
    pPosition += (pSurface->pitch * y);

    // offset by x
    pPosition += (pSurface->format->BytesPerPixel * x);

    // copy pixel data
    memcpy(&col, pPosition, pSurface->format->BytesPerPixel);

    // convert color
    SDL_GetRGB(col, pSurface->format, &color.r, &color.g, &color.b);
    return (color);
}

int collision_Parfaite(SDL_Surface *calque, joueur j, int decalage, int d)
{
    SDL_Color col;

    if (d == 1)
    {
        col = GetPixel(calque, j.pos_joueur.x + j.bon->w + decalage, j.pos_joueur.y + (j.bon->h / 2));
    }
    else if (d == 2)
    {
        col = GetPixel(calque, j.pos_joueur.x + decalage, j.pos_joueur.y + (j.bon->h / 2));
    }
    else if (d == 3)
    {
        col = GetPixel(calque, j.pos_joueur.x + (j.bon->w / 2) + decalage, j.pos_joueur.y);
    }
    else if (d == 4)
    {
        col = GetPixel(calque, j.pos_joueur.x + (j.bon->w / 2) + decalage, j.pos_joueur.y + j.bon->h);
    }

    // printf("%d    %d   %d\n",col.r,col.b,col.g );

    if ((col.r == 0) && (col.b == 0) && (col.g == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

///////////ENEMIE.C

void initEnnemi(Ennemi *e)
{
    e->sprite = IMG_Load("enemie.png");
    e->pos.y = 820;
    e->pos.x = 600;
    e->pos.h = 100;
    e->pos.w = 100;

    e->direction = 0;

    e->pos_sp.x = 0;
    e->pos_sp.y = 0;
    e->pos_sp.h = 100;
    e->pos_sp.w = 100;
}
/**
 * @brief To affiche the enimie .
 * @param e the ennemi
 * @param screen the screen of program
 * @return Nothing
 */
void afficherEnnemi(Ennemi e, SDL_Surface *screen)
{
    SDL_BlitSurface(e.sprite, &e.pos_sp, screen, &e.pos);
}
/**
 * @brief To animer the ennemi .
 * @param e the ennemi
 * @return Nothing
 */
void animerEnnemi(Ennemi *e)
{
    e->pos_sp.x += e->pos_sp.w;
    if (e->pos_sp.x == 400)
    {
        e->pos_sp.x = 0;
    }
    e->pos_sp.y = e->direction * e->pos_sp.h;
}
/**
 * @brief To deplacer the ennemi .
 * @param e the ennemi
 * @return Nothing
 */
void deplacer(Ennemi *e)
{
    int Xmin = 500, Xmax = 800;
    if (e->direction == 0)
    {
        e->pos.x += 2;
    }
    else if (e->direction == 1)
    {
        e->pos.x -= 2;
    }
    if (e->pos.x <= Xmin)
    {
        e->direction = 0;
    }
    if (e->pos.x >= Xmax)
    {
        e->direction = 1;
    }
}
/**
 * @brief To initialize the collision .
 * @param E the size of ennemi
 * @param p the size of perso
 * @return  1 = collision , 0 = no collision
 */
int collision(SDL_Rect E, SDL_Rect p)
{
    int collision;
    if (((p.x + p.w) < E.x) || (p.x > (E.x + E.w)) || ((p.y + p.h) < E.y) || (p.y > (E.y + E.h)))
    {
        collision = 0;
    }
    else
    {
        collision = 1;
    }
    return collision;
}

void deplacerIA(Ennemi *E, SDL_Rect posPerso)
{
    if (((E->pos.x - posPerso.x) < 300) && ((E->pos.x - posPerso.x) > 80))
    {
        E->direction = 1;
        E->pos.x -= 5;
    }
    else if (((E->pos.x - posPerso.x) < -80) && ((E->pos.x - posPerso.x) > -300))
    {
        E->direction = 0;
        E->pos.x += 5;
    }
    else if (((E->pos.x - posPerso.x) <= 80) && ((E->pos.x - posPerso.x) >= 0))
    {
        E->direction = 3;
    }
    else if (((E->pos.x - posPerso.x) <= 0) && ((E->pos.x - posPerso.x) >= -80))
    {
        E->direction = 2;
    }
    else
        deplacer(E);
}

// enemie 2
void initEnnemi2(Ennemi2 *e2)
{
    e2->sprite2 = IMG_Load("enemie2.png");
    e2->pos2.y = 400;
    e2->pos2.x = 200;
    e2->pos2.h = 100;
    e2->pos2.w = 100;

    e2->direction2 = 0;

    e2->pos_sp2.x = 0;
    e2->pos_sp2.y = 300;
    e2->pos_sp2.h = 100;
    e2->pos_sp2.w = 100;
}
void afficherEnnemi2(Ennemi2 e2, SDL_Surface *screen)
{
    SDL_BlitSurface(e2.sprite2, &e2.pos_sp2, screen, &e2.pos2);
}
void animerEnnemi2(Ennemi2 *e2)
{
    e2->pos_sp2.y -= e2->pos_sp2.h;
    if (e2->pos_sp2.y == 0)
    {
        e2->pos_sp2.y = 300;
    }
    e2->pos_sp2.x = e2->direction2 * e2->pos_sp2.w;
}
void deplacer2(Ennemi2 *e2)
{
    int Ymin = 100, Ymax = 400;
    if (e2->direction2 == 1)
    {
        e2->pos2.y += 10;
    }
    else if (e2->direction2 == 0)
    {
        e2->pos2.y -= 10;
    }
    if (e2->pos2.y <= Ymin)
    {
        e2->direction2 = 1;
    }
    if (e2->pos2.y >= Ymax)
    {
        e2->direction2 = 0;
    }
}
int collision2(SDL_Rect E2, SDL_Rect p)
{
    int collision2;
    if (((p.x + p.w) < E2.x) || (p.x > (E2.x + E2.w)) || ((p.y + p.h) < E2.y) || (p.y > (E2.y + E2.h)))
    {
        collision2 = 0;
    }
    else
    {
        collision2 = 1;
    }
    return collision2;
}

// MINIMAP.C

void initminimap(minimap *m)
{
    m->position_mini_map.x = 0;
    m->position_mini_map.y = 617;
}

//----------------------------------Initialiser-Le-Temps-------------------------------------//
/**
 * @brief initialiser le temps , charger la police , avoir le temps de départ
 * @param t temps
 * @return nothing
 */

void initialiser_temps(temps *t)
{
    t->texte = NULL;
    t->position.x = 1750;
    t->position.y = 0;
    strcpy(t->entree, "");
    t->secondesEcoulees = 0;
    time(&(t->t1));
    // temps du debut
}

//----------------------------------Affichage-Du-Temps----------------------------------------//
/**
 * @brief avoir le prochain temps , transfomer le temps en min et sec , afficher le temps sur l'ecran
 * @param t temps
 * @param ecran surface
 */

void afficher_temps(temps *t, SDL_Surface *ecran)
{
    SDL_Color couleurBlanche = {255, 255, 255};
    TTF_Font *police = TTF_OpenFont("COOPBL.ttf", 40);
    time(&(t->t2));

    t->secondesEcoulees = t->t2 - t->t1;
    t->min = ((t->secondesEcoulees / 60) % 60);
    t->sec = ((t->secondesEcoulees) % 60);
    int millisecondes = SDL_GetTicks() % 60;
    sprintf(t->entree, "%02d:%02d:%02d", t->min, t->sec, millisecondes);
    t->texte = TTF_RenderText_Blended(police, t->entree, couleurBlanche);
    SDL_BlitSurface(t->texte, NULL, ecran, &(t->position));
    TTF_CloseFont(police);
}

//---------------------------------Liberer---------------------------------------------------//
/**
 * @brief liberer le temps
 * @param t temps
 */
void free_temps(temps *t)
{
    SDL_FreeSurface(t->texte);
}

//---------------------------------Meilleur-Joueur-------------------------------------------//
/**
 * @brief  déterminer le meilleur joueur
 * @param nomfich nom de fichier chaine de caractères
 * @param score le temps ecoulé
 * @param nomjoueur nom de joueur
 */

void meilleur_joueur(char nomfich[], int score, char nomjoueur[])
{
    FILE *fic;
    int sc;
    char nj[15];
    fic = fopen(nomfich, "a+");
    fscanf(fic, "%s , %d\n", nomjoueur, &score);
    while (!(feof(fic)))
    {
        fscanf(fic, "%s , %d\n", nj, &sc);
        if (sc > score)
        {
            score = sc;
            strcpy(nomjoueur, nj);
        }
    }
    printf("\n%s , %d", nomjoueur, score);
}

//---------------------------------Mise-a-jour-----------------------------------------------//
/**
 * @brief déterminer la position du miniperso par rapport au joueur principal dans le minimap
 * @param posJoueur postion du joueur du background
 * @param m minimap
 * @param camera pour le scrolling
 * @param redimenstionnement redimensionnement par rapport au axe d'abscisse et l'axe d'ordonnée
 * @param ecran surface de l'ecran
 * @param fois pour tester le nombre de fois qu'on a apppele la fonction MAJMinimap
 */
void MAJMinimap(SDL_Rect posJoueur, minimap *m, int p, SDL_Surface *ecran)
{
    redimensionnement redi;
    redi.red_x = 32; // redimensionnement par rapport au axe d'abscisse
    redi.red_y = 75; // redimensionnement par rapport au axe d'ordonnée
    m->pos_perso.x = posJoueur.x * (redi.red_x / 100);
    m->pos_perso.y = posJoueur.y * (redi.red_y / 100) + 617;
}

//------------------------------------Affichage du minimap-----------------------------------------//
/**
 * @brief affichage du minimap sur l'ecran
 * @param m minimap
 * @param ecran surface de l'ecran
 */
void afficherminimap(minimap m, SDL_Surface *ecran)
{
    m.mini_map = IMG_Load("234MINI.png");
    SDL_BlitSurface(m.mini_map, NULL, ecran, &(m).position_mini_map);
    m.perso = IMG_Load("persomini.png");
    SDL_BlitSurface(m.perso, NULL, ecran, &(m).pos_perso);
}

//-----------------------------------NOM----------------------------------------------------------//
/**
 * @brief recevoir le nom du joueur a partir du clavier numerique et l'afficher sur l'ecran
 * @param nomfich nom du fichier
 * @param ecran surface de l'ecran
 * @param nomh la chaine dans laquelle on va récuperer le nom du joueur a travers une concaténation des caractères
 */

void nom(char nomfich[], SDL_Surface *ecran, char nomh[])
{
    FILE *fic;
    SDL_Color couleur = {0, 0, 255};
    int continuer = 1;
    char ch[20];
    SDL_Surface *texte, *fenetre = NULL;
    SDL_Rect pos_texte, pos_fenetre;
    TTF_Font *police;
    SDL_Event event;
    int ok = 0;
    fic = fopen(nomfich, "a+");
    police = TTF_OpenFont("COOPBL.ttf", 60);
    pos_fenetre.x = 660;
    pos_fenetre.y = 371;
    fenetre = IMG_Load("OOO.png");
    SDL_BlitSurface(fenetre, NULL, ecran, &pos_fenetre);
    SDL_Flip(ecran);
    pos_texte.x = 790;
    pos_texte.y = 520;
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_RETURN:
                ok = 1;
                printf("\nhhhh");
                continuer = 0;
                break;
            case SDLK_0:
                strcat(nomh, "0");
                printf("\nhhhh");
                strcpy(ch, "0");
                break;
            case SDLK_1:
                strcat(nomh, "1");
                strcpy(ch, "1");
                break;
            case SDLK_2:
                strcat(nomh, "2");
                strcpy(ch, "2");
                break;
            case SDLK_3:
                strcat(nomh, "3");
                strcpy(ch, "3");
                break;
            case SDLK_4:
                strcat(nomh, "4");
                strcpy(ch, "4");
                break;
            case SDLK_5:
                strcat(nomh, "5");
                strcpy(ch, "5");
                break;
            case SDLK_6:
                strcat(nomh, "6");
                strcpy(ch, "6");
                break;
            case SDLK_7:
                strcat(nomh, "7");
                strcpy(ch, "7");
                break;
            case SDLK_8:
                strcat(nomh, "8");
                strcpy(ch, "8");
                break;
            case SDLK_9:
                strcat(nomh, "9");
                strcpy(ch, "9");
                break;
            case SDLK_a:
                strcat(nomh, "a");
                strcpy(ch, "a");
                break;
            case SDLK_b:
                strcat(nomh, "b");
                strcpy(ch, "b");
                break;
            case SDLK_c:
                strcat(nomh, "c");
                strcpy(ch, "c");
                break;
            case SDLK_d:
                strcat(nomh, "d");
                strcpy(ch, "d");
                break;
            case SDLK_e:
                strcat(nomh, "e");
                strcpy(ch, "e");
                break;
            case SDLK_f:
                strcat(nomh, "f");
                strcpy(ch, "f");
                break;
            case SDLK_g:
                strcat(nomh, "g");
                strcpy(ch, "g");
                break;
            case SDLK_h:
                strcat(nomh, "h");
                strcpy(ch, "h");
                break;
            case SDLK_i:
                strcat(nomh, "i");
                strcpy(ch, "i");
                break;
            case SDLK_j:
                strcat(nomh, "j");
                strcpy(ch, "j");
                break;
            case SDLK_k:
                strcat(nomh, "k");
                strcpy(ch, "k");
                break;
            case SDLK_l:
                strcat(nomh, "l");
                strcpy(ch, "l");
                break;
            case SDLK_m:
                strcat(nomh, "m");
                strcpy(ch, "m");
                break;
            case SDLK_n:
                strcat(nomh, "n");
                strcpy(ch, "n");
                break;
            case SDLK_o:
                strcat(nomh, "o");
                strcpy(ch, "o");
                break;
            case SDLK_p:
                strcat(nomh, "p");
                strcpy(ch, "p");
                break;
            case SDLK_q:
                strcat(nomh, "q");
                strcpy(ch, "q");
                break;
            case SDLK_r:
                strcat(nomh, "r");
                strcpy(ch, "r");
                break;
            case SDLK_s:
                strcat(nomh, "s");
                strcpy(ch, "s");
                break;
            case SDLK_t:
                strcat(nomh, "t");
                strcpy(ch, "t");
                break;
            case SDLK_u:
                strcat(nomh, "u");
                strcpy(ch, "u");
                break;
            case SDLK_v:
                strcat(nomh, "v");
                strcpy(ch, "v");
                break;
            case SDLK_w:
                strcat(nomh, "w");
                strcpy(ch, "w");
                break;
            case SDLK_x:
                strcat(nomh, "x");
                strcpy(ch, "x");
                break;
            case SDLK_y:
                strcat(nomh, "y");
                strcpy(ch, "y");
                break;
            case SDLK_z:
                strcat(nomh, "z");
                strcpy(ch, "z");
                break;
            default:
                strcat(nomh, "z");
                strcpy(ch, "z");
                ok = 1;
                break;
            }
            if (!ok)
            {
                texte = TTF_RenderText_Blended(police, ch, couleur);
                SDL_BlitSurface(texte, NULL, ecran, &pos_texte);
                pos_texte.x += 30;
                SDL_Flip(ecran);
            }
            break;
        }
    }
    fprintf(fic, "%s , %d\n", nomh, 0);
    fclose(fic);
    SDL_FreeSurface(fenetre);
    TTF_CloseFont(police);
    SDL_FreeSurface(texte);
}

//-------------------------------------Sauvegarder-------------------------------------//
/**
 * @brief sauvegarder le score et le nom du joueur
 * @param score le temps écoulé
 * @param nomfich nom du fichier
 * @param nomjoueur le nom du joueur
 */
void sauvegarder(int score, char nomfich[], char nomjoueur[])
{
    FILE *fic;
    fic = fopen(nomfich, "a+");
    fprintf(fic, "%s , %d\n", nomjoueur, score);
    fclose(fic);
}

//-----------------------------------Calcul**Score---------------------------------//
void calcul_score(int *score, int direction)
{
    if (direction == 1)
        (*score)++;
    if (direction == 2)
        (*score) = (*score) - 2;
    if (*score <= 0)
        (*score) = 0;
}
//-----------------------------------Afficher**Score-------------------------------//
void afficher_score(int score, SDL_Surface *ecran)
{
    SDL_Surface *sc_surf;
    SDL_Color couleur_blanche = {255, 255, 255};
    SDL_Rect pos_sc;
    char ch_sc[100];
    TTF_Font *police = TTF_OpenFont("COOPBL.ttf", 35);
    pos_sc.x = 20;
    pos_sc.y = 30;
    sprintf(ch_sc, "score :  %d", score);
    sc_surf = TTF_RenderText_Blended(police, ch_sc, couleur_blanche);
    SDL_BlitSurface(sc_surf, NULL, ecran, &pos_sc);
    TTF_CloseFont(police);
}
void afficher_meilleur_joueur(char nomjoueur[], SDL_Surface *ecran)
{

    TTF_Font *police = TTF_OpenFont("COOPBL.ttf", 30);
    SDL_Color couleur_blanche = {255, 255, 255};
    SDL_Rect pos_texte;
    SDL_Surface *texte;
    char ch[100];
    strcpy(ch, "meilleur joueur : ");
    strcat(ch, nomjoueur);
    texte = TTF_RenderText_Blended(police, ch, couleur_blanche);
    pos_texte.x = 20;
    pos_texte.y = 60;
    SDL_BlitSurface(texte, NULL, ecran, &pos_texte);
    TTF_CloseFont(police);
    SDL_FreeSurface(texte);
}

// PERSO.C

void initperso(personne *p)
{
    p->x = 0;
    p->y = 400.0;
    p->posperso.x = 60;
    p->posperso.y = 370;
    p->posperso.h = 100;
    p->posperso.w = 100;
    p->pos_vie.y = 0;
    p->pos_vie.x = 0;
    p->status = STAT_SOL;
    p->vx = p->vy = 0.0f;
    p->vitesse = 5;
    p->acc = 1;
    p->num = 1;
    p->dr = -1;
    p->vie = 3;
    p->hit = 0;
    char nomFich[20];
    int i;
    for (i = 0; i < 20; i++)
    {
        sprintf(nomFich, "a%d.png", i);
        p->tab[i] = IMG_Load(nomFich);
    }
    p->afficher_vie[0] = IMG_Load("vie0.png");
    p->afficher_vie[1] = IMG_Load("vie1.png");
    p->afficher_vie[2] = IMG_Load("vie2.png");
    p->afficher_vie[3] = IMG_Load("vie3.png");
}
void afficherperso(personne *p, SDL_Surface *ecran)
{
    SDL_BlitSurface(p->tab[p->num], NULL, ecran, &(p->posperso));
}
void affichervie(personne p, SDL_Surface *ecran)
{
    SDL_BlitSurface(p.afficher_vie[0], NULL, ecran, &p.pos_vie);
}
void Saute(personne *p, float impulsion)
{
    p->vy = -impulsion;
    p->status = STAT_AIR;
}
void deplacerperso(personne *p)
{
    if (p->dep == 1)
    {
        p->x += ((0.5 * p->acc) + (p->vitesse));
    }
    else if (p->dep == 0)
    {
        p->x -= ((0.5 * p->acc) + (p->vitesse));
    }
}
void controlsol(personne *p)
{
    if (p->posperso.y > 420.0)
    {
        p->posperso.y = 420.0;
        if (p->vy > 0)
            p->vy = 0.0;
        p->status = STAT_SOL;
    }
}
void animerperso(personne *p)
{
    if (p->dr == 0)
    {
        if (p->num >= 5)
            p->num = 1;
        else
            p->num++;
    }
    else if (p->dr == 1)
    {
        if (p->num == 10 || p->num < 6 || p->num > 11)
            p->num = 6;
        else
            p->num++;
    }
    else if (p->dr == 2)
    {
        if (p->num == 13 || p->num < 11 || p->num > 14)
            p->num = 11;
        else
            p->num++;
    }
    else if (p->dr == 3)
    {

        p->num = 15;
    }
    else if (p->dr == 4)
        p->num = 17;
    else
    {
        if (p->num == 15 || p->num < 14 || p->num > 16)
        {
            p->num = 14;
        }
        else
            p->num = 17;
    }
}
void Updateperso(personne *p, Uint8 *keys, joueur *j, int scroll, SDL_Surface *a)
{
    float impulsion = 50.0;
    float factgravite = 0.5;
    float factsautmaintenu = 3.0;
    if (keys[SDLK_RIGHT] == 0 && keys[SDLK_LEFT] == 0 && keys[SDLK_j] == 0 && keys[SDLK_UP] == 0 && keys[SDLK_w] == 0)
    {
        p->dr = -1;
        animerperso(p);
    }
    if (keys[SDLK_RIGHT] == 1)
    {
        p->dep = 1;
        // deplacerperso(p);
        p->dr = 0;
        animerperso(p);
        j->pos_joueur.x += 10;
        p->posperso.x += 10;
    }
    if (keys[SDLK_j])
    {
        p->dr = 2;
        animerperso(p);
    }
    if (keys[SDLK_w])
    {
        p->acc = 20;
        p->dr = 4;
        animerperso(p);
    }

    if (keys[SDLK_LEFT])
    {
        p->dep = 0;
        // deplacerperso(p);
        p->dr = 1;
        animerperso(p);
        j->pos_joueur.x -= 10;
        p->posperso.x -= 10;
    }
    if (keys[SDLK_UP])
    {
        Saute(p, impulsion);
        //  if ( collision_Parfaite(a,*j,0,4) ==1)
        //  {

        // controlsol(p);

        // }
        p->dr = 3;
        animerperso(p);
    }
    p->posperso.y += p->vx;
    p->posperso.y += p->vy;

    j->pos_joueur2 = p->posperso;

    if (collision_Parfaite(a, *j, 0, 4) == 0)
        p->vy = 10.0f;
    if (collision_Parfaite(a, *j, 0, 4) == 1)
        p->vy = 0.0f;

    printf("\n le pos du perso est %d", p->posperso.y);
    if (p->posperso.y >= 1080)
    {
        p->posperso.y = 1080;
    }
    if (p->posperso.x >= (1920 / 2) || (scroll == 1))
    {
        p->posperso.x = 1920 / 2;
    }
}
void updatepersol(personne *p1, Uint8 *keys)
{
    float impulsion = 6.0f;
    float factgravite = 0.5f;
    float factsautmaintenu = 3.0f;
    if (keys[SDLK_d])
    {
        p1->dep = 1;
        deplacerperso(p1);
        p1->dr = 0;
        animerperso(p1);
    }
    if (keys[SDLK_q])
    {
        p1->dep = 0;
        deplacerperso(p1);
        p1->dr = 1;
        animerperso(p1);
    }
    if (keys[SDLK_h])
    {
        p1->dep = 2;
        animerperso(p1);
    }
    if (keys[SDLK_z] && p1->status == STAT_SOL)
        Saute(p1, impulsion);
    controlsol(p1);
    p1->x += p1->vx;
    p1->y += p1->vy;
}
int gestionvie(personne *p, SDL_Surface *ecran)
{
    if ((p->hit == 1) && (p->vie != 0))
    {
        p->vie--;
        affichervie(*p, ecran);
        SDL_Flip(ecran);
    }
    p->hit = 0;
    if (p->vie == 0)
        return 0;
    else
        return 1;
}
void init_attack(attack *atk)
{
    atk->imgattack[0] = IMG_Load("pixil-frame-0.png");
    atk->imgattack[1] = IMG_Load("pixil-frame-1.png");
    atk->imgattack[2] = IMG_Load("pixil-frame-2.png");
    atk->imgattack[3] = IMG_Load("pixil-frame-3.png");
    atk->imgattack[4] = IMG_Load("pixil-frame-4.png");
    atk->imgattack[5] = IMG_Load("pixil-frame-5.png");
    atk->imgattack[6] = IMG_Load("pixil-frame-6.png");
    atk->imgattack[7] = IMG_Load("pixil-frame-7.png");
    atk->imgattack[8] = IMG_Load("pixil-frame-8.png");
    atk->imgattack[9] = IMG_Load("pixil-frame-9.png");
    atk->imgattack[10] = IMG_Load("pixil-frame-10.png");
    atk->imgattack[11] = IMG_Load("pixil-frame-11.png");
    atk->imgattack[12] = IMG_Load("pixil-frame-12.png");
    atk->imgattack[13] = IMG_Load("pixil-frame-13.png");
    atk->imgattack[14] = IMG_Load("pixil-frame-14.png");
    atk->imgattack[15] = IMG_Load("pixil-frame-15.png");
    atk->imgattack[16] = IMG_Load("pixil-frame-16.png");
    atk->imgattack[17] = IMG_Load("pixil-frame-17.png");
    printf("i am here");
}
void affiche_attack(attack atk, personne *p, SDL_Surface *ecran, int i)
{
    SDL_Rect posattack;
    posattack.x = p->posperso.x + 50;
    SDL_BlitSurface(atk.imgattack[i], &p->posperso, ecran, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:
void INITIALISATION()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "Erreur INITIALISATION %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    TTF_Init();

    if (TTF_Init() != 0)
    {
        fprintf(stderr, "Erreur TTF_INIT %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void pos(SDL_Rect *pos, float x, float y)
{
    pos->x = x;
    pos->y = y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void blit(SDL_Surface *what, SDL_Surface *where, SDL_Rect pos)
{
    SDL_BlitSurface(what, NULL, where, &pos);

    // SDL_Flip(where);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void MW(char name[], char icon_loc[])
{
    SDL_WM_SetCaption(name, NULL);
    SDL_WM_SetIcon(IMG_Load(icon_loc), NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void mouse_motion(SDL_Surface *play, SDL_Surface *settings, SDL_Surface *quit, SDL_Rect pos_play, SDL_Rect pos_settings, SDL_Rect pos_quit, SDL_Event event)
{

    Mix_Chunk *select;

    select = Mix_LoadWAV("select.wav");

    // PLAY
    if ((event.motion.x >= pos_play.x) && (event.motion.x <= pos_play.x + play->w) && (event.motion.y >= pos_play.y) && (event.motion.y <= pos_play.y + play->h))
    {
        SDL_FreeSurface(play);
        play = IMG_Load("play2.png");
        Mix_PlayChannel(1, select, 0);
    }
    else
    {
        SDL_FreeSurface(play);
        play = IMG_Load("play.png");
    }
    // SETTINGS
    if ((event.motion.x >= pos_settings.x) && (event.motion.x <= pos_settings.x + play->w) && (event.motion.y >= pos_settings.y) && (event.motion.y <= pos_settings.y + play->h))
    {
        SDL_FreeSurface(settings);
        settings = IMG_Load("settings2.png");
        Mix_PlayChannel(1, select, 0);
    }
    else
    {
        SDL_FreeSurface(settings);
        settings = IMG_Load("settings.png");
    }
    // QUIT
    if ((event.motion.x >= pos_quit.x) && (event.motion.x <= pos_quit.x + play->w) && (event.motion.y >= pos_quit.y) && (event.motion.y <= pos_quit.y + play->h))
    {
        SDL_FreeSurface(quit);
        quit = IMG_Load("quit2.png");
        Mix_PlayChannel(1, select, 0);
    }
    else
    {
        SDL_FreeSurface(quit);
        quit = IMG_Load("quit22.png");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void mouse_button(SDL_Surface *screen, SDL_Surface *background, SDL_Rect pos_back, SDL_Surface *play, SDL_Surface *settings, SDL_Surface *quit, SDL_Rect pos_play, SDL_Rect pos_settings, SDL_Rect pos_quit, SDL_Surface *logo, SDL_Rect pos_logo, int *c, SDL_Event event, SDL_Surface *volume, SDL_Rect pos_volume, int *vol, int *f)
{
    if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= pos_play.x) && (event.motion.x <= pos_play.x + play->w) && (event.motion.y >= pos_play.y) && (event.motion.y <= pos_play.y + play->h))
        play_menu(screen, background, pos_back, play, pos_play, settings, pos_settings, quit, pos_quit, logo, pos_logo);

    else if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= pos_settings.x) && (event.motion.x <= pos_settings.x + play->w) && (event.motion.y >= pos_settings.y) && (event.motion.y <= pos_settings.y + play->h))
        settings_menu(screen, background, pos_back, play, pos_play, settings, pos_settings, quit, pos_quit, logo, pos_logo, volume, pos_volume, vol, f);

    else if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= pos_quit.x) && (event.motion.x <= pos_quit.x + play->w) && (event.motion.y >= pos_quit.y) && (event.motion.y <= pos_quit.y + play->h))
        (*c) = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

/*void affiche_menu_fleches(int i, SDL_Surface *screen, SDL_Surface *back, SDL_Rect pos_back)
{
    switch (i)
    {
    case 1:
        SDL_FreeSurface(back);
        back = IMG_Load("/home/taher/Desktop/atelier/play1.png");
        blit(back, screen, pos_back);
        SDL_Flip(screen);
        break;
    case 2:
        SDL_FreeSurface(back);
        back = IMG_Load("/home/taher/Desktop/atelier/settings1.png");
        blit(back, screen, pos_back);
        SDL_Flip(screen);
        break;
    case 3:
        SDL_FreeSurface(back);
        back = IMG_Load("/home/taher/Desktop/atelier/quit1.png");
        blit(back, screen, pos_back);
        SDL_Flip(screen);
        break;
    }
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void play_menu(SDL_Surface *screen, SDL_Surface *back, SDL_Rect pos_back, SDL_Surface *play, SDL_Rect pos_play, SDL_Surface *settings, SDL_Rect pos_settings, SDL_Surface *quit, SDL_Rect pos_quit, SDL_Surface *logo, SDL_Rect pos_logo)
{
    int c = 1;
    int x = 1;
    int choix;
    int z;

    while (c)
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_MOUSEMOTION:

            if ((event.motion.x >= 265) && (event.motion.x <= 505) && (event.motion.y >= 250) && (event.motion.y <= 470))
            {
                SDL_FreeSurface(back);
                back = IMG_Load("mp.png");
            }
            else if ((event.motion.x >= 800) && (event.motion.x <= 980) && (event.motion.y >= 250) && (event.motion.y <= 470))
            {
                SDL_FreeSurface(back);
                back = IMG_Load("sp.png");
            }
            else
            {
                SDL_FreeSurface(back);
                back = IMG_Load("single_mult.png");
            }

            break;

        case SDL_MOUSEBUTTONUP:

            if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 265) && (event.motion.x <= 505) && (event.motion.y >= 250) && (event.motion.y <= 470))
            {
                choix = 1;
            }
            else if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 800) && (event.motion.x <= 980) && (event.motion.y >= 250) && (event.motion.y <= 470))
            {
                choix = 2;
            }
            else
                choix = 0;

            if (choix)
            {
                while (x)
                {
                    SDL_Surface *back2 = NULL;
                    SDL_Rect pos;
                    pos.x = 0;
                    pos.y = 0;
                    SDL_Event event2;
                    SDL_WaitEvent(&event2);
                    switch (event2.type)
                    {

                    case SDL_MOUSEMOTION:

                        screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);

                        if ((event2.motion.x >= 70) && (event2.motion.x <= 484) && (event2.motion.y >= 280) && (event2.motion.y <= 440))
                        {
                            SDL_FreeSurface(back2);
                            back2 = IMG_Load("ng.png");
                        }
                        else if ((event2.motion.x >= 803) && (event2.motion.x <= 1212) && (event2.motion.y >= 280) && (event2.motion.y <= 440))
                        {
                            SDL_FreeSurface(back2);
                            back2 = IMG_Load("contin.png");
                        }
                        else
                        {
                            SDL_FreeSurface(back2);
                            back2 = IMG_Load("play4.png");
                        }

                        break;

                    case SDL_MOUSEBUTTONUP:

                        screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);

                        switch (choix)
                        {
                        case 2:

                            z = singleplayer();

                            break;

                        case 1:

                            z = multiplayer();

                            break;
                        }

                    case SDL_KEYDOWN:
                        switch (event2.key.keysym.sym)
                        {

                        case SDLK_q:

                            x = 0;

                            break;
                        }
                    }

                    blit(back2, screen, pos);
                    SDL_Flip(screen);

                    SDL_FreeSurface(back2);
                }
            }

            break;

        case SDL_KEYDOWN:
            SDL_Delay(100); // <== VERY IMPORTANT HERE
            switch (event.key.keysym.sym)
            {
            case SDLK_q:
                SDL_FreeSurface(back);
                back = IMG_Load("background.jpg");
                blit(back, screen, pos_back);
                blit(play, screen, pos_play);
                blit(settings, screen, pos_settings);
                blit(quit, screen, pos_quit);
                blit(logo, screen, pos_logo);
                c = 0;
                break;
            }
            break;
        }

        blit(back, screen, pos_back);
        SDL_Flip(screen);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////:

void settings_menu(SDL_Surface *screen, SDL_Surface *back, SDL_Rect pos_back, SDL_Surface *play, SDL_Rect pos_play, SDL_Surface *settings, SDL_Rect pos_settings, SDL_Surface *quit, SDL_Rect pos_quit, SDL_Surface *logo, SDL_Rect pos_logo, SDL_Surface *volume, SDL_Rect pos_volume, int *vol, int *f)
{
    int c = 1;

    while (c)
    {
        switch ((*vol))
        {
        case 1:
            SDL_FreeSurface(back);
            back = IMG_Load("settings3.png");
            SDL_FreeSurface(volume);
            volume = IMG_Load("0.png");
            Mix_VolumeMusic(0);
            break;

        case 2:
            SDL_FreeSurface(back);
            back = IMG_Load("settings3.png");
            SDL_FreeSurface(volume);
            volume = IMG_Load("25.png");
            Mix_VolumeMusic(25);
            break;

        case 3:
            SDL_FreeSurface(back);
            back = IMG_Load("settings3.png");
            SDL_FreeSurface(volume);
            volume = IMG_Load("50.png");
            Mix_VolumeMusic(50);
            break;

        case 4:
            SDL_FreeSurface(back);
            back = IMG_Load("settings3.png");
            SDL_FreeSurface(volume);
            volume = IMG_Load("75.png");
            Mix_VolumeMusic(75);
            break;

        case 5:
            SDL_FreeSurface(back);
            back = IMG_Load("settings3.png");
            SDL_FreeSurface(volume);
            volume = IMG_Load("100.png");
            Mix_VolumeMusic(100);
            break;
        }

        blit(back, screen, pos_back);
        blit(volume, screen, pos_volume);
        SDL_Flip(screen);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_MOUSEBUTTONDOWN:

            if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 750.43) && (event.motion.x <= 825.55) && (event.motion.y >= 282.22) && (event.motion.y <= 357.33))
            {
                if ((*vol) < 5)
                    (*vol)++;
            }

            if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 454.45) && (event.motion.x <= 529.57) && (event.motion.y >= 282.22) && (event.motion.y <= 357.33))
            {
                if ((*vol) > 1)
                    (*vol)--;
            }

            if ((event.button.button == SDL_BUTTON_LEFT) && (event.motion.x >= 594.49) && (event.motion.x <= 685.52) && (event.motion.y >= 494.18) && (event.motion.y <= 585.2))
            {
                SDL_FreeSurface(back);
                back = IMG_Load("background.jpg");
                blit(back, screen, pos_back);
                blit(play, screen, pos_play);
                blit(settings, screen, pos_settings);
                blit(quit, screen, pos_quit);
                blit(logo, screen, pos_logo);
                c = 0;
            }
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_f:
                if (*f == 1)
                    (*f) = 0;
                else
                    (*f) = 1;
                switch (*f)
                {
                case 0:
                    screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);
                    break;

                case 1:
                    screen = SDL_SetVideoMode(1920, 1080, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);
                    break;
                }

                break;

            case SDLK_q:
                SDL_FreeSurface(back);
                back = IMG_Load("background.jpg");
                blit(back, screen, pos_back);
                blit(play, screen, pos_play);
                blit(settings, screen, pos_settings);
                blit(quit, screen, pos_quit);
                blit(logo, screen, pos_logo);
                c = 0;
                break;
            }
            break;
        }
    }
}

int singleplayer()
{
    // initialiser  les variables
    SDL_Event event;
    Mix_Music *son;
    SDL_Surface *ecran = NULL, *perso = NULL, *Masque, *vie;
    int direction, continuer = 1, k = 0, i = 0, s = 0, animation_attack = 0, decalage_animation_attack = 0;
    int kkh = 0, kkm = 0, kks = 0, score = 0, mell_sc, dam = 3;
    int scroll = 0;
    int z;
    Background b, night;
    attack atk;
    Ennemi E;
    Ennemi2 E2;
    SDL_Rect posperso, posperso2, pos_vie;
    pos_vie.x = 0;
    pos_vie.y = 0;
    minimap m;
    joueur j;
    temps t;
    personne per;
    int numkeys;
    Uint8 *keys;
    char nomfich[10], nomjou[10], mell_jou[] = {"/0"};
    posperso.x = 900;
    posperso.y = 300;

    j.pos_joueur2.x = 370;
    posperso2 = posperso;
    j.pos_joueur2.x = 60;
    j.pos_joueur2.y = 370;
    j.pos_joueur = j.pos_joueur2;
    vie = IMG_Load("vie0.png");
    j.bon = IMG_Load("a1.png");
    Masque = IMG_Load("masque1080.png");
    init_attack(&atk);

    initBack(&b);
    initimg(&b);
    initminimap(&m);
    initperso(&per);
    initEnnemi(&E);
    initEnnemi2(&E2);
    TTF_Init();
    per.afficher_vie[0] = IMG_Load("vie0.png");
    per.afficher_vie[1] = IMG_Load("vie1.png");
    per.afficher_vie[2] = IMG_Load("vie2.png");
    per.afficher_vie[3] = IMG_Load("vie3.png");
    //  initnightmap(&night);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
    {
        printf("unable to set video mode:%s \n", SDL_GetError());
        return 1;
    }
    SDL_WM_SetIcon(IMG_Load("icone game.png"), NULL);
    ecran = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    if (ecran == NULL)
    {
        printf("unable to set video mode:%s \n", SDL_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) // Initialisation de l'API Mixer
    {
        printf("%s", Mix_GetError());
    }
    son = Mix_LoadMUS("a.mp3");
    Mix_PlayMusic(son, -1);
    Mix_VolumeMusic(5);
    // SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur);
    MAJMinimap(j.pos_joueur, &m, 0, ecran);
    afficherminimap(m, ecran);

    strcpy(nomfich, "AAA.txt");
    afficher_score(score, ecran);
    meilleur_joueur("BB.txt", mell_sc, mell_jou);
    nom(nomfich, ecran, nomjou);

    // SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur);
    // MAJMinimap(j.pos_joueur, &m, 0, ecran);
    // afficherminimap(m, ecran);
    initialiser_temps(&t);

    SDL_EnableKeyRepeat(11, 11);
    // BOUCLE PRINCIPAL
    while (continuer)
    {
        if (s >= 25)
            s = 0;
        s++;
        if (s == 25)
        {

            if (i >= 6)
            {
                i = 0;
            }
            i++;
        }

        afficherBack(b, ecran, i);
        afficher_meilleur_joueur(mell_jou, ecran);
        // afficherBack(night, ecran,i);
        // SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur2);

        //  afficherminimap(m, ecran);
        afficher_score(score, ecran);
        afficher_temps(&t, ecran);
        deplacerIA(&E, j.pos_joueur2);
        deplacer2(&E2);
        animerEnnemi(&E);
        animerEnnemi2(&E2);
        afficherEnnemi(E, ecran);
        afficherEnnemi2(E2, ecran);
        afficherperso(&per, ecran);
        affichervie(per, ecran);
        affiche_attack(atk, &per, ecran, animation_attack);
        // MAJMinimap(j.pos_joueur, &m, 0, ecran);

        keys = SDL_GetKeyState(&numkeys);
        if (keys[SDLK_j] == 1)
        {
            if (decalage_animation_attack >= 3)
                decalage_animation_attack = 0;
            decalage_animation_attack++;
            if (decalage_animation_attack == 3)
            {

                if (animation_attack >= 17)
                {
                    animation_attack = 0;
                }
                animation_attack++;
            }
            posperso = per.posperso;
            posperso.y = per.posperso.y - 80;
            posperso.x = per.posperso.x + 100;
            SDL_BlitSurface(atk.imgattack[animation_attack], NULL, ecran, &posperso);
        }
        if (keys[SDLK_LEFT] == 1)
        {
            direction = 2;

            k = collision_Parfaite(Masque, j, 0, direction);

            if (k == 0)
            {

                //  j.pos_joueur.x -= 10;
                calcul_score(&score, direction);

                if ((b.scroll.x >= 50 && b.scroll.x < (b.scroll.w - ecran->w)) || (b.scroll.x == (b.scroll.w - ecran->w) && per.posperso.x <= 760))
                {
                    scroll = scrollingBack(&b, j.pos_joueur2, direction);
                    E.pos.x += 10;
                    E2.pos2.x += 10;
                }
                else if (b.scroll.x < 50 || b.scroll.x >= (b.scroll.w - ecran->w))
                {
                    scroll = 0;
                    //  j.pos_joueur2.x -= 10;

                    printf("dep left poshero %d abcisse cam : %d\n", posperso.x, b.scroll.x);
                }
            }
        }
        if (keys[SDLK_RIGHT] == 1)
        {
            direction = 1;
            k = collision_Parfaite(Masque, j, 0, direction);
            if (k == 0)
            {
                printf("k= %d \n", k);

                // j.pos_joueur.x += 10;
                calcul_score(&score, direction);

                if (j.pos_joueur2.x < (ecran->w / 2) || b.scroll.x >= (b.scroll.w - ecran->w))
                {
                    // j.pos_joueur2.x += 10;
                    scroll = 0;

                    printf("dep right poshero %d  abcisse cam %d\n", j.pos_joueur2.x, b.scroll.x);
                }
                else if (j.pos_joueur2.x >= (ecran->w / 2))
                {
                    scroll = scrollingBack(&b, j.pos_joueur2, direction);
                    E.pos.x -= 10;
                    E2.pos2.x -= 10;
                }
                if (j.pos_joueur2.x >= (ecran->w / 2))

                {
                    j.pos_joueur2.x = (ecran->w / 2);
                }
            }
        }
        if (k == 0)
        {
            Updateperso(&per, keys, &j, scroll, Masque);
        }
        j.pos_joueur2 = per.posperso;
        j.pos_joueur.y = per.posperso.y;
        if (collision(E.pos, j.pos_joueur) == 0)
        {
            SDL_BlitSurface(per.afficher_vie[dam], NULL, ecran, &pos_vie);

            printf("collision\n");
            //  SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur2);
        }

        if (collision(E.pos, j.pos_joueur) == 1)
        {
            dam = -1;
            SDL_BlitSurface(per.afficher_vie[dam], NULL, ecran, &pos_vie);
            if (dam <= 0)
            {
                dam = 0;
            }

            printf("collision\n");
            //  SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur2);
        }
        if (collision(E.pos, posperso) == 1 && animation_attack == 12)
        {

            E.sprite = NULL;
        }
        // gestionvie(&per, ecran);

        if (collision2(E2.pos2, j.pos_joueur) == 0)
        {
            SDL_BlitSurface(per.afficher_vie[dam], NULL, ecran, &pos_vie);

            printf("collision2\n");
            //  SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur2);
        }

        if (collision2(E2.pos2, j.pos_joueur) == 1)
        {
            dam = -1;
            SDL_BlitSurface(per.afficher_vie[dam], NULL, ecran, &pos_vie);
            if (dam <= 0)
            {
                dam = 0;
            }
            printf("collision2\n");
            //  SDL_BlitSurface(j.bon, NULL, ecran, &j.pos_joueur2);
        }
        if (collision2(E2.pos2, posperso) == 1 && animation_attack == 12)
        {

            E2.sprite2 = NULL;
        }
        if (dam <= 0)
        {
            dam = 3;
        }
        while (SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_QUIT:
                continuer = 0;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {

                case SDLK_ESCAPE:
                    continuer = 0;
                    break;
                    /*  case SDLK_LEFT:
                        direction = 2;

                        k = collision_Parfaite(Masque, j, 0, direction);

                        if (k == 0)
                        {

                          //  j.pos_joueur.x -= 10;
                          calcul_score(&score, direction);

                          if ((b.scroll.x >= 50 && b.scroll.x < (b.scroll.w - ecran->w)) || (b.scroll.x == (b.scroll.w - ecran->w) && per.posperso.x <= 760))
                          {
                            scroll = scrollingBack(&b, j.pos_joueur2, direction);
                            E.pos.x += 10;
                            E2.pos2.x += 10;
                          }
                          else if (b.scroll.x < 50 || b.scroll.x >= (b.scroll.w - ecran->w))
                          {
                            scroll = 0;
                            //  j.pos_joueur2.x -= 10;

                            printf("dep left poshero %d abcisse cam : %d\n", posperso.x, b.scroll.x);
                          }
                        }
                        break;*/
                case SDLK_UP:
                    direction = 3;
                    k = collision_Parfaite(Masque, j, 0, direction);
                    if (k == 0)
                    {

                        printf("k= %d \n", k);
                        // j.pos_joueur.y -= 10;
                        if ((j.pos_joueur2.y < 617) && (j.pos_joueur2.y > 0))
                        {
                            printf("poshero %d\n", posperso.y);
                            printf("pos cam %d\n", b.scroll.y);
                            j.pos_joueur2.y -= 10;
                        }
                        else
                        {
                            printf("poshero %d\n", posperso.y);
                            printf("pos cam %d\n", b.scroll.y);
                            scroll = scrollingBack(&b, posperso, direction);
                        }
                    }
                    break;
                case SDLK_DOWN:
                    direction = 4;

                    if (k == 0)
                    {

                        printf("k= %d \n", k);
                        // j.pos_joueur.y += 10;
                        if ((j.pos_joueur2.y < 617) && (j.pos_joueur2.y >= 0))
                        {
                            printf("poshero %d\n", posperso.y);
                            printf("pos cam %d\n", b.scroll.y);
                            j.pos_joueur2.y += 10;
                        }
                        else
                        {
                            printf("poshero %d\n", posperso.y);
                            printf("pos cam %d\n", b.scroll.y);
                            scroll = scrollingBack(&b, posperso, direction);
                        }
                    }
                    break;
                }
            }
        }
        k = collision_Parfaite(Masque, j, 0, direction);
        SDL_Flip(ecran);

        if (j.pos_joueur.x == 1500)
        {
            z = jeu_enigme(&score);
            j.pos_joueur.x = 1501;
            ecran = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
        }

        else if (j.pos_joueur.x == 3000)
        {
            z = jeu_enigme(&score);
            j.pos_joueur.x = 3001;
            ecran = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
        }
    }
    sauvegarder(score, "BB.txt", nomjou);
    SDL_FreeSurface(ecran);
    SDL_FreeSurface(j.bon);
    SDL_FreeSurface(b.imgBack1[0]);
    SDL_FreeSurface(b.imgBack1[1]);
    SDL_FreeSurface(b.imgBack1[2]);
    SDL_FreeSurface(b.imgBack1[3]);
    SDL_FreeSurface(b.imgBack1[4]);
    SDL_FreeSurface(b.imgBack1[5]);
    SDL_FreeSurface(b.imgBack1[6]);
    SDL_FreeSurface(b.imgBack1[7]);
    return 1;
}

int multiplayer()
{
    return 1;
}