#include "bus.hpp"
#include <cassert>
#include <stdexcept>

namespace gameboy::io {
    void dma_transfer(Bus& bus, std::uint8_t source)
    {
        auto source_address{source << 8};
        constexpr auto dest_address{0xFE00};
        for (auto i{0}; i < 160; ++i) {
            bus.write_byte(dest_address + i, bus.read_byte(source_address + i));
        }
    }

    Bus::Bus(Bundle bundle) : peripherals{std::move(bundle)}
    {
        work_ram.resize(0xE000 - 0xC000);
        ports.resize(0xFF80 - 0xFF00);
        high_ram.resize(0xFFFF - 0xFF80);
        //ram[0xFF44] = 144; // bypass frame check
    }

    std::uint8_t Bus::read_byte(int address) const
    {
        if (address < 0) {
            throw std::out_of_range{"Negative Address!"};
        }
        else if (address < 0x8000) {
            return peripherals.cartridge_space.read(address);
        }
        else if (address < 0xA000) {
            return peripherals.vram.get().read(address);
        }
        else if (address < 0xC000) {
            return peripherals.cartridge_space.read(address);
        }
        else if (address < 0xE000) {
            return work_ram[address - 0xC000];
        }
        else if (address < 0xFE00) {
            return work_ram[address - 0xE000]; // mirror
        }
        else if (address < 0xFEA0) {
            return peripherals.oam.get().read(address);
        }
        else if (address < 0xFF00) {
            /*
                Access to this area is prohibited. Unfortunately, it does occurs
                in some games and may cause OAM corruption on real hardware.
            */
            return 0;
        }
        else if (address == 0xFF00) {
            return peripherals.joypad.get().read(address);
        }
        else if (address >= 0xFF01 && address < 0xFF03) {
            return peripherals.serial.get().read(address);
        }
        else if (address >= 0xFF04 && address < 0xFF08) {
            return peripherals.timer.get().read(address);
        }
        else if (address == 0xFF0F) {
            return peripherals.interrupt.get().read(address);
        }
        else if (address >= 0xFF10 && address < 0xFF40) {
            return peripherals.psg.get().read(address);
        }
        else if (address >= 0xFF40 && address < 0xFF4C) {
            return peripherals.lcd.get().read(address);
        }
        else if (address >= 0xFF00 && address < 0xFF80) {
            return ports[address - 0xFF00];
        }
        else if (address < 0xFFFF) {
            return high_ram[address - 0xFF80];
        }
        else if (address == 0xFFFF) {
            return interrupt_enable;
        }
        else {
            throw std::out_of_range{"Not Implemented Address Space!"};
        }
    }

    void Bus::write_byte(int address, std::uint8_t value)
    {
        if (address < 0) {
            throw std::out_of_range{"Negative Address!"};
        }
        else if (address < 0x8000) {
            peripherals.cartridge_space.write(address, value);
        }
        else if (address < 0xA000) {
            peripherals.vram.get().write(address, value);
        }
        else if (address < 0xC000) {
            peripherals.cartridge_space.write(address, value);
        }
        else if (address < 0xE000) {
            work_ram[address - 0xC000] = value;
        }
        else if (address < 0xFE00) {
            throw std::out_of_range{"This address is the mirror of WRAM."};
        }
        else if (address < 0xFEA0) {
            peripherals.oam.get().write(address, value);
        }
        else if (address < 0xFF00) {
            /*
                Access to this area is prohibited. Unfortunately, it does occurs
                in some games and may cause OAM corruption on real hardware.
            */
        }
        else if (address == 0xFF00) {
            peripherals.joypad.get().write(address, value);
        }
        else if (address >= 0xFF01 && address < 0xFF03) {
            peripherals.serial.get().write(address, value);
        }
        else if (address >= 0xFF04 && address < 0xFF08) {
            peripherals.timer.get().write(address, value);
        }
        else if (address == 0xFF0F) {
            peripherals.interrupt.get().write(address, value);
        }
        else if (address >= 0xFF10 && address < 0xFF40) {
            peripherals.psg.get().write(address, value);
        }
        else if (address == 0xFF46) {
            peripherals.lcd.get().write(address, value);
            dma_transfer(*this, value);
        }
        else if (address >= 0xFF40 && address < 0xFF4C) {
            peripherals.lcd.get().write(address, value);
        }
        else if (address == 0xFF50) {
            peripherals.cartridge_space.disable_boot_rom();
        }
        else if (address >= 0xFF00 && address < 0xFF80) {
            ports[address - 0xFF00] = value;
        }
        else if (address < 0xFFFF) {
            high_ram[address - 0xFF80] = value;
        }
        else if (address == 0xFFFF) {
            interrupt_enable = value;
        }
        else {
            throw std::out_of_range{"Not Implemented Address Space!"};
        }
    }

    int make_address(std::uint8_t high, std::int8_t low)
    {
        return (high << 8) | low;
    }
}