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

void CPU::write_rp(uint8_t pair, uint16_t value, bool af)
{
    if (pair == 3)
    {
        if (af)
        {
            main[7] = value >> 8;
            main_flags = value & 0xff;
        }
        else
        {
            sp = value;
        }
    }
    else
    {
        main[pair * 2] = value >> 8;
        main[(pair * 2) + 1] = value & 0xff;
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

    // TODO: Flags for bitwise operations and compare
    switch (op)
    {
    case 0:
    case 1:
        if ((main[7] & 0xf) + (value & 0xf) > 0xf)
        {
            main_flags |= Flags::H;
        }
        else
        {
            main_flags &= ~Flags::H;
        }

        if ((int8_t)main[7] + (int8_t)value != (int8_t)result)
        {
            main_flags |= Flags::P;
        }
        else
        {
            main_flags &= ~Flags::P;
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
    case 2:
    case 3:
        if ((main[7] & 0xf) < (value & 0xf))
        {
            main_flags |= Flags::H;
        }
        else
        {
            main_flags &= ~Flags::H;
        }

        if ((int8_t)main[7] - (int8_t)value != (int8_t)result)
        {
            main_flags |= Flags::P;
        }
        else
        {
            main_flags &= ~Flags::P;
        }

        main_flags |= Flags::N;

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
        case 1: // 16-bit load immediate/add
            if (inst.q == 0)
            {
                // LD rp[p], nn
                uint8_t lsb = safe_read(pc++);
                uint8_t msb = safe_read(pc++);
                write_rp(inst.p, (msb << 8) | lsb);
                return 10;
            }
            else
            {
                // ADD HL, rp[p]
                uint32_t result = read_rp(2) + read_rp(inst.p);

                if (read_rp(2) & 0xfff + read_rp(inst.p) & 0xfff > 0xfff)
                {
                    main_flags |= Flags::H;
                }
                else
                {
                    main_flags &= ~Flags::H;
                }

                main_flags &= ~Flags::N;

                if (result & 0x10000)
                {
                    main_flags |= Flags::C;
                }
                else
                {
                    main_flags &= ~Flags::C;
                }

                return 11;
            }
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
