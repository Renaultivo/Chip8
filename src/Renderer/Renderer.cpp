#include "Renderer.hpp"

Renderer::Renderer() {

  this->window = SDL_CreateWindow(
    "Chip8 Emulator",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    512, 256,
    SDL_WINDOW_RESIZABLE
  );

  if (this->window == nullptr) {
    puts("Failed to create window.");
    exit(EXIT_FAILURE);
  }

  this->renderer = SDL_CreateRenderer(
    this->window,
    -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );

  if (this->window == nullptr) {
    puts("Failed to create window.");
    exit(EXIT_FAILURE);
  }

  SDL_RenderSetScale(this->renderer, 8, 8);

}

void Renderer::clear() {

  for (int row=0; row < this->rows; row++) {

    for (int column=0; column < this->columns; column++) {
      this->pixelMap[row][column] = 0;
    }

  }

}

bool Renderer::setPixel(u_int8_t x, u_int8_t y) {
  this->pixelMap[y][x] = !this->pixelMap[y][x];
  return !this->pixelMap[y][x];
}

void Renderer::render() {

  SDL_RenderSetScale(this->renderer, this->scale, this->scale);

  SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
  SDL_RenderClear(this->renderer);

  SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);

  for (int row=0; row < 32; row++) {

    for (int column=0; column < 64; column++) {
      
      if (this->pixelMap[row][column]) {
        SDL_RenderDrawPoint(this->renderer, column, row);
      }

    }

  }

  SDL_RenderPresent(this->renderer);

}

void Renderer::onWindowResize(SDL_Event *event) {

  int windowWidth = event->window.data1;
  int windowHeight = event->window.data1;

  int verticalScale = windowHeight / this->rows;
  int horizontalScale = windowWidth / this->columns;
  
  if (horizontalScale < verticalScale) {
    this->scale = horizontalScale;
  } else {
    this->scale = verticalScale;
  }

}
