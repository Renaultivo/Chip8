#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <SDL2/SDL.h>

class Renderer {

  private:
  
    short unsigned int rows = 32;
    short unsigned int columns = 64;

    // changes according to window size
    short unsigned int scale = 18;

    bool pixelMap[32][64];
    SDL_Window *window;
    SDL_Renderer *renderer;

  public:
    Renderer();
    ~Renderer();
    void clear();
    bool setPixel(u_int8_t x, u_int8_t y);
    void render();
    void onWindowResize(SDL_Event *event);

};

#endif