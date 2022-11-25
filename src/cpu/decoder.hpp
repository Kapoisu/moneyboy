#ifndef CPU_DECODER_H
#define CPU_DECODER_H

#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "registers.hpp"
#include "mmu.hpp"

namespace gameboy::cpu {
    struct Instruction {
        int opcode{0x00};
        std::string name{"NOP"};
        int cycle{1}; // m-cycle
        std::function<void(int cycle, Registers&, Mmu&)> execute{[](int cycle, Registers& regs, Mmu&) {}};
    };

    class Decoder {
    public:
        Instruction operator()(std::uint8_t opcode) const;
    private:
        static const std::unordered_map<std::uint8_t, Instruction> instruction_map;
    };
}

#endif