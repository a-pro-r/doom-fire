using namespace std;
#include<iostream>
#include<SDL.h>
#include <cstdio>
#include<vector>
#include<string>

const int FIRE_WIDTH = 300;
const int FIRE_HEIGHT = 300;
const int SCREEN_SCALE = 3; 
int r, g, b;
const int startR = 7, startG = 7, startB = 7;

// Generate 37-color palette
uint32_t firePalette[37];


vector<vector<int>> firePixels(FIRE_HEIGHT, vector<int>(FIRE_WIDTH, 0));

bool parseHexColor(const std::string& s, int& r, int& g, int& b){
    if (std::sscanf(s.c_str(), "#%02x%02x%02x", &r, &g, &b) == 3) {
        return true;
    } else {
        return false;
    }

}

void generatePalette(int r, int g, int b){
    int R,G,B;
    uint32_t a = 255;
    for(int i =0 ; i <= 18; ++i){
        float t = i / 18.0;
        R = startR + (r - startR) * t;
        G = startG + (g - startG) * t;
        B = startB + (b - startB) * t;
        uint32_t color = ((R & 0xFF) << 24) | 
                 ((G & 0xFF) << 16) | 
                 ((B & 0xFF) << 8)  | 
                 (a & 0xFF);
        firePalette[i] = color;
    }
    for(int i = 18; i<= 36;++i){
        float t = (i - 18) / 18.0;
        R = r + (255 - r) * t;
        G = g + (255 - g) * t;
        B = b + (255 - b) * t;
        uint32_t color = ((R & 0xFF) << 24) | 
                 ((G & 0xFF) << 16) | 
                 ((B & 0xFF) << 8)  | 
                 (a & 0xFF);
        firePalette[i] = color;
    }

}

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

    int r = 0xDF, g = 0x57, b = 0x07;
    if (argc > 1) {                             
        if (!parseHexColor(argv[1], r, g, b)) {
            std::fprintf(stderr,
                "Invalid color '%s', using default orange\n", argv[1]);
        }
    }
    generatePalette(r, g, b);
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