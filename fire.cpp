using namespace std;
#include<iostream>
#include<SDL.h>
#include<vector>

const int FIRE_WIDTH = 300;
const int FIRE_HEIGHT = 300;
const int SCREEN_SCALE = 3; 

// Iconic 37-color palette from the PSX Doom fire effect
const uint32_t firePalette[37] = {
    0x070707FF, 0x1F0707FF, 0x2F0F07FF, 0x470F07FF, 0x571707FF, 0x671F07FF, 0x771F07FF,
    0x8F2707FF, 0x9F2F07FF, 0xAF3F07FF, 0xBF4707FF, 0xC74707FF, 0xDF4F07FF, 0xDF5707FF,
    0xDF5707FF, 0xD75F07FF, 0xD75F07FF, 0xD7670FFF, 0xCF6F0FFF, 0xCF770FFF, 0xCF7F0FFF,
    0xCF8717FF, 0xC78717FF, 0xC78F17FF, 0xC7971FFF, 0xBF9F1FFF, 0xBF9F1FFF, 0xBFA727FF,
    0xBFA727FF, 0xBFAF2FFF, 0xB7AF2FFF, 0xB7B72FFF, 0xB7B737FF, 0xCFCF6FFF, 0xDFDF9FFF,
    0xEFEFC7FF, 0xFFFFFFFF
};


vector<vector<int>> firePixels(FIRE_HEIGHT, vector<int>(FIRE_WIDTH, 0));



void spreadFire(int row, int col){

    int pixel = firePixels[row][col];
    if(pixel == 0){
        firePixels[row- 1][col] = 0;
    }
    else{
        int randIdx = rand() %3;
        int decay = randIdx & 1;

        int new_col = col - randIdx + 1;

        if(new_col >=0 && new_col < FIRE_WIDTH){
            firePixels[row - 1][new_col] = pixel - decay;
        }
    }
}

void fire(){
    for(int x = 1; x < FIRE_HEIGHT; ++x){
        for(int y = 0; y< FIRE_WIDTH; ++y){
            spreadFire(x, y);
        }
    }
}


int main(int argc, char* argv[]){

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("hello_world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,FIRE_WIDTH * SCREEN_SCALE, FIRE_HEIGHT *SCREEN_SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, FIRE_WIDTH, FIRE_HEIGHT);

    
    for (int x = 0; x < FIRE_WIDTH; x++) {
        firePixels[FIRE_HEIGHT - 1][x] = 36;
    }
    bool running = true;
    SDL_Event events;

    while (running){
        while(SDL_PollEvent(&events)){
            if(events.type == SDL_QUIT){
                running = false;
            }
        }
        fire();
        // render stuff goes here
        void* texturePixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &texturePixels, &pitch);
        uint32_t* screenBuffer = static_cast<uint32_t*>(texturePixels);

        // map pixel codes to actual firePalette
        for(int row = 0; row< FIRE_HEIGHT; ++row){
            for(int col = 0; col < FIRE_WIDTH; ++col){
                screenBuffer[row * FIRE_WIDTH + col] = firePalette[firePixels[row][col]];
            }
        }

        SDL_UnlockTexture(texture);

        // 4. Render and scale onto the window screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Target roughly 60 FPS

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}