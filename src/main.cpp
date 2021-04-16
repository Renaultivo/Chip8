#include <iostream>
#include <ctype.h>
#include "./Core/Chip8.cpp"


void printOpCodeBuffer(u_int8_t *buffer, size_t fileSize) {

 /* for (int i=0; i<fileSize; i+=2) {
    printf("| $%04x | 0x%04x | ", 0x200 + i, (buffer[i] << 8 | buffer[i+1]));
    execute(buffer[i] << 8 | buffer[i+1]);
    printf("\n");
  }*/

}

int main(int argumentCounter, char **argumentValues) {

  if (argumentCounter <= 1) {
    std::cout<<"No rom was provided."<<std::endl;
    std::cout<<"Example: \""<<argumentValues[0]<<" './RomPathHere'\"."<<std::endl;
    exit(EXIT_FAILURE);
  }

  FILE *file;
  file = fopen(argumentValues[1], "rb");

  // move the cursor to the end of the file
  fseek(file, 0L, SEEK_END);

  // get the current position of the cursor
  size_t fileSize = ftell(file);

  // after get the file size seek back
  fseek(file, 0L, SEEK_SET);

  u_int8_t buffer[fileSize];
  long int currentCursorPosition = 0;

  fread(buffer, fileSize, 1, file);
  printOpCodeBuffer(buffer, fileSize);

  Chip8 *chip8 = new Chip8();
  chip8->loadProgram(buffer, fileSize);
  chip8->start();

  u_int32_t nextFrameTime = 0;

  for (;;) {

    u_int32_t currentTime = SDL_GetTicks();

    // 16.7 milliseconds between each frame (60fps)

    if (currentTime >= nextFrameTime) {
      nextFrameTime = currentTime + 16.7;
      //nextFrameTime = currentTime + 1000;
      chip8->cycle();
    }

    SDL_Event event;

    while (SDL_PollEvent(&event)) {

      switch (event.type) {
        case SDL_QUIT: {
          exit(EXIT_SUCCESS);
          break;
        }
        case SDL_KEYUP: {
          chip8->onKeyUp(&event);
          break;
        }
        case SDL_KEYDOWN: {
          chip8->onKeyDown(&event);
          break;
        }
        case SDL_WINDOWEVENT: {

          if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            
          }

          break;
        }
        default:
          break;
      }

    }

  }

  return 0;

}