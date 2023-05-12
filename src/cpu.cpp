#include <z80emu/cpu.hpp>

using namespace Z80;

uint16_t CPU::safe_read(uint16_t address)
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

void CPU::safe_write(uint16_t address, uint8_t value)
{
    if (write != nullptr)
    {
        write(address, value);
    }
}

uint8_t CPU::read_register(uint8_t reg)
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

void CPU::write_register(uint8_t reg, uint8_t value)
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

void CPU::execute()
{
    Instruction inst = Instruction(safe_read(pc++));

    switch (inst.x)
    {
    case 0:
        switch (inst.z)
        {
        case 6: // LD r[y], n
            write_register(inst.y, safe_read(pc++));
            break;
        }
        break;
    }
}
