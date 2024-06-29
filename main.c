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

int main(int argc, char argv[])
{
    // INITIALISATION

    INITIALISATION();

    // Declaration variables

    int c = 1, i = 1, v = 50, vol = 3, a = 1, f = 0, t;

    SDL_Surface *screen = NULL, *background = NULL, *names = NULL, *text = NULL, *boy = NULL, *play = NULL, *settings = NULL, *quit = NULL, *logo = NULL, *volume = NULL;

    // Image Load

    background = IMG_Load("background.jpg");
    boy = IMG_Load("boy_front.png");
    play = IMG_Load("play.png");
    settings = IMG_Load("settings.png");
    quit = IMG_Load("quit22.png");
    logo = IMG_Load("logo.png");
    volume = IMG_Load("50.png");

    // POSITIONS

    SDL_Rect pos_screen, pos_back, pos_text, pos_boy, pos_play, pos_settings, pos_quit, pos_logo, pos_volume, pos_names;

    pos(&pos_screen, 0, 0);
    pos(&pos_back, 0, 0);
    pos(&pos_text, width / 30, 0);
    pos(&pos_names, width / 30, 660);
    pos(&pos_boy, width / 2, height / 2);
    pos(&pos_play, 186.5 - (225 / 2), 135.5 - (225 / 2));
    pos(&pos_settings, 186.5 - (225 / 2), 360.5 - (225 / 2));
    pos(&pos_quit, 186.5 - (225 / 2), 585.5 - (225 / 2));
    pos(&pos_logo, 649.2647 - (438.5294 / 2), 324.5 - (375 / 2));
    pos(&pos_volume, 640 - (371 / 2), 320.84 - (75 / 2));

    // MW_CAPTION and WM_SETICON

    MW("MENU V2", "icon.jpg");

    // TEXT

    TTF_Font *font_text = TTF_OpenFont("sketch.ttf", 55);
    TTF_Font *font_names = TTF_OpenFont("Retro Gaming.ttf", 50);

    SDL_Color textcolor = {255, 0, 0};
    SDL_Color namescolor = {10, 255, 20};

    text = TTF_RenderText_Blended(font_text, "Made with Passion by ARS Coding", textcolor);
    names = TTF_RenderText_Blended(font_names, "Taher | Yassine | Amine |Sofiene | Nader | Wahib", namescolor);

    // SON

    Mix_Music *music;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
    {
        fprintf(stderr, "Erreur son = %s", Mix_GetError());
    }
    music = Mix_LoadMUS("aoud.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(v);

    // SET VIDEO MODE

    screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);

    if (screen == NULL)
    {
        fprintf(stderr, "Erreur Video %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // BLITTER LES IMAGES

    blit(background, screen, pos_back);
    blit(play, screen, pos_play);
    blit(settings, screen, pos_settings);
    blit(quit, screen, pos_quit);
    blit(logo, screen, pos_logo);

    SDL_Flip(screen);

    while (c)
    {
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type)
        {

        case SDL_QUIT:
            c = 0;
            break;

        case SDL_MOUSEMOTION:
            mouse_motion(play, settings, quit, pos_play, pos_settings, pos_quit, event);

            break;

        case SDL_MOUSEBUTTONUP:
            mouse_button(screen, background, pos_back, play, settings, quit, pos_play, pos_settings, pos_quit, logo, pos_logo, &c, event, volume, pos_volume, &vol, &f);
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {

            case SDLK_t:

                t = tiktak();
                screen = SDL_SetVideoMode(width, height, 32, SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_HWSURFACE);

                break;

            case SDLK_ESCAPE:
                c = 0;
                break;
            }
            break;
        }
        // BLITTER LES IMAGES

        blit(background, screen, pos_back);
        blit(logo, screen, pos_logo);
        blit(play, screen, pos_play);
        blit(settings, screen, pos_settings);
        blit(quit, screen, pos_quit);

        if (pos_text.x < 1250)
        {
            pos_text.x += 5;
            blit(text, screen, pos_text);
        }
        else
            pos_text.x = -1250;

        if (pos_names.x > -1300)
        {
            pos_names.x -= 5;
            blit(names, screen, pos_names);
            SDL_Flip(screen);
        }
        else
            pos_names.x = 1300;
    }

    SDL_FreeSurface(background);
    SDL_FreeSurface(logo);
    SDL_FreeSurface(play);
    SDL_FreeSurface(settings);
    SDL_FreeSurface(quit);
    Mix_FreeMusic(music);
    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}