#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "SDL.h"


typedef struct{
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

//SDL Configuration
typedef struct{
    uint32_t window_width;  //SDL Window Width
    uint32_t window_height; //SDL Window Height
    uint32_t fg_color; //Foreground Color RGBA=8888
    uint32_t bg_color; //Background Color RGBA=8888
    uint32_t scale_factor; //Amount to scale a chip8 window by. For ex, let us make each pixel 20x larger
} config_t;

//Emulator States
typedef enum{
    QUIT,
    RUNNING,
    PAUSED,
}emulator_state_t;

//Chip8 Machine Object
typedef struct{
    emulator_state_t state;
}chip8_t;

bool init_sdl(sdl_t *sdl, const config_t config){
    //initialise sdl
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0){
        SDL_Log("Could not initialise SDL subsystems %s\n",SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                                    config.window_width * config.scale_factor , 
                                                    config.window_height * config.scale_factor, 
                                                    0);
    if(!sdl->window){
        SDL_Log("Could not create SDL window %s\n",SDL_GetError());
        return false;    //window failure
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer){
        SDL_Log("Could not create SDL renderer %s\n",SDL_GetError());
        return false;    //Renderer failure
    }

    return true; //success
}

//setup emulator config from passed in arguments
bool set_config_from_args(config_t *config, const int argc, char **argv){
    // Set defaults
    *config = (config_t){
        .window_width = 64, //chip8 original x resolution
        .window_height = 32, //chip8 original y resolution
        .fg_color = 0xFFFFFFFF, //WHITE
        .bg_color = 0xFFFF00FF, //YELLOW
        .scale_factor = 20, //Default resolution will be 1280x640
    };

    //Override defaults from passed in arguments
    for(int i=1; i<argc; i++){
        (void)argv[i]; //prevent compiler errors from unused variables 
        //...
    }
    return true;
}

//Initialise CHIP8
bool init_chip8(chip8_t *chip8){
    chip8->state = RUNNING; //Default State is Running
    return true; // Initialisation of chip8 is success
}

//final program
void final_cleanup(const sdl_t sdl){
    //end program
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
}

//CLEAR SCREEN
void clear_screen(const sdl_t sdl, const config_t config){
    //Clear Screen / SDL Window to Background Color
    const uint8_t r = (config.bg_color >> 24) & 0xFF;
    const uint8_t g = (config.bg_color >> 16) & 0xFF;
    const uint8_t b = (config.bg_color >>  8) & 0xFF;
    const uint8_t a = (config.bg_color >>  0) & 0xFF;
    SDL_SetRenderDrawColor(sdl.renderer,r,g,b,a);
    SDL_RenderClear(sdl.renderer);
}

//Update the Screen
void update_screen(const sdl_t sdl){
    SDL_RenderPresent(sdl.renderer);
}

void handle_input(chip8_t *chip8){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
                //End Program and Quit
                chip8->state = QUIT; //Will change the state of the sdl, causing end of main emulator loop
                return;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        //Esc key will exit window and stop the program
                        chip8->state = QUIT;
                        return;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                break;
            default:
                break;
        }
    }
}

//The Main Program
int main(int argc, char **argv){
    (void) argc;
    (void) argv;

    //initialise emulator options and cofigurations
    config_t config={0};
    if (!set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);

    //initialise sdl
    sdl_t sdl ={0};
    if ((!init_sdl(&sdl, config))) exit(EXIT_FAILURE);

    //Initialise chip8 Machine
    chip8_t chip8={0};
    if (!init_chip8(&chip8)) exit(EXIT_FAILURE);

    //Initial Clear Screen
    clear_screen(sdl, config);

    //Main Emulator Loop, For FPS,etc
    while(chip8.state != QUIT){
        //Handle User Input
        handle_input(&chip8);

        //for paused state we can do=> if (chip8.state == PAUSED) continue;

        //Get_Time()
        //Emulate Chip8 Instructions
        //Get_Time() elapsed since last get_time()
        //Delay for (approximately)60hz/60fps<16.667ms>
        //SDL_Delay(16 - actual_time_elapsed);
        SDL_Delay(16);
        //Update Window with Changes
        update_screen(sdl);
    }

    //end program
    final_cleanup(sdl);

    exit(EXIT_SUCCESS);
}
