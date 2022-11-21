#ifndef CPU_CORE_H
#define CPU_CORE_H

#include "decoder.hpp"
#include "mmu.hpp"
#include "registers.hpp"

namespace gameboy::cpu {
    class Core {
    public:
        void tick();

        static constexpr double frequency{ 1.048576e6 };
    private:
        void test();

        Decoder decoder{};
        Instruction instruction{};
        Mmu mmu{ "res/DMG_boot" };
        Registers regs{};
    };
}

#endif