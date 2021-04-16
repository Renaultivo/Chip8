#include <iostream>
#include <ctype.h>

void printASM(u_int16_t opcode) {

  int x = (opcode & 0x0F00) >> 8;
  int y = (opcode & 0x00F0) >> 4;

  switch (opcode & 0xF000) {

      case 0x0000: {

        switch (opcode) {
          
          case 0x00E0: {
            printf("CLS");
            break;
          }

          case 0x0EE: {
            printf("RET");
            break;
          }

          default:
            break;

        };

        break;

      };

      case 0x1000: {
        printf("JP   $%03x", (opcode & 0xFFF));
        break;
      };

      case 0x2000: {
        printf("CALL $%03x", (opcode & 0xFFF));
        break;
      };

      case 0x3000: {
        printf("SE   reg%d, 0x%02x", x, (opcode & 0xFF));
        break;
      };

      case 0x4000: {
        printf("SNE  reg%d, 0x%02x", x, (opcode & 0xFF));
        break;
      };

      case 0x5000: {
        printf("SE   reg%d, reg%d", x, y);
        break;
      };

      case 0x6000: {
        printf("LD   reg%d, %02x", x, (opcode & 0xFF));
        break;
      };

      case 0x7000: {
        printf("ADD  reg%d, %02x", x, (opcode & 0xFF));
        break;
      };

      case 0x8000: {
        
        switch (opcode & 0xF) {

          case 0x0: {
            printf("LD   reg%d, reg%d", x, y);
            break;
          }

          case 0x1: {
            printf("OR   reg%d, reg%d", x, y);
            break;
          }

          case 0x2: {
            printf("AND  reg%d, reg%d", x, y);
            break;
          }

          case 0x3: {
            printf("XOR  reg%d, reg%d", x, y);
            break;
          }

          case 0x4: {
            printf("ADD  reg%d, reg%d", x, y);
            break;
          }

          case 0x5: {
            printf("SUB  reg%d, reg%d", x, y);
            break;
          }

          case 0x6: {
            printf("SHR  reg%d", x);
            break;
          }

          case 0x7: {
            printf("SUBN reg%d, reg%d", x, y);
            break;
          }

          case 0xE: {
            printf("SHL  reg%d", x);
            break;
          }

          default:
            break;

        }

        break;

      }

      case 0x9000: {
        printf("SNE  reg%d, reg%d", x, y);
        break;
      }

      case 0xA000: {
        printf("LD   I, 0x%03x", (opcode & 0xFFF));
        break;
      }

      case 0xB000: {
        printf("JP   reg%d, $%03x", x, (opcode & 0xFFF));
        break;
      }

      case 0xC000: {
        printf("RND  reg%d, 0x%02x", x, (opcode & 0xFF));
        break;
      }

      case 0xD000: {
        printf("DRW  reg%d, reg%d, 0x%01x", x, y, (opcode & 0xF));
        break;
      }

      case 0xE000: {

        switch (opcode & 0xFF) {

          case 0x9E: {
            printf("SKP  reg%d", x);
            break;
          }

          case 0xA1: {
            printf("SKNP reg%d", x);
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
            printf("LD   reg%d, DT", x);
            break;
          }

          case 0x0A: {
            printf("LD   reg%d, K", x);
            break;
          }

          case 0x15: {
            printf("LD   DT, reg%d", x);
            break;
          }

          case 0x18: {
            printf("LD   ST, reg%d", x);
            break;
          }

          case 0x1E: {
            printf("ADD  DT, reg%d", x);
            break;
          }

          case 0x29: {
            printf("LD   F, reg%d", x);
            break;
          }

          case 0x33: {
            printf("LD   B, reg%d", x);
            break;
          }

          case 0x55: {
            printf("LD   [I], reg${x}");
            break;
          }

          case 0x65: {
            printf("LD   reg%d, [I]", x);
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

void printOpCodeBuffer(u_int8_t *buffer, size_t fileSize) {

  for (int i=0; i<fileSize; i+=2) {
    printf("| $%04x | 0x%04x | ", 0x200 + i, (buffer[i] << 8 | buffer[i+1]));
    printASM(buffer[i] << 8 | buffer[i+1]);
    printf("\n");
  }

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
  // so we can measure the file size
  fseek(file, 0L, SEEK_END);

  // get the current position of the cursor
  // that is in the end of the file
  size_t fileSize = ftell(file);

  // after get the file size seek back
  fseek(file, 0L, SEEK_SET);

  // 96 = 12 8-bit instructions
  u_int8_t buffer[fileSize];
  long int currentCursorPosition = 0;

  printf(".-------------------------------------------------\n");
  printf("|  .... RAM address (starts with 0x200) \n");
  printf("|  ||||                                           \n");
  printf("|  |||| |   ......... operation code (opcode)     \n");
  printf("|  |||| |   ||||                                  \n");
  printf("|  vvvv |   vvvv |                                \n");
  printf("| $XXXX | 0xXXXX | assembly code                  \n");
  printf("|-------|--------|--------------------------------\n");

  fread(buffer, fileSize, 1, file);
  printOpCodeBuffer(buffer, fileSize);

  return 0;

}