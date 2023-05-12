#pragma once
#include <cstdint>

namespace Z80
{

    class CPU
    {
    private:
        inline uint16_t safe_read(uint16_t address)
        {
            if (read != nullptr)
            {
                return read(address);
            }
            else
            {
                return 0;
            }
        }

        inline void safe_write(uint16_t address, uint8_t value)
        {
            if (write != nullptr)
            {
                write(address, value);
            }
        }

        inline uint8_t read_register(uint8_t reg)
        {
            if (reg == 6)
            {
                return safe_read((main[4] << 8) | main[5]);
            }
            else
            {
                return main[reg];
            }
        }

        inline void write_register(uint8_t reg, uint8_t value)
        {
            if (reg == 6)
            {
                safe_write((main[4] << 8) | main[5], value);
            }
            else
            {
                main[reg] = value;
            }
        }

    public:
        uint8_t main[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t alt[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint16_t pc = 0;

        uint16_t (*read)(uint16_t) = nullptr;
        void (*write)(uint16_t, uint8_t) = nullptr;

        void execute();
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
}