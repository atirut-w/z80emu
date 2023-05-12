#include <z80emu/cpu.hpp>

using namespace Z80;

void CPU::execute()
{
    Instruction inst = Instruction(safe_read(pc++));

    switch (inst.x)
    {
    case 0:
        switch (inst.z)
        {
        case 6:
            write_register(inst.y, safe_read(pc++));
            break;
        }
        break;
    }
}
