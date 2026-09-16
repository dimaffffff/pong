#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define WINDOW_HEIGHT 1500
#define WINDOW_WIDTH 3000
#define PADSPEED 5 // WARN: Yes, it is kinda bad to define this but I am way too lazy to do this properly
#define INITBALLSPEEDX 5
#define INITBALLSPEEDY 5

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

typedef struct colour{
    unsigned char r,g,b;
} colour;

typedef struct vector2{
    int x,y;
} vector;
//Not really a ball, more like a square
struct pongBall{
    vector pos,speed;
    int size;
    colour colour;
};

struct playerPad{
    vector size,pos;
    colour colour;
};

struct input{
    char state;
    SDL_Keycode key;
};

struct inputState{
    char length;
    struct input* array;
};

struct state{
    struct pongBall ball;
    struct playerPad p1Pad,p2Pad;
    struct inputState inputs; 
};

void drawRect(vector pos, vector size){
    SDL_FRect rect = {.x = pos.x, .y = pos.y, .w = size.x, .h = size.y};
    SDL_RenderRect(renderer,&rect);
    SDL_RenderFillRect(renderer,&rect);
}

void drawPad(struct playerPad padData){
    SDL_SetRenderDrawColor(renderer,padData.colour.r,padData.colour.g,padData.colour.b,SDL_ALPHA_OPAQUE);
    drawRect(padData.pos,padData.size);
}

void drawBall(struct pongBall ballData){
    SDL_SetRenderDrawColor(renderer,ballData.colour.r,ballData.colour.g,ballData.colour.b,SDL_ALPHA_OPAQUE);
    vector rectSize = {ballData.size,ballData.size};
    drawRect(ballData.pos,rectSize);
}

void setKeyState(struct inputState* inputState,char state,SDL_Keycode key){
    if (inputState->length == 0) return ;
    char count = 0;
    do{
        if ((inputState->array[count]).key == key){
            (inputState->array[count]).state = state;
        }
    }while(count++<inputState->length);
}

char getKeyState(struct inputState* inputState,SDL_Keycode key){
    if (inputState->length == 0) return 0;
    char count = 0;
    do{
        if ((inputState->array[count]).key == key){
            return (inputState->array[count]).state;
        }
    }while(count++<inputState->length);
    return 0;
}

void movePad(struct playerPad* pad, int movement){
    int newYPos = pad->pos.y + movement;
    if (WINDOW_HEIGHT - pad->size.y > newYPos && newYPos > 0){
        pad->pos.y = newYPos;
    }
}

char padCollisionCheck(struct playerPad pad,vector pos){
    if(pos.x>pad.pos.x && pos.x < (pad.pos.x+pad.size.x) && pos.y > pad.pos.y && pos.y < (pad.pos.y + pad.size.y)){
        return 1;
    }
    return 0;
}

char ballCollisionCheck(struct playerPad pad, struct pongBall ball){
    vector leftDownCorner = {.x = ball.pos.x,.y = (ball.pos.y + ball.size)};
    vector rightDownCorner = {.x = (ball.pos.x + ball.size),.y = (ball.pos.y + ball.size)};
    vector rightUpCorner = {.x = (ball.pos.x + ball.size),.y = ball.pos.y};
    if (padCollisionCheck(pad,ball.pos) || padCollisionCheck(pad,leftDownCorner) || padCollisionCheck(pad,rightDownCorner) || padCollisionCheck(pad,rightUpCorner)){
       return 1; 
    }
    return 0;
}

void ballMovement(struct state* gameState){
    vector newPos = {.x = gameState->ball.pos.x + gameState->ball.speed.x,.y = gameState->ball.pos.y + gameState->ball.speed.y};
    //Y movement
    if (newPos.y < WINDOW_HEIGHT && newPos.y > 0){
        gameState->ball.pos.y = newPos.y;
    }else{
        gameState->ball.speed.y *= -1;
    }
    //X movement
    if (newPos.x < WINDOW_WIDTH && newPos.x > 0){
        if (ballCollisionCheck(gameState->p1Pad,gameState->ball) || ballCollisionCheck(gameState->p2Pad,gameState->ball)){
            gameState->ball.speed.x *= -1;
        }
        gameState->ball.pos.x += gameState->ball.speed.x;
    }else{
        gameState->ball.pos.x = (WINDOW_WIDTH/2) - gameState->ball.size;
        gameState->ball.pos.y = (WINDOW_HEIGHT/2) - gameState->ball.size;
    }
}

int init(void){

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

char processEvents(struct state* gameState){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_EVENT_QUIT:
                printf("INFO: got quit event\n");
                return 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                setKeyState(&(gameState->inputs),1,event.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                setKeyState(&(gameState->inputs),0,event.key.key);
                break;
            default:
                break;
        }
    }
    return 1;
}

void inputProcessing(struct state* gameState){
    struct inputState inputState = gameState->inputs;
    if (inputState.length == 0) return ;
    char count = 0;
    do{
        if ((inputState.array[count]).state == 1){
            switch ((inputState.array[count]).key){
                case SDLK_A:
                    movePad(&(gameState->p1Pad),-PADSPEED);
                    break;
                case SDLK_S:
                    movePad(&(gameState->p1Pad), PADSPEED);
                    break;
                case SDLK_J:
                    movePad(&(gameState->p2Pad),-PADSPEED);
                    break;
                case SDLK_K:
                    movePad(&(gameState->p2Pad), PADSPEED);
                    break;
            }
        }
    }while(count++<inputState.length);
}

char mainLoop(struct state* gameState){

    SDL_SetRenderDrawColorFloat(renderer,0,0,0,SDL_ALPHA_OPAQUE);

    SDL_RenderClear(renderer);
    
    inputProcessing(gameState);
    ballMovement(gameState);

    drawPad(gameState->p1Pad);
    drawPad(gameState->p2Pad);
    drawBall(gameState->ball);

    SDL_RenderPresent(renderer);

    return 0; // NOTE: This is the only place where 0 is SUCCESS (except exit statuses of course)
}

void onQuit(struct state* gameState){
    //clean up
    
}

int main(void){
    printf("INFO: entrypoint reached!\n");
    if (!init()) return 1;
    //initial state of the game
    char running = 1;
    char exception = 0;
    char ballSize = 50;
    vector padSize = {.x =30,.y=(int)WINDOW_HEIGHT/6};
    colour gameColour = {255,255,255};
    struct input inputs[] = {{.key = SDLK_A},{.key = SDLK_S},{.key = SDLK_J},{.key = SDLK_K}};
    struct state gameState = {
        .ball = {.pos = {.x = (WINDOW_WIDTH/2)-ballSize,.y = (WINDOW_HEIGHT/2)-ballSize},.size=ballSize,.colour = gameColour,.speed = {.x=INITBALLSPEEDX,.y=INITBALLSPEEDY}},
        .p1Pad = {.size=padSize,.pos={.x=0,.y=(WINDOW_HEIGHT/2)-(padSize.y/2)},.colour = gameColour},
        .p2Pad = {.size=padSize,.pos={.x=WINDOW_WIDTH-padSize.x,.y=(WINDOW_HEIGHT/2)-(padSize.y/2)},.colour = gameColour},
        .inputs = {.length = sizeof(inputs)/sizeof(struct input),.array = inputs}
    };
    printf("INFO: initialisation completed successfully\n");
    while (running && !exception){
        running = processEvents(&gameState);
        exception = mainLoop(&gameState);
    }
    onQuit(&gameState);
    return exception;
}

