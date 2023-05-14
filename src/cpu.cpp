#include <z80emu/cpu.hpp>

using namespace Z80;

uint8_t CPU::safe_read(uint16_t address)
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

void CPU::alu(uint8_t op, uint8_t value)
{
    uint16_t result;

    switch (op)
    {
    case 0: // ADD
        result = main[7] + value;
        break;
    case 1: // ADC
        result = main[7] + value + (main_flags & Flags::C ? 1 : 0);
        break;
    case 2: // SUB
        result = main[7] - value;
        break;
    case 3: // SBC
        result = main[7] - value - (main_flags & Flags::C ? 1 : 0);
        break;
    }

    // TODO: Flags for subtraction, bitwise operations and compare
    switch (op)
    {
    case 0:
    case 1:
        if (result & 0x80)
        {
            main_flags |= Flags::S;
        }
        else
        {
            main_flags &= ~Flags::S;
        }

        if ((uint8_t)result == 0)
        {
            main_flags |= Flags::Z;
        }
        else
        {
            main_flags &= ~Flags::Z;
        }

        if ((main[7] & 0xf) + (value & 0xf) > 0xf)
        {
            main_flags |= Flags::H;
        }
        else
        {
            main_flags &= ~Flags::H;
        }

        if ((main[7] & 0x80) == (value & 0x80))
        {
            if ((main[7] & 0x80) != (result & 0x80))
            {
                main_flags |= Flags::P;
            }
            else
            {
                main_flags &= ~Flags::P;
            }
        }

        main_flags &= ~Flags::N;

        if (result & 0x100)
        {
            main_flags |= Flags::C;
        }
        else
        {
            main_flags &= ~Flags::C;
        }

        break;
    }
    main[7] = result & 0xff;
}

uint8_t CPU::execute()
{
    Instruction inst = Instruction(safe_read(pc++));

    switch (inst.x)
    {
    case 0:
        switch (inst.z)
        {
        case 6: // LD r[y], n
            write_register(inst.y, safe_read(pc++));
            return 7;
        }
        break;
    case 3:
        switch (inst.z)
        {
        case 6: // alu[y] n
            alu(inst.y, safe_read(pc++));
            return 4;
        }
        break;
    }

    return 4; // NOP
}
