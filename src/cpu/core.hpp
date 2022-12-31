#ifndef CPU_CORE_H
#define CPU_CORE_H

#include <memory>
#include "io/bus.hpp"
#include "instruction.hpp"
#include "registers.hpp"
#include "debugger.hpp"

namespace gameboy::cpu {
    class Core {
    public:
        explicit Core(std::shared_ptr<io::Bus> shared_bus);
        void tick();
        void test();

    private:
        Instruction decode(int opcode);
        void execute(const Instruction::Operation& func, int cycle);
        Instruction::SideEffect resolve_prefixed_instruction();
        void check_interrupt();

        int m_cycle{0};
        Instruction instruction{};
        Registers regs{};
        bool interrupt_master_enable{};
        std::shared_ptr<io::Bus> p_bus;
    };
}

#endif