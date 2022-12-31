#include "emulator.hpp"
#include "cartridge/banking.hpp"
#include "io/bus.hpp"
#include "system/interrupt.hpp"
#include <chrono>
#include <iostream>

namespace gameboy {
    using Clock = std::chrono::steady_clock;
    using Timestamp = std::chrono::time_point<Clock>;

    class Performance {
    public:
        void add_frame(const Timestamp& start, const Timestamp& end)
        {
            ++count;
            sum += static_cast<double>((end - start).count());
        }
        void show_average() const
        {
            constexpr int frequency{100};
            if (count % frequency == 0) {
                std::cout << "Average execution time per frame: "  << (sum / count) << "\n";
            }
        }
    private:
        int count{};
        double sum{};
    };

    using namespace ui;

    Emulator::Emulator()
        : p_game_window{ui::create_window("Money Boy", Width{480}, Height{432})}
        , p_game_renderer{ui::create_renderer(p_game_window, Scale{3.0}, Scale{3.0})}
        , p_game_texture{ui::create_texture(p_game_renderer, Width{160}, Height{144})}
    {
    }

    void Emulator::load_game()
    {
        using cartridge::Banking;
        using cartridge::Rom;
        using io::Bus;

        auto p_cartridge{std::make_unique<Rom>("res/blargg/01-special.gb")};
        auto p_mbc{create_mbc(std::move(p_cartridge))};
#ifndef PREBOOT
        auto p_boot_loader{std::make_unique<BootLoader>("res/DMG_boot")};
        p_boot_loader->capture_cartridge(std::move(p_mbc));
        auto p_address_bus{std::make_shared<Bus>(Banking{std::move(p_boot_loader)})};
#else
        auto p_address_bus{std::make_shared<Bus>(Banking{std::move(p_mbc)})};
#endif
        auto p_interrupt{std::make_shared<system::Interrupt>()};
        p_joypad = std::make_shared<system::Joypad>(p_interrupt);
        p_serial = std::make_shared<system::Serial>(p_interrupt);
        p_timer = std::make_shared<system::Timer>(p_interrupt);
        p_lcd = std::make_shared<ppu::Lcd>(p_interrupt);
        p_address_bus->connect_joypad(p_joypad);
        p_address_bus->connect_serial(p_serial);
        p_address_bus->connect_timer(p_timer);
        p_address_bus->connect_interrupt(p_interrupt);
        p_address_bus->connect_lcd(p_lcd);
        p_cpu = std::make_unique<cpu::Core>(p_address_bus);
        p_ppu = std::make_unique<ppu::Core>(p_address_bus);
    }

    void Emulator::run()
    {
        load_game();

        constexpr int cycles_per_frame{70224};
        auto sync = [cycles_per_frame](const Timestamp& prev, const Timestamp& current) -> bool {
            using Seconds = std::chrono::duration<double, std::chrono::seconds::period>;
            static constexpr double frequency{4.194304e6};

            Seconds seconds_per_frame{(1 / frequency * cycles_per_frame)};
            return current - prev >= seconds_per_frame;
        };

        Performance checker{};
        Timestamp prev{Clock::now()};
        int cycle{};
        bool quit{false};
        while (!quit) {
            SDL_Event event{};
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) { // click [×] on the top-right corner
                    quit = true;
                }
                if (event.type == SDL_KEYDOWN) {
                    process_keystroke<SDL_KEYDOWN>(*p_joypad, event.key.keysym.sym);
                }
                if (event.type == SDL_KEYUP) {
                    process_keystroke<SDL_KEYUP>(*p_joypad, event.key.keysym.sym);
                }
            }

            if (cycle < cycles_per_frame) {
                p_timer->tick();
                p_serial->tick();
                p_cpu->tick();

                for (auto i{0}; i < 4; ++i) {
                    p_ppu->tick(*p_lcd);
                }

                p_lcd->update(*p_game_renderer, *p_game_texture);
            }

            cycle += 4;

            if (cycle >= cycles_per_frame) {
                Timestamp current{Clock::now()};
                if (cycle == cycles_per_frame) {
                    checker.add_frame(prev, current);
                    checker.show_average();
                }

                if (sync(prev, current)) {
                    prev = current;
                    cycle = 0;
                }
            }
        }
    }
}