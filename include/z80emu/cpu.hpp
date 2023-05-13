#pragma once
#include <cstdint>
#include <functional>

namespace Z80
{
    class CPU
    {
    private:
        uint8_t safe_read(uint16_t);
        void safe_write(uint16_t, uint8_t);
        uint8_t read_register(uint8_t);
        void write_register(uint8_t, uint8_t);

        void alu(uint8_t, uint8_t);

    public:
        uint8_t main[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t alt[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t main_flags = 0;
        uint8_t alt_flags = 0;

        uint16_t pc = 0;

        std::function<uint8_t(uint16_t)> read;
        std::function<void(uint16_t, uint8_t)> write;

        uint8_t execute();
    };

    class Instruction
    {
    public:
        uint8_t x, y, z, p, q;

        Instruction(uint8_t opcode)
        {
            x = opcode >> 6;
            y = (opcode >> 3) & 0b111;
            z = opcode & 0b111;
            p = y >> 1;
            q = y & 1;
        }
    };

    enum Flags
    {
        C = 1 << 0,
        N = 1 << 1,
        P = 1 << 2,
        H = 1 << 4,
        Z = 1 << 6,
        S = 1 << 7,
    };
}
