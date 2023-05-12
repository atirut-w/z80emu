#include <z80emu/cpu.hpp>

using namespace Z80;

void CPU::execute()
{
    Instruction inst = Instruction(safe_read(pc++));
}
