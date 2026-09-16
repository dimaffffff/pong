#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define WINDOW_HEIGHT 1000
#define WINDOW_WIDTH 1000

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

struct colour{
    int r,g,b;
};
//Not really a ball, more like a square
struct pongBall{
    int x,y,size;
    struct colour colour;
};

struct playerPad{
    int x,y,height,width;
    struct colour colour;
};

int init(void){
    int padWidth = 30;
    struct pongBall ball = {.x = WINDOW_WIDTH/2, .y = WINDOW_HEIGHT,.size = 50};
    struct playerPad p1Pad = {.x = 0, .y = WINDOW_HEIGHT/2,.height=WINDOW_HEIGHT/2,.width=padWidth};
    struct playerPad p2Pad = {.x = WINDOW_WIDTH-padWidth, .y = WINDOW_HEIGHT/2,.height=WINDOW_HEIGHT/2,.width=padWidth};

    SDL_SetAppMetadata("pong","1.0","com.game.pong");

    if (!SDL_Init(SDL_INIT_VIDEO)){
        printf("CRIT: couldn`t initialise SDL!\n");
        return 0;
    }

    if (!SDL_CreateWindowAndRenderer("pong",WINDOW_WIDTH,WINDOW_HEIGHT,0,&window,&renderer)){
        printf("CRIT: couldn't create a window/renderer: %s\n", SDL_GetError());
        return 0;
    }

    SDL_SetRenderLogicalPresentation(renderer,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_LOGICAL_PRESENTATION_LETTERBOX);
    return 1;
}

char processEvents(void){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_EVENT_QUIT:
                printf("INFO: got quit event\n");
                return 0;
                break;
            default:
                break;
        }
    }
    return 1;
}

char mainLoop(void){
    const double time = ((double)SDL_GetTicks())/1000.0;

    SDL_SetRenderDrawColorFloat(renderer,0,0,0,SDL_ALPHA_OPAQUE);

    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);

    return 0; // NOTE: This is the only place where 0 is SUCCESS (except exit statuses of course)
}

void onQuit(void){
    //clean up
}

int main(void){
    printf("INFO: entrypoint reached!\n");
    if (!init()) return 1;
    printf("INFO: initialisation completed successfully\n");
    char running = 1;
    char exception = 0;
    while (running && !exception){
        running = processEvents();
        exception = mainLoop();
    }
    onQuit();
    return exception;
}

