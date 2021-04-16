#ifndef CPU_HPP
#define CPU_HPP
#include <iostream>
#include <map>
#include "../Renderer/Renderer.cpp"

class Chip8 {
  
  private:
    // Memory RAM (4K bytes)
    u_int8_t memory[4096];
    
    // 16 8-bit general purpose registers
    u_int8_t reg[16];
    
    // index register
    u_int16_t i;

    // program counter
    u_int16_t pc;

    // stack pointer
    u_int8_t sp;

    // stack
    u_int16_t stack[16];

    // Timers
    u_int8_t soundTimer;
    u_int8_t delayTimer;

    bool paused = false;
    short int speed = 10;

    /*

      Chip8 Keyboard
      .---------------.
      | 1 |	2 |	3 |	C |
      |---|---|---|---|
      | 4 |	5 |	6 |	D |
      |---|---|---|---|
      | 7 |	8 |	9 |	E |
      |---|---|---|---|
      | A |	0 |	B |	F |
      '---------------'

    */

    std::map<short unsigned int, short unsigned int> keyboard = {
      { SDLK_1, 0x1 }, { SDLK_2, 0x2 }, { SDLK_3, 0x3 }, { SDLK_4, 0xC },
      { SDLK_q, 0x4 }, { SDLK_w, 0x5 }, { SDLK_e, 0x6 }, { SDLK_r, 0xD },
      { SDLK_a, 0x7 }, { SDLK_s, 0x8 }, { SDLK_d, 0x9 }, { SDLK_f, 0xE },
      { SDLK_z, 0xA }, { SDLK_x, 0x0 }, { SDLK_c, 0xB }, { SDLK_v, 0xF }
    };

    bool keyMap[16];
    short int waitingForKeyPress;

    Renderer *renderer;

  public:
    Chip8();
    ~Chip8();
    void start();
    void pause();
    void loadDefaultSprites();
    void loadProgram(u_int8_t *programData, size_t length);
    void cycle();
    void onKeyUp(SDL_Event *event);
    void onKeyDown(SDL_Event *event);
    void execute(u_int16_t opcode);
    void onWindowResize(SDL_Event *event);

};

#endif