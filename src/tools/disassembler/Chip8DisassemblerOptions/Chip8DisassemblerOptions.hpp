#ifndef CHIP8DISASSEMBLEROPTIONS_HPP
#define CHIP8DISASSEMBLEROPTIONS_HPP
#include <iostream>
#include <string>

struct DisassemblerOption {
  std::string option;
  std::string description;
  void (*function)();
};

class Chip8DisassemblerOptions {

  public:
    DisassemblerOption printOpCodeTable;

};

#endif
