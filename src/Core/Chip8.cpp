#include "Chip8.hpp"
#include <iostream>
#include <SDL2/SDL.h>

typedef void (*onNextKeyEvent)(short unsigned int keyCode);

Chip8::Chip8() {

  this->paused = true;

  this->loadDefaultSprites();

  // first 0x200 bytes -> used by the sprites
  this->pc = 0x200;
  this->sp = 0;

  this->waitingForKeyPress = -1;

  this->renderer = new Renderer();

}

void Chip8::start() {
  this->paused = false;
}

void Chip8::pause() {
  this->paused = true;
}

void Chip8::loadDefaultSprites() {

  u_int8_t sprites[] = {
          
    0b11110000, // 0000
    0b10010000, // 0  0
    0b10010000, // 0  0
    0b10010000, // 0  0
    0b11110000, // 0000

    0b00100000, //  1
    0b01100000, // 11
    0b00100000, //  1
    0b00100000, //  1
    0b01110000, // 111

    0b11110000, // 2222
    0b00010000, //    2
    0b11110000, // 2222
    0b10000000, // 2
    0b11110000, // 2222

    0b11110000, // 3333
    0b00010000, //    3
    0b11110000, // 3333
    0b00010000, //    3
    0b11110000, // 3333

    0b10010000, // 4  4
    0b10010000, // 4  4
    0b11110000, // 4444
    0b00010000, //    4
    0b00010000, //    4

    0b11110000, // 5555
    0b10000000, // 5
    0b11110000, // 5555
    0b00010000, //    5
    0b11110000, // 5555

    0b11110000, // 6666
    0b10000000, // 6
    0b11110000, // 6666
    0b10010000, // 6  6
    0b11110000, // 6666

    0b11110000, // 7777
    0b00010000, //    7
    0b00100000, //   7
    0b01000000, //  7
    0b01000000, //  7

    0b11110000, // 8888
    0b10010000, // 8  8
    0b11110000, // 8888
    0b10010000, // 8  8
    0b11110000, // 8888

    0b11110000, // 9999
    0b10010000, // 9  9
    0b11110000, // 9999
    0b00010000, //    9
    0b11110000, // 9999

    0b11110000, // aaaa
    0b10010000, // a  a
    0b11110000, // aaaa
    0b10010000, // a  a
    0b10010000, // a  a

    0b11100000, // bbb
    0b10010000, // b  b
    0b11100000, // bbb
    0b10010000, // b  b
    0b11100000, // bbb

    0b11110000, // cccc
    0b10000000, // c
    0b10000000, // c
    0b10000000, // c
    0b11110000, // cccc

    0b11100000, // ddd
    0b10010000, // d  d
    0b10010000, // d  d
    0b10010000, // d  d
    0b11100000, // ddd

    0b11110000, // eeee
    0b10000000, // e
    0b11110000, // eeee
    0b10000000, // e
    0b11110000, // eeee

    0b11110000, // ffff
    0b10000000, // f
    0b11110000, // ffff
    0b10000000, // f
    0b10000000  // f
    
  };

  int spritesLength = sizeof(sprites) / sizeof(sprites[0]);

  for (int i=0; i<spritesLength; i++) {
    this->memory[i] = sprites[i];
  }

}

void Chip8::loadProgram(u_int8_t *programData, size_t length) {

  //  0x200 = initial program address 
  for (int i=0; i<length; i++) {
    this->memory[i + 0x200] = programData[i];
  }

}

void Chip8::cycle() {

  for (int i=0; i<this->speed && !this->paused; i+=2) {

    this->execute(
      (this->memory[this->pc] << 8) | this->memory[this->pc + 1]
    );

  }

  this->renderer->render();

  if (!this->paused) {

    if (this->delayTimer > 0) {
      this->delayTimer--;
    }

    if (this->soundTimer > 0) {
      this->soundTimer--;
    }

  }

  for (int i=0; i<16; i++) {
    this->keyMap[i] = false;
  }

}

void Chip8::onKeyUp(SDL_Event *event) {
  
  if (this->keyboard.find(event->key.keysym.sym) != this->keyboard.end()) {
    printf("\nChip8::onKeyUp -> key %02x", this->keyboard[event->key.keysym.sym]);
    this->keyMap[this->keyboard[event->key.keysym.sym]] = false;
  }

}

void Chip8::onKeyDown(SDL_Event *event) {

  if (this->keyboard.find(event->key.keysym.sym) != this->keyboard.end()) {

    printf("\nChip8::onKeyDown -> key %02x", this->keyboard[event->key.keysym.sym]);

    if (this->waitingForKeyPress >= 0) {
      this->reg[this->waitingForKeyPress] = this->keyboard[event->key.keysym.sym];
      this->paused = false;
      this->waitingForKeyPress = -1;
    }

    this->keyMap[this->keyboard[event->key.keysym.sym]] = true;

  }
}

void Chip8::execute(u_int16_t opcode) {

  this->pc += 2;

  int x = (opcode & 0x0F00) >> 8;
  int y = (opcode & 0x00F0) >> 4;

  switch (opcode & 0xF000) {

    case 0x0000: {

      switch (opcode) {
        
        case 0x00E0: {
          // printf("\nCLS");
          this->renderer->clear();
          break;
        }

        case 0x0EE: {
          // printf("\nRET");
          this->pc = this->stack[this->sp];
          this->stack[this->sp] = 0;
          this->sp--;
          break;
        }

        default:
          break;

      };

      break;

    };

    case 0x1000: {
      // printf("\nJP   $%03x", (opcode & 0xFFF));
      this->pc = (opcode & 0xFFF);
      break;
    };

    case 0x2000: {
      // printf("\nCALL $%03x", (opcode & 0xFFF));
      this->sp++;
      this->stack[this->sp] = this->pc;
      this->pc = (opcode & 0xFFF);
      break;
    };

    case 0x3000: {
      // printf("\nSE   reg%d, 0x%02x", x, (opcode & 0xFF));
      
      if (this->reg[x] == (opcode & 0xFF)) {
        this->pc += 2;
      }

      break;
    };

    case 0x4000: {
      // printf("\nSNE  reg%d, 0x%02x", x, (opcode & 0xFF));
      
      if (this->reg[x] != (opcode & 0xFF)) {
        this->pc += 2;
      }

      break;
    };

    case 0x5000: {
      // printf("\nSE   reg%d, reg%d", x, y);
      
      if (this->reg[x] == this->reg[y]) {
        this->pc += 2;
      }
      
      break;
    };

    case 0x6000: {
      // printf("\nLD   reg%d, %02x", x, (opcode & 0xFF));
      this->reg[x] = (opcode & 0xFF);
      break;
    };

    case 0x7000: {
      // printf("\nADD  reg%d, %02x", x, (opcode & 0xFF));
      this->reg[x] += (opcode & 0xFF);
      break;
    };

    case 0x8000: {
      
      switch (opcode & 0xF) {

        case 0x0: {
          // printf("\nLD   reg%d, reg%d", x, y);
          this->reg[x] = this->reg[y];
          break;
        }

        case 0x1: {
          // printf("\nOR   reg%d, reg%d", x, y);
          this->reg[x] |= this->reg[y];
          break;
        }

        case 0x2: {
          // printf("\nAND  reg%d, reg%d", x, y);
          this->reg[x] &= this->reg[y];
          break;
        }

        case 0x3: {
          // printf("\nXOR  reg%d, reg%d", x, y);
          this->reg[x] ^= this->reg[y];
          break;
        }

        case 0x4: {
          // printf("\nADD  reg%d, reg%d", x, y);
        
          u_int16_t addResult =  this->reg[x] + this->reg[y];

          if (addResult > 0xFF) {
            this->reg[0xF] = 1;
          } else {
            this->reg[0xF] = 0;
          }

          this->reg[x] = addResult;

          break;
        }

        case 0x5: {
          // printf("\nSUB  reg%d, reg%d", x, y);

          if (this->reg[x] > this->reg[y]) {
            this->reg[0xF] = 1;
          } else {
            this->reg[0xF] = 0;
          }

          this->reg[x] -= this->reg[y];

          break;
        }

        case 0x6: {
          // printf("\nSHR  reg%d", x);
          this->reg[x] >>= 1;
          break;
        }

        case 0x7: {
          // printf("\nSUBN reg%d, reg%d", x, y);

          if (this->reg[y] > this->reg[x]) {
            this->reg[0xF] = 1;
          } else {
            this->reg[0xF] = 0;
          }

          this->reg[x] = this->reg[y] - this->reg[x];

          break;
        }

        case 0xE: {
          // printf("\nSHL  reg%d", x);
          this->reg[x] <<= 1;
          break;
        }

        default:
          break;

      }

      break;

    }

    case 0x9000: {
      // printf("\nSNE  reg%d, reg%d", x, y);

      if (this->reg[x] != this->reg[y]) {
        this->pc += 2;
      }

      break;
    }

    case 0xA000: {
      // printf("\nLD   I, 0x%03x", (opcode & 0xFFF));
      this->i = (opcode & 0xFFF);
      break;
    }

    case 0xB000: {
      // printf("\nJP   reg0, $%03x", (opcode & 0xFFF));
      this->pc = this->reg[0] + (opcode & 0xFFF);
      break;
    }

    case 0xC000: {
      // printf("\nRND  reg%d, 0x%02x", x, (opcode & 0xFF));
      this->reg[x] = (rand() % 0xFF) & (opcode & 0xFF);
      break;
    }

    case 0xD000: {
      // printf("\nDRW  reg%d, reg%d, 0x%01x", x, y, (opcode & 0xF));

      // number of sprites to draw
      int n = (opcode & 0xF);
      this->reg[0xF] = 0;

      for (short unsigned int row=0; row < n; row++) {

        u_int8_t sprite = this->memory[this->i + row];

        for (short unsigned int column=0; column<8; column++) {

          if ((sprite & 0x80) > 0) {
            
            bool previousPixel = this->renderer->setPixel(
              this->reg[x] + column,
              this->reg[y] + row
            );

            if (previousPixel) {
              this->reg[0xF] = 1;
            }

          }

          sprite <<= 1;

        }

      }

      break;

    }

    case 0xE000: {

      switch (opcode & 0xFF) {

        case 0x9E: {
          // printf("\nSKP  reg%d", x);

          if (this->keyMap[this->reg[x]]) {
            this->pc += 2;
          }

          break;
        }

        case 0xA1: {
          // printf("\nSKNP reg%d", x);

          if (!this->keyMap[this->reg[x]]) {
            this->pc += 2;
          }

          break;
        }

        default:
          break;

      }

      break;

    }

    case 0xF000: {

      switch (opcode & 0xFF) {

        case 0x07: {
          // printf("\nLD   reg%d, DT", x);
          this->reg[x] = this->delayTimer;
          break;
        }

        case 0x0A: {
          // printf("\nLD   reg%d, K", x);
          this->paused = true;
          this->waitingForKeyPress = x;
          break;
        }

        case 0x15: {
          // printf("\nLD   DT, reg%d", x);
          this->delayTimer = this->reg[x];
          break;
        }

        case 0x18: {
          // printf("\nLD   ST, reg%d", x);
          this->soundTimer = this->reg[x];
          break;
        }

        case 0x1E: {
          // printf("\nADD  DT, reg%d", x);
          this->i += this->reg[x];
          break;
        }

        case 0x29: {
          // printf("\nLD   F, reg%d", x);
          // each sprite digit has 5 rows
          this->i = this->reg[x] * 5;
          break;
        }

        case 0x33: {
          // printf("\nLD   B, reg%d", x);
          this->memory[this->i] = this->reg[x] / 100;
          this->memory[this->i+1] = (this->reg[x] % 100) / 10;
          this->memory[this->i+2] = this->reg[x] % 10;
          break;
        }

        case 0x55: {
          // printf("\nLD   [I], reg${x}");
          for (int i=0; i<=x; i++) {
            this->memory[this->i + i] = this->reg[i];
          }
          break;
        }

        case 0x65: {
          // printf("\nLD   reg%d, [I]", x);
          for (int i=0; i<=x; i++) {
            this->reg[i] = this->memory[this->i + i];
          }
          break;
        }

        default:
          break;

      }

      break;
    }

    default:
      break;

  };

}

void Chip8::onWindowResize(SDL_Event *event) {
  this->renderer->onWindowResize(event);
}
