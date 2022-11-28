#ifndef CPU_CORE_H
#define CPU_CORE_H

#include "mmu.hpp"
#include "registers.hpp"
#include "instruction.hpp"

namespace gameboy::cpu {
    class Core {
    public:
        void tick();
        void test();

        static constexpr double frequency{1.048576e6};
    private:
        Instruction decode(int opcode);

        Instruction instruction{};
        Mmu mmu{"res/DMG_boot"};
        Registers regs{};
    };
}

#endif