add_executable(gameboy main.cpp)
target_sources(gameboy PRIVATE boot_loader.cpp)
target_sources(gameboy PRIVATE emulator.cpp)

target_sources(gameboy PRIVATE apu/core.cpp)
target_sources(gameboy PRIVATE apu/psg.cpp)
target_sources(gameboy PRIVATE apu/timer.cpp)

target_sources(gameboy PRIVATE cartridge/banking.cpp)
target_sources(gameboy PRIVATE cartridge/mbc.cpp)
target_sources(gameboy PRIVATE cartridge/storage.cpp)

target_sources(gameboy PRIVATE cpu/arithmetic.cpp)
target_sources(gameboy PRIVATE cpu/core.cpp)
target_sources(gameboy PRIVATE cpu/instruction.cpp)
target_sources(gameboy PRIVATE cpu/registers.cpp)

target_sources(gameboy PRIVATE io/bus.cpp)

target_sources(gameboy PRIVATE system/interrupt.cpp)
target_sources(gameboy PRIVATE system/joypad.cpp)
target_sources(gameboy PRIVATE system/serial.cpp)
target_sources(gameboy PRIVATE system/timer.cpp)

target_sources(gameboy PRIVATE ppu/core.cpp)
target_sources(gameboy PRIVATE ppu/lcd.cpp)
target_sources(gameboy PRIVATE ppu/oam.cpp)
target_sources(gameboy PRIVATE ppu/tile.cpp)
target_sources(gameboy PRIVATE ppu/vram.cpp)

target_sources(gameboy PRIVATE ui/display.cpp)
target_sources(gameboy PRIVATE ui/sound.cpp)
target_sources(gameboy PRIVATE ui/wrapper.cpp)

target_include_directories(gameboy PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_include_directories(gameboy PRIVATE ${SDL2_INCLUDE_DIR})
target_link_directories(gameboy PRIVATE ${SDL2_BINDIR})
target_link_libraries(gameboy PRIVATE ${SDL2_LIBRARIES})
target_link_options(gameboy PRIVATE -mconsole)