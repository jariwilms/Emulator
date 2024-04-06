#include "stdafx.hpp"

#include "SharpSM83.hpp"

namespace dot::gb
{
    const std::array<std::string, 0x100> OPCODE_NAMES_D
    {
        "NOP",         "LD BC, d16", "LD [BC], A",  "INC BC",     "INC B",        "DEC B",      "LD B, d8",    "RLCA",       "LD [a16], SP", "ADD HL, BC", "LD A, [BC]",  "DEC BC",    "INC C",       "DEC C",    "LD C, d8",    "RRCA",
        "STOP 0",      "LD DE, d16", "LD [DE], A",  "INC DE",     "INC D",        "DEC D",      "LD D, d8",    "RLA",        "JR r8",        "ADD HL, DE", "LD A, [DE]",  "DEC DE",    "INC E",       "DEC E",    "LD E, d8",    "RRA",
        "JR NZ, r8",   "LD HL, d16", "LD [HL+], A", "INC HL",     "INC H",        "DEC H",      "LD H, d8",    "DAA",        "JR Z, r8",     "ADD HL, HL", "LD A, [HL+]", "DEC HL",    "INC L",       "DEC L",    "LD L, d8",    "CPL",
        "JR NC, r8",   "LD SP, d16", "LD [HL-], A", "INC SP",     "INC [HL]",     "DEC [HL]",   "LD [HL], d8", "SCF",        "JR C, r8",     "ADD HL, SP", "LD A, [HL-]", "DEC SP",    "INC A",       "DEC A",    "LD A, d8",    "CCF",
        "LD B, B",     "LD B, C",    "LD B, D",     "LD B, E",    "LD B, H",      "LD B, L",    "LD B, [HL]",  "LD B, A",    "LD C, B",      "LD C, C",    "LD C, D",     "LD C, E",   "LD C, H",     "LD C, L",  "LD C, [HL]",  "LD C, A", 
        "LD D, B",     "LD D, C",    "LD D, D",     "LD D, E",    "LD D, H",      "LD D, L",    "LD D, HL",    "LD D, A",    "LD E, B",      "LD E, C",    "LD E, D",     "LD E, E",   "LD E, H",     "LD E, L",  "LD E, [HL]",  "LD E, A", 
        "LD H, B",     "LD H, C",    "LD H, D",     "LD H, E",    "LD H, H",      "LD H, L",    "LD H, [HL]",  "LD H, A",    "LD L, B",      "LD L, C",    "LD L, D",     "LD L, E",   "LD L, H",     "LD L, L",  "LD L, [HL]",  "LD L, A", 
        "LD [HL], B",  "LD [HL], C", "LD [HL], D",  "LD [HL], E", "LD [HL], H",   "LD [HL], L", "HALT",        "LD [HL], A", "LD A, B",      "LD A, C",    "LD A, D",     "LD A, E",   "LD A, H",     "LD A, L",  "LD A, [HL]",  "LD A, A", 
        "ADD A, B",    "ADD A, C",   "ADD A, D",    "ADD A, E",   "ADD A, H",     "ADD A, L",   "ADD A, [HL]", "ADD A, A",   "ADC A, B",     "ADC A, C",   "ADC A, D",    "ADC A, E",  "ADC A, H",    "ADC A, L", "ADC A, [HL]", "ADC A, A", 
        "SUB B",       "SUB C",      "SUB D",       "SUB E",      "SUB H",        "SUB L",      "SUB [HL]",    "SUB A",      "SBC A, B",     "SBC A, C",   "SBC A, D",    "SBC A, E",  "SBC A, H",    "SBC A, L", "SBC A, [HL]", "SBC A, A", 
        "AND B",       "AND C",      "AND D",       "AND E",      "AND H",        "AND L",      "AND [HL]",    "AND A",      "XOR B",        "XOR C",      "XOR D",       "XOR E",     "XOR H",       "XOR L",    "XOR [HL]",    "XOR A", 
        "OR B",        "OR C",       "OR D",        "OR E",       "OR H",         "OR L",       "OR [HL]",     "OR A",       "CP B",         "CP C",       "CP D",        "CP E",      "CP H",        "CP L",     "CP [HL]",     "CP A", 
        "RET NZ",      "POP BC",     "JP NZ, a16",  "JP a16",     "CALL NZ, a16", "PUSH BC",    "ADD A, d8",   "RST 00H",    "RET Z",        "RET",        "JP Z, a16",   "PREFIX CB", "CALL Z, a16", "CALL a16", "ADC A, d8",   "RST 08H", 
        "RET NC",      "POP DE",     "JP NC, a16",  "INVALID",    "CALL NC, a16", "PUSH DE",    "SUB d8",      "RST 10H",    "RET C",        "RETI",       "JP C, a16",   "INVALID",   "CALL C, a16", "INVALID",  "SBC A, d8",   "RST 18H", 
        "LDH [a8], A", "POP HL",     "LD [C], A",   "INVALID",    "INVALID",      "PUSH HL",    "AND d8",      "RST 20H",    "ADD SP, r8",   "JP [HL]",    "LD [a16], A", "INVALID",   "INVALID",     "INVALID",  "XOR d8",      "RST 28H", 
        "LDH A, [a8]", "POP AF",     "LD A, [C]",   "DI",         "INVALID",      "PUSH AF",    "OR d8",       "RST 30H",    "LD HL, SP+r8", "LD SP, HL",  "LD A, [a16]", "EI",        "INVALID",     "INVALID",  "CP d8",       "RST 38H", 
    };
    static void print_register_diff(const SharpSM83::Registers& current, const SharpSM83::Registers& previous)
    {
        if (current.a != previous.a) std::cout << std::format("[A]: {:x} -> {:x}\n", previous.a, current.a);
        if (current.f != previous.f) std::cout << std::format("[F]: {:x} -> {:x}\n", previous.f, current.f);
        if (current.b != previous.b) std::cout << std::format("[B]: {:x} -> {:x}\n", previous.b, current.b);
        if (current.c != previous.c) std::cout << std::format("[C]: {:x} -> {:x}\n", previous.c, current.c);
        if (current.d != previous.d) std::cout << std::format("[D]: {:x} -> {:x}\n", previous.d, current.d);
        if (current.e != previous.e) std::cout << std::format("[E]: {:x} -> {:x}\n", previous.e, current.e);
        if (current.h != previous.h) std::cout << std::format("[H]: {:x} -> {:x}\n", previous.h, current.h);
        if (current.l != previous.l) std::cout << std::format("[L]: {:x} -> {:x}\n", previous.l, current.l);

        if (current.sp != previous.sp) std::cout << std::format("[SP]: {:x} -> {:x}\n", previous.sp, current.sp);
        if (current.pc != previous.pc) std::cout << std::format("[PC]: {:x} -> {:x}\n", previous.pc, current.pc);
    }

    SharpSM83::SharpSM83(std::array<byte, 0x10000>& memory)
        : m_memory{ memory }
    {

    }

    void SharpSM83::cycle()
    {
        if (m_ime)
        {
            const auto& enabled  = m_memory.at(arc::IE);
            const auto& interrupt = m_memory.at(arc::IF);

            if (enabled & interrupt)
            {
                if ((m_memory.at(0xffff) & 1) & (m_memory.at(0xff0f) & 1)) {
                    --m_registers.sp;
                    m_memory.at(m_registers.sp) = m_registers.pc >> 8;
                    --m_registers.sp;

                    m_memory.at(m_registers.sp) = m_registers.pc & 0xff;
                    m_registers.pc = 0x40;
                    m_memory.at(0xff0f) = m_memory.at(0xff0f) & ~1;
                }
                else
                {
                    __debugbreak();
                }
            }
        }
        else
        {
            fetch();
            //decode(); //Redundant? Nothing has to be decoded due to jump table
            execute();
        }
    }

    void SharpSM83::reset()
    {
        m_registers.reset();
        m_opcode = 0;

        m_ime     = false;
        m_imeNext = false;
    }

    void SharpSM83::fetch()
    {
        m_opcode = m_memory.at(m_registers.pc++);
    }
    void SharpSM83::execute()
    {
        m_lastRegisters = m_registers;

        switch (m_opcode)
        {
            case 0x00: //NOP
            {
                break;
            }
            case 0x01: //LD BC, d16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                m_registers.bc = shifted_composite<word>(msb, lsb);

                break;
            }
            case 0x02: //LD [BC], A
            {
                m_memory.at(m_registers.bc) = m_registers.a;

                break;
            }
            case 0x03: //INC BC
            {
                ++m_registers.bc;

                break;
            }
            case 0x04: //INC B
            {
                const auto& halfCarryFlag = half_carry_occurred_8b_add(m_registers.b, 1);

                ++m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.b == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, halfCarryFlag);

                break;
            }
            case 0x05: //DEC B
            {
                const auto& halfCarryFlag = half_carry_occurred_8b_sub(m_registers.b, 1);

                --m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.b == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, halfCarryFlag);

                break;
            }
            case 0x06: //LD B, d8
            {
                m_registers.b = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x07: //RLCA
            {
                const auto& carryFlag = get_bit(m_registers.a, 7);

                m_registers.a = shift(m_registers.a, ShiftType::ROR, 7);

                m_registers.set_flag(Registers::Flag::Zero,      false);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     carryFlag);

                break;
            }
            case 0x08: //LD [a16], SP
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                const auto& address = shifted_composite<word>(msb, lsb);

                m_memory.at(address)     = static_cast<byte>( m_registers.sp       & 0xFF);
                m_memory.at(address + 1) = static_cast<byte>((m_registers.sp >> 8) & 0xFF);

                break;
            }
            case 0x09: //ADD HL, BC
            {
                const auto& halfCarryFlag = half_carry_occurred_16b_add(m_registers.hl, m_registers.bc);
                const auto& carryFlag     = carry_occurred_16b_add(m_registers.hl, m_registers.bc);

                m_registers.hl += m_registers.bc;

                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, halfCarryFlag);
                m_registers.set_flag(Registers::Flag::Carry,     carryFlag);

                break;
            }
            case 0x0A: //LD A, [BC]
            {
                m_registers.a = m_memory.at(m_registers.bc);

                break;
            }
            case 0x0B: //DEC BC
            {
                --m_registers.bc;

                break;
            }
            case 0x0C: //INC C
            {
                const auto& halfCarry = half_carry_occurred_8b_add(m_registers.c, 1);

                ++m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.c == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, halfCarry);

                break;
            }
            case 0x0D: //DEC C, TODO: idem as 0x05
            {
                --m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.c == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.c, 3));

                break;
            }
            case 0x0E: //LD C, n8
            {
                m_registers.c = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x0F: //RRCA
            {
                m_registers.a = shift(m_registers.a, ShiftType::ROR, 1);

                m_registers.set_flag(Registers::Flag::Zero,      false);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     get_bit(m_registers.a, 7));

                break;
            }



            case 0x10:
            {
                stop();

                break;
            }
            case 0x11: //LD DE, n16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                m_registers.de = (msb << 8) | lsb;

                break;
            }
            case 0x12: //LD [DE], A
            {
                m_memory.at(m_registers.de) = m_registers.a;

                break;
            }
            case 0x13: //INC DE
            {
                ++m_registers.de;

                break;
            }
            case 0x14: //INC D
            {
                ++m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.d == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.d, 3));

                break;
            }
            case 0x15: //DEC D
            {
                --m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.d == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.d, 3));

                break;
            }
            case 0x16: //LD D, n8
            {
                m_registers.d = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x17: //RLA
            {
                const auto& bit = get_bit(m_registers.a, 7);

                m_registers.a = shift(m_registers.a, ShiftType::ROR, 7);       //== Rotate left, 1
                m_registers.a = set_bit(m_registers.a, 0, m_registers.flag(Registers::Flag::Carry));

                m_registers.set_flag(Registers::Flag::Zero,      false);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     bit);
                
                break;
            }
            case 0x18:
            {
                const auto& offset = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

                m_registers.pc += offset;

                break;
            }
            case 0x19: //ADD HL, DE
            {
                m_registers.hl += m_registers.de;

                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.hl, 11));
                m_registers.set_flag(Registers::Flag::Carry,     get_bit(m_registers.hl, 15));

                break;
            }
            case 0x1A: //LD A, [DE]
            {
                m_registers.a = m_memory.at(m_registers.de);

                break;
            }
            case 0x1B: //DEC DE
            {
                --m_registers.de;

                break;
            }
            case 0x1C: //INC E, TODO: idem as 0x1C
            {
                ++m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.e == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.e, 3));

                break;
            }
            case 0x1D: //DEC E, TODO: idem as 0x0D
            {
                --m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.e == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.e, 3));

                break;
            }
            case 0x1E: //LD E, n8
            {
                m_registers.e = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x1F: //RRA
            {
                const auto& bit = get_bit(m_registers.a, 0);

                m_registers.a = shift(m_registers.a, ShiftType::ROR, 1);
                m_registers.a = set_bit(m_registers.a, 7, m_registers.flag(Registers::Flag::Carry));

                m_registers.set_flag(Registers::Flag::Zero,      false);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     bit);

                break;
            }



            case 0x20: //JR NZ, e8
            {
                const auto& offset = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

                if (!m_registers.flag(Registers::Flag::Zero)) m_registers.pc += offset;

                break;
            }
            case 0x21: //LD HL, n16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                m_registers.hl = (msb << 8) | lsb;

                break;
            }
            case 0x22: //LD [HL+], A
            {
                m_memory.at(m_registers.hl) = m_registers.a;
                ++m_registers.hl;

                break;
            }
            case 0x23: //INC HL
            {
                ++m_registers.hl;

                break;
            }
            case 0x24: //INC H
            {
                ++m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.h == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.h, 3));

                break;
            }
            case 0x25: //DEC H
            {
                --m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.h == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.h, 3));

                break;
            }
            case 0x26: //LD H, n8
            {
                m_registers.h = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x27: //DAA https://forums.nesdev.org/viewtopic.php?p=196282&sid=23337b4c00337de146414df0713eb703#p196282
            {
                if (!m_registers.flag(Registers::Flag::Negative))              //After an addition, adjust if (half-)carry occurred or if result is out of bounds
                {
                    if (m_registers.flag(Registers::Flag::Carry) || m_registers.a > 0x99)
                    {
                        m_registers.a += 0x60; 

                        m_registers.set_flag(Registers::Flag::Carry, true);
                    }
                    if (m_registers.flag(Registers::Flag::HalfCarry) || (m_registers.a & 0x0F) > 0x09)
                    {
                        m_registers.a += 0x6;
                    }
                }
                else                                                           //After a subtraction, only adjust if (half-)carry occurred
                {
                    if (m_registers.flag(Registers::Flag::Carry))     m_registers.a -= 0x60;
                    if (m_registers.flag(Registers::Flag::HalfCarry)) m_registers.a -= 0x06;
                }

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);

                break;
            }
            case 0x28: //JR Z
            {
                const auto& offset = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

                if (m_registers.flag(Registers::Flag::Zero)) m_registers.pc += offset;

                break;
            }
            case 0x29: //ADD HL, HL
            {
                m_registers.hl += m_registers.hl;

                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.hl, 11));
                m_registers.set_flag(Registers::Flag::Carry,     get_bit(m_registers.hl, 15));

                break;
            }
            case 0x2A: //LD A, [HL+]
            {
                m_registers.a = m_memory.at(m_registers.hl);
                ++m_registers.hl;

                break;
            }
            case 0x2B: //DEC HL
            {
                --m_registers.hl;

                break;
            }
            case 0x2C: //INC L, TODO: idem as 0x1C
            {
                ++m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.l == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.l, 3));

                break;
            }
            case 0x2D: //DEC L, TODO: idem as 0x0D
            {
                --m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.l == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.l, 3));

                break;
            }
            case 0x2E: //LD L, n8
            {
                m_registers.l = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x2F:
            {
                m_registers.a = ~m_registers.a;

                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);

                break;
            }



            case 0x30: //JR NC, e8
            {
                const auto& offset = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

                if (!m_registers.flag(Registers::Flag::Carry)) m_registers.pc += offset;

                break;
            }
            case 0x31: //LD SP, n16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                m_registers.sp = (msb << 8) | lsb;

                break;
            }
            case 0x32: //LD [HL-], A
            {
                m_memory.at(m_registers.hl) = m_registers.a;
                --m_registers.hl;

                break;
            }
            case 0x33: //INC SP
            {
                ++m_registers.sp;

                break;
            }
            case 0x34: //INC [HL]
            {
                const auto& value = ++m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,      value          == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (value & 0x0F) == 0);

                break;
            }
            case 0x35: //DEC [HL]
            {
                const auto& value = --m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,      value          == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (value & 0x0F) == 0);

                break;
            }
            case 0x36: //LD [HL], n8
            {
                m_memory.at(m_registers.hl) = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x37: //SCF
            {
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     true);

                break;
            }
            case 0x38: //JR C
            {
                const auto& offset = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

                if (m_registers.flag(Registers::Flag::Carry)) m_registers.pc += offset;

                break;
            }
            case 0x39: //ADD HL, SP
            {
                m_registers.hl += m_registers.sp;

                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.hl, 11));
                m_registers.set_flag(Registers::Flag::Carry,     get_bit(m_registers.hl, 15));

                break;
            }
            case 0x3A: //LD A, [HL-]
            {
                m_registers.a = m_memory.at(m_registers.hl);
                --m_registers.hl;

                break;
            }
            case 0x3B: //DEC SP
            {
                --m_registers.sp;

                break;
            }
            case 0x3C: //INC A, TODO: idem as 0x1C
            {
                ++m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.a, 3));

                break;
            }
            case 0x3D: //DEC A, TODO: idem as 0x0D
            {
                --m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.a, 3));

                break;
            }
            case 0x3E: //LD A, n8
            {
                m_registers.a = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x3F: //CCF
            {
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     !m_registers.flag(Registers::Flag::Carry));

                break;
            }



            case 0x40: //LD B, B
            {
                m_registers.b = m_registers.b;

                break;
            }
            case 0x41: //LD B, C
            {
                m_registers.b = m_registers.c;

                break;
            }
            case 0x42: //LD B, D
            {
                m_registers.b = m_registers.d;

                break;
            }
            case 0x43: //LD B, E
            {
                m_registers.b = m_registers.e;

                break;
            }
            case 0x44: //LD B, H
            {
                m_registers.b = m_registers.h;

                break;
            }
            case 0x45: //LD B, L
            {
                m_registers.b = m_registers.l;

                break;
            }
            case 0x46: //LD B, [HL]
            {
                m_registers.b = m_memory.at(m_registers.hl);

                break;
            }
            case 0x47: //LD B, A
            {
                m_registers.b = m_registers.a;

                break;
            }
            case 0x48: //LD C, B
            {
                m_registers.c = m_registers.b;

                break;
            }
            case 0x49: //LD C, C
            {
                m_registers.c = m_registers.c;

                break;
            }
            case 0x4A: //LD C, D
            {
                m_registers.c = m_registers.d;

                break;
            }
            case 0x4B: //LD C, E
            {
                m_registers.c = m_registers.e;

                break;
            }
            case 0x4C: //LD C, H
            {
                m_registers.c = m_registers.h;

                break;
            }
            case 0x4D: //LD C, L
            {
                m_registers.c = m_registers.l;

                break;
            }
            case 0x4E: //LD C, [HL]
            {
                m_registers.c = m_memory.at(m_registers.hl);

                break;
            }
            case 0x4F: //LD C, A
            {
                m_registers.c = m_registers.a;

                break;
            }



            case 0x50: //LD D, B
            {
                m_registers.d = m_registers.b;

                break;
            }
            case 0x51: //LD D, C
            {
                m_registers.d = m_registers.c;

                break;
            }
            case 0x52: //LD D, D
            {
                m_registers.d = m_registers.d;

                break;
            }
            case 0x53: //LD D, E
            {
                m_registers.d = m_registers.e;

                break;
            }
            case 0x54: //LD D, H
            {
                m_registers.d = m_registers.h;

                break;
            }
            case 0x55: //LD D, L
            {
                m_registers.d = m_registers.l;

                break;
            }
            case 0x56: //LD D, [HL]
            {
                m_registers.d = m_memory.at(m_registers.hl);

                break;
            }
            case 0x57: //LD D, A
            {
                m_registers.d = m_registers.a;

                break;
            }
            case 0x58: //LD E, B
            {
                m_registers.e = m_registers.b;

                break;
            }
            case 0x59: //LD E, C
            {
                m_registers.e = m_registers.c;

                break;
            }
            case 0x5A: //LD E, D
            {
                m_registers.e = m_registers.d;

                break;
            }
            case 0x5B: //LD E, E
            {
                m_registers.e = m_registers.e;

                break;
            }
            case 0x5C: //LD E, H
            {
                m_registers.e = m_registers.h;

                break;
            }
            case 0x5D: //LD E, L
            {
                m_registers.e = m_registers.l;

                break;
            }
            case 0x5E: //LD E, [HL]
            {
                m_registers.e = m_memory.at(m_registers.hl);

                break;
            }
            case 0x5F: //LD E, A
            {
                m_registers.e = m_registers.a;

                break;
            }



            case 0x60: //LD H, B
            {
                m_registers.h = m_registers.b;

                break;
            }
            case 0x61: //LD H, C
            {
                m_registers.h = m_registers.c;

                break;
            }
            case 0x62: //LD H, D
            {
                m_registers.h = m_registers.d;

                break;
            }
            case 0x63: //LD H, E
            {
                m_registers.h = m_registers.e;

                break;
            }
            case 0x64: //LD H, H
            {
                m_registers.h = m_registers.h;

                break;
            }
            case 0x65: //LD H, L
            {
                m_registers.h = m_registers.l;

                break;
            }
            case 0x66: //LD H, [HL]
            {
                m_registers.h = m_memory.at(m_registers.hl);

                break;
            }
            case 0x67: //LD H, A
            {
                m_registers.h = m_registers.a;

                break;
            }
            case 0x68: //LD L, B
            {
                m_registers.l = m_registers.b;

                break;
            }
            case 0x69: //LD L, C
            {
                m_registers.l = m_registers.c;

                break;
            }
            case 0x6A: //LD L, D
            {
                m_registers.l = m_registers.d;

                break;
            }
            case 0x6B: //LD L, E
            {
                m_registers.l = m_registers.e;

                break;
            }
            case 0x6C: //LD L, H
            {
                m_registers.l = m_registers.h;

                break;
            }
            case 0x6D: //LD L, L
            {
                m_registers.l = m_registers.l;

                break;
            }
            case 0x6E: //LD L, [HL]
            {
                m_registers.l = m_memory.at(m_registers.hl);

                break;
            }
            case 0x6F: //LD L, A
            {
                m_registers.l = m_registers.a;

                break;
            }



            case 0x70: //LD [HL], B
            {
                m_memory.at(m_registers.hl) = m_registers.b;

                break;
            }
            case 0x71: //LD [HL], C
            {
                m_memory.at(m_registers.hl) = m_registers.c;

                break;
            }
            case 0x72: //LD [HL], D
            {
                m_memory.at(m_registers.hl) = m_registers.d;

                break;
            }
            case 0x73: //LD [HL], E
            {
                m_memory.at(m_registers.hl) = m_registers.e;

                break;
            }
            case 0x74: //LD [HL], H
            {
                m_memory.at(m_registers.hl) = m_registers.h;

                break;
            }
            case 0x75: //LD [HL], L
            {
                m_memory.at(m_registers.hl) = m_registers.l;

                break;
            }
            case 0x76: //HALT
            {
                halt();

                break;
            }
            case 0x77: //LD [HL], A
            {
                m_memory.at(m_registers.hl) = m_registers.a;

                break;
            }
            case 0x78: //LD A, B
            {
                m_registers.a = m_registers.b;

                break;
            }
            case 0x79: //LD A, C
            {
                m_registers.a = m_registers.c;

                break;
            }
            case 0x7A: //LD A, D
            {
                m_registers.a = m_registers.d;

                break;
            }
            case 0x7B: //LD A, E
            {
                m_registers.a = m_registers.e;

                break;
            }
            case 0x7C: //LD A, H
            {
                m_registers.a = m_registers.h;

                break;
            }
            case 0x7D: //LD A, L
            {
                m_registers.a = m_registers.l;

                break;
            }
            case 0x7E: //LD A, [HL]
            {
                m_registers.a = m_memory.at(m_registers.hl);

                break;
            }
            case 0x7F: //LD A, A
            {
                m_registers.a = m_registers.a;

                break;
            }



            case 0x80: //ADD A, B
            {
                m_registers.a += m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x81: //ADD A, C
            {
                m_registers.a += m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x82: //ADD A, D
            {
                m_registers.a += m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x83: //ADD A, E
            {
                m_registers.a += m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x84: //ADD A, H
            {
                m_registers.a += m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x85: //ADD A, L
            {
                m_registers.a += m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x86: //ADD A, [HL]
            {
                m_registers.a += m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x87: //ADD A, A
            {
                m_registers.a += m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x88: //ADC A, B
            {
                m_registers.a += m_registers.b + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x89: //ADC A, C
            {
                m_registers.a += m_registers.c + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x8A: //ADC A, D
            {
                m_registers.a += m_registers.d + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x8B: //ADC A, E
            {
                m_registers.a += m_registers.e + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x8C: //ADC A, H
            {
                m_registers.a += m_registers.h + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x8D: //ADC A, L
            {
                m_registers.a += m_registers.l + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x8E: //ADC A, [HL]
            {
                m_registers.a += m_memory.at(m_registers.hl) + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x8F: //ADC A, A
            {
                m_registers.a += m_registers.a + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }



            case 0x90: //SUB A, B
            {
                m_registers.a -= m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x91: //SUB A, C
            {
                m_registers.a -= m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero, m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative, false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry, (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x92: //SUB A, D
            {
                m_registers.a -= m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero, m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative, false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry, (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x93: //SUB A, E
            {
                m_registers.a -= m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x94: //SUB A, H
            {
                m_registers.a -= m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x95: //SUB A, L
            {
                m_registers.a -= m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x96: //SUB A, [HL]
            {
                m_registers.a -= m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x97: //SUB A, A
            {
                m_registers.a -= m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x98: //SBC A, B
            {
                m_registers.a -= m_registers.b + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x99: //SBC A, C
            {
                m_registers.a -= m_registers.c + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x9A: //SBC A, D
            {
                m_registers.a -= m_registers.d + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x9B: //SBC A, E
            {
                m_registers.a -= m_registers.e + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x9C: //SBC A, H
            {
                m_registers.a -= m_registers.h + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x9D: //SBC A, L
            {
                m_registers.a -= m_registers.l + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x9E: //SBC A, [HL]
            {
                m_registers.a -= m_memory.at(m_registers.hl) + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0x9F: //SBC A, A
            {
                m_registers.a -= m_registers.a + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }



            case 0xA0: //AND A, B
            {
                m_registers.a &= m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA1: //AND A, C
            {
                m_registers.a &= m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA2: //AND A, D
            {
                m_registers.a &= m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA3: //AND A, E
            {
                m_registers.a &= m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA4: //AND A, H
            {
                m_registers.a &= m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA5: //AND A, L
            {
                m_registers.a &= m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA6: //AND A, [HL]
            {
                m_registers.a &= m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA7: //AND A, A
            {
                m_registers.a &= m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA8: //XOR A, B
            {
                m_registers.a ^= m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xA9: //XOR A, C
            {
                m_registers.a ^= m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xAA: //XOR A, D
            {
                m_registers.a ^= m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xAB: //XOR A, E
            {
                m_registers.a ^= m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xAC: //XOR A, H
            {
                m_registers.a ^= m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xAD: //XOR A, L
            {
                m_registers.a ^= m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xAE: //XOR A, [HL]
            {
                m_registers.a ^= m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xAF: //XOR A, A
            {
                m_registers.a ^= m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,      true);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }



            case 0xB0: //OR A, B
            {
                m_registers.a |= m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB1: //OR A, C
            {
                m_registers.a |= m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB2: //OR A, D
            {
                m_registers.a |= m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB3: //OR A, E
            {
                m_registers.a |= m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB4: //OR A, H
            {
                m_registers.a |= m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB5: //OR A, L
            {
                m_registers.a |= m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB6: //OR A, [HL]
            {
                m_registers.a |= m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB7: //OR A, A
            {
                m_registers.a |= m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xB8: //CP A, B
            {
                const auto& result = m_registers.a - m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xB9: //CP A, C
            {
                const auto& result = m_registers.a - m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xBA: //CP A, D
            {
                const auto& result = m_registers.a - m_registers.d;

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xBB: //CP A, E
            {
                const auto& result = m_registers.a - m_registers.e;

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xBC: //CP A, H
            {
                const auto& result = m_registers.a - m_registers.h;

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xBD: //CP A, L
            {
                const auto& result = m_registers.a - m_registers.l;

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xBE: //CP A, [HL]
            {
                const auto& result = m_registers.a - m_memory.at(m_registers.hl);

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xBF: //CP A, A
            {
                const auto& result = m_registers.a - m_registers.a;

                m_registers.set_flag(Registers::Flag::Zero,      true);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }



            case 0xC0: //RET NZ
            {
                if (!m_registers.flag(Registers::Flag::Zero))
                {
                    const auto& lsb = m_memory.at(m_registers.sp++);
                    const auto& msb = m_memory.at(m_registers.sp++);

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xC1: //POP BC
            {
                const auto& lsb = m_memory.at(m_registers.sp++);
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.bc = (msb << 8) | lsb;

                break;
            }
            case 0xC2: //JP NZ, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (!m_registers.flag(Registers::Flag::Zero)) m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xC3: //JP a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xC4: //CALL NZ, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (!m_registers.flag(Registers::Flag::Zero))
                {
                    --m_registers.sp;

                    m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                    m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xC5: //PUSH BC
            {
                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.bc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.bc       & 0x00FF;

                break;
            }
            case 0xC6: //ADD A, n8
            {
                m_registers.a += m_memory.at(m_registers.pc++);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0xC7: //RST $00
            {
                const word address = 0x00;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }
            case 0xC8: //RET Z
            {
                if (m_registers.flag(Registers::Flag::Zero))
                {
                    const auto& lsb = m_memory.at(m_registers.sp++);
                    const auto& msb = m_memory.at(m_registers.sp++);

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xC9: //RET
            {
                const auto& lsb = m_memory.at(m_registers.sp++);
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xCA: //JP Z, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (m_registers.flag(Registers::Flag::Zero)) m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xCB: //PREFIX
            {
                //TODO: See Prefixed graph below opcodes
                //Also important: https://www.reddit.com/r/EmuDev/comments/7qf352/comment/dsp9jtx/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button

                break;
            }
            case 0xCC: //CALL Z, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (m_registers.flag(Registers::Flag::Zero))
                {
                    --m_registers.sp;

                    m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                    m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xCD: //CALL a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xCE: //ADC A, n8
            {
                m_registers.a += m_memory.at(m_registers.pc++) + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0xCF: //RST $08
            {
                const word address = 0x08;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }



            case 0xD0: //RET NC
            {
                if (!m_registers.flag(Registers::Flag::Carry))
                {
                    const auto& lsb = m_memory.at(m_registers.sp++);
                    const auto& msb = m_memory.at(m_registers.sp++);

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xD1: //POP DE
            {
                const auto& lsb = m_memory.at(m_registers.sp++);
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.de = (msb << 8) | lsb;

                break;
            }
            case 0xD2: //JP NC, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (!m_registers.flag(Registers::Flag::Carry)) m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xD3: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xD4: //CALL NC, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (!m_registers.flag(Registers::Flag::Carry))
                {
                    --m_registers.sp;

                    m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                    m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xD5: //PUSH DE
            {
                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.de >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.de       & 0x00FF;

                break;
            }
            case 0xD6: //SUB A, n8
            {
                m_registers.a -= m_memory.at(m_registers.pc++);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0xD7: //RST $10
            {
                const word address = 0x10;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }
            case 0xD8: //RET C
            {
                if (m_registers.flag(Registers::Flag::Carry))
                {
                    const auto& lsb = m_memory.at(m_registers.sp++);
                    const auto& msb = m_memory.at(m_registers.sp++);

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xD9: //RETI
            {
                const auto& lsb = m_memory.at(m_registers.sp++);
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.pc = (msb << 8) | lsb;
                m_ime          = true;

                break;
            }
            case 0xDA: //JP C, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (m_registers.flag(Registers::Flag::Carry)) m_registers.pc = (msb << 8) | lsb;

                break;
            }
            case 0xDB: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xDC: //CALL C, a16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                if (m_registers.flag(Registers::Flag::Carry))
                {
                    --m_registers.sp;

                    m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                    m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                    m_registers.pc = (msb << 8) | lsb;
                }

                break;
            }
            case 0xDD: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xDE: //SBC A, n8
            {
                m_registers.a -= m_memory.at(m_registers.pc++) + m_registers.flag(Registers::Flag::Carry);

                m_registers.set_flag(Registers::Flag::Zero,       m_registers.a         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.a & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.a & 0xF0) == 0);

                break;
            }
            case 0xDF: //RST $18
            {
                const word address = 0x18;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }



            case 0xE0: //LDH [a8], A
            {
                const auto& value = m_memory.at(m_registers.pc++);
                const word address = (0xFF << 8) | value;

                m_memory.at(address) = m_registers.a;

                break;
            }
            case 0xE1: //POP HL
            {
                const auto& lsb = m_memory.at(m_registers.sp++);
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.hl = (msb << 8) | lsb;

                break;
            }
            case 0xE2: //LD [C], A
            {
                const word address = (0xFF << 8) | m_registers.c;

                m_memory.at(address) = m_registers.a;

                break;
            }
            case 0xE3: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xE4: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xE5: //PUSH HL
            {
                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.hl >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.hl       & 0x00FF;

                break;
            }
            case 0xE6: //AND A, n8
            {
                m_registers.a &= m_memory.at(m_registers.pc++);

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xE7: //RST $20
            {
                const word address = 0x20;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }
            case 0xE8: //ADD SP, e8
            {
                const auto& e = static_cast<std::int8_t>(m_registers.pc++);

                m_registers.sp += e;

                m_registers.set_flag(Registers::Flag::Zero,      false);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.sp & 0x0F) == 0); //TODO: might be wrong
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.sp & 0xF0) == 0);

                break;
            }
            case 0xE9: //JP HL
            {
                m_registers.pc = m_registers.hl;

                break;
            }
            case 0xEA: //LDH [a16], A
            {
                const auto& lsb     = m_memory.at(m_registers.pc++);
                const auto& msb     = m_memory.at(m_registers.pc++);
                const word address = (msb << 8) | lsb;

                m_memory.at(address) = m_registers.a;

                break;
            }
            case 0xEB: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xEC: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xED: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xEE: //XOR A, n8
            {
                m_registers.a ^= m_memory.at(m_registers.pc++);

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xEF: //RST $28
            {
                const word address = 0x28;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }



            case 0xF0: //LDH A, [a8]
            {
                const auto& value   = m_memory.at(m_registers.pc++);
                const word address = (0xFF << 8) | value;

                m_registers.a = m_memory.at(address);

                break;
            }
            case 0xF1: //POP AF
            {
                const auto& lsb = m_memory.at(m_registers.sp++) & 0xF0;
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.af = (msb << 8) | lsb;

                break;
            }
            case 0xF2: //LD A, [C]
            {
                const word address = (0xFF << 8) | m_registers.c;

                m_registers.a = m_memory.at(address);

                break;
            }
            case 0xF3: //DI
            {
                m_ime = false;
                //TODO: "Canceling any  scheduled effects of the EI instruction if any"

                break;
            }
            case 0xF4: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xF5: //PUSH AF
            {
                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.af >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.af       & 0x00FF;

                break;
            }
            case 0xF6: //OR A, n8
            {
                m_registers.a |= m_memory.at(m_registers.pc++);

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, true);
                m_registers.set_flag(Registers::Flag::Carry,     false);

                break;
            }
            case 0xF7: //RST $30
            {
                const word address = 0x30;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }
            case 0xF8: //LD HL, SP + e8
            {
                const auto& value = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

                m_registers.hl += m_registers.sp + value;

                m_registers.set_flag(Registers::Flag::Zero,       false);
                m_registers.set_flag(Registers::Flag::Negative,   false);
                m_registers.set_flag(Registers::Flag::HalfCarry, (m_registers.hl & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (m_registers.hl & 0xF0) == 0);

                break;
            }
            case 0xF9: //LD SP, HL
            {
                m_registers.sp = m_registers.hl;

                break;
            }
            case 0xFA: //LDH A, [a16]
            {
                const auto& lsb     = m_memory.at(m_registers.pc++);
                const auto& msb     = m_memory.at(m_registers.pc++);
                const word  address = (msb << 8) | lsb;

                 m_registers.a = m_memory.at(address);

                break;
            }
            case 0xFB: //EI
            {
                m_imeNext = true;

                break;
            }
            case 0xFC: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xFD: //
            {
                //Instruction does not exist

                __debugbreak();

                break;
            }
            case 0xFE: //CP A, n8
            {
                const auto& result = m_registers.a - m_memory.at(m_registers.pc++);

                m_registers.set_flag(Registers::Flag::Zero,       result         == 0);
                m_registers.set_flag(Registers::Flag::Negative,   true);
                m_registers.set_flag(Registers::Flag::HalfCarry, (result & 0x0F) == 0);
                m_registers.set_flag(Registers::Flag::Carry,     (result & 0xF0) == 0);

                break;
            }
            case 0xFF: //RST $38
            {
                const word address = 0x38;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }
        }

        if (m_registers.pc > 0x100)
        {
            std::cout << std::format("{}\n", OPCODE_NAMES_D.at(m_opcode));
            print_register_diff(m_registers, m_lastRegisters);
            return;
        }
    }



    void SharpSM83::nop()
    {

    }
    void SharpSM83::ld_r16_d16(word& r16)
    {
        const auto& lsb = m_memory.at(m_registers.pc++);
        const auto& msb = m_memory.at(m_registers.pc++);

        r16 = shifted_composite<word>(msb, lsb);
    }
    void SharpSM83::ld_r16_mem_a(word r16)
    {
        m_memory.at(r16) = m_registers.a;
    }
    void SharpSM83::ld_a_r16_mem(word r16)
    {
        m_registers.a = m_memory.at(r16);
    }
    void SharpSM83::ld_d16_sp()
    {
        const auto& lsb     = m_memory.at(m_registers.pc++);
        const auto& msb     = m_memory.at(m_registers.pc++);
        const auto& address = shifted_composite<word>(msb, lsb);

        m_memory.at(address)     = static_cast<byte>( m_registers.sp       & 0x00FF);
        m_memory.at(address + 1) = static_cast<byte>((m_registers.sp >> 8) & 0x00FF);
    }
    void SharpSM83::inc_r16(word& r16)
    {
        ++r16;
    }
    void SharpSM83::dec_r16(word& r16)
    {
        --r16;
    }
    void SharpSM83::add_hl_r16(word r16)
    {
        const word result = m_registers.hl + r16;

        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(m_registers.hl, r16, 12));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_add(m_registers.hl, r16, 16));

        m_registers.hl = result;
    }
    void SharpSM83::inc_r8(byte& r8)
{
        const byte result = r8 + byte{ 1u };

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(r8, byte{ 1u }, 4));

        r8 = result;
    }
    void SharpSM83::dec_r8(byte& r8)
{
        const byte result = r8 - 1;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(r8, byte{ 1u }, 4));

        r8 = result;
    }
    void SharpSM83::ld_r8_d8(byte& r8)
    {
        r8 = m_memory.at(m_registers.pc++);
    }
    void SharpSM83::rlca()
    {
        const auto& carryFlag = (m_registers.a >> 7) & 0x1;

        m_registers.set_flag(Registers::Flag::Zero,      false);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     carryFlag);

        m_registers.a = shift(m_registers.a, ShiftType::ROR, 7);
    }
    void SharpSM83::rrca()
    {
        const auto& carryFlag = m_registers.a & 0x1;

        m_registers.set_flag(Registers::Flag::Zero,      false);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     carryFlag);

        m_registers.a = shift(m_registers.a, ShiftType::ROR, 1);
    }
    void SharpSM83::rla()
    {
        const auto& carryFlag = (m_registers.a >> 7) & 0x1;
        const auto& result    = static_cast<byte>((m_registers.a << 1) | m_registers.flag(Registers::Flag::Carry));

        m_registers.set_flag(Registers::Flag::Zero,      false);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     carryFlag);

        m_registers.a = result;
    }
    void SharpSM83::rra()
    {
        const auto& carryFlag = (m_registers.a >> 7) & 0x1;
        const auto& result    = static_cast<byte>((m_registers.flag(Registers::Flag::Carry) << 7) | (m_registers.a >> 1));

        m_registers.set_flag(Registers::Flag::Zero,      false);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     carryFlag);

        m_registers.a = result;
    }
    void SharpSM83::daa() //DAA https://forums.nesdev.org/viewtopic.php?p=196282&sid=23337b4c00337de146414df0713eb703#p196282
    {
        if (!m_registers.flag(Registers::Flag::Negative))                      //After an addition, adjust if (half-)carry occurred or if result is out of bounds
        {
            if (m_registers.flag(Registers::Flag::Carry) || m_registers.a > 0x99)
            {
                m_registers.a += 0x60;

                m_registers.set_flag(Registers::Flag::Carry, true);
            }
            if (m_registers.flag(Registers::Flag::HalfCarry) || (m_registers.a & 0x0F) > 0x09)
            {
                m_registers.a += 0x06;
            }
        }
        else                                                                   //After a subtraction, only adjust if (half-)carry occurred
        {
            if (m_registers.flag(Registers::Flag::Carry))     m_registers.a -= 0x60;
            if (m_registers.flag(Registers::Flag::HalfCarry)) m_registers.a -= 0x06;
        }

        m_registers.set_flag(Registers::Flag::Zero,      m_registers.a == 0);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
    }
    void SharpSM83::cpl()
    {
        m_registers.a = ~m_registers.a;

        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, true);
    }
    void SharpSM83::scf()
    {
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     true);
    }
    void SharpSM83::ccf()
    {
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     !m_registers.flag(Registers::Flag::Carry));
    }
    void SharpSM83::jr_d8()
    {
        const byte address = m_memory.at(m_registers.pc++);

        m_registers.pc += address;
    }
    void SharpSM83::jr_cond_d8(bool flag)
    {
        const byte address = m_memory.at(m_registers.pc++);

        if (flag) m_registers.pc += address;
    }
    void SharpSM83::stop()
    {
        //TODO
        //https://gbdev.io/pandocs/Reducing_Power_Consumption.html

        __debugbreak();
    }
    void SharpSM83::ld_r8_r8(byte& a, byte& b)
    {
        a = b;
    }
    void SharpSM83::halt()
    {
        //TODO

        __debugbreak();
    }
    void SharpSM83::add_a_r8(byte r8)
    {
        const byte result = m_registers.a + r8;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(m_registers.a, r8, 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_add(m_registers.a, r8, 8));

        m_registers.a = result;
    }
    void SharpSM83::adc_a_r8(byte r8)
    {
        const auto& carryFlag = m_registers.flag(Registers::Flag::Carry);
        const byte  result    = m_registers.a + r8 + carryFlag;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(m_registers.a, static_cast<byte>(r8 + carryFlag), 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_add(m_registers.a, static_cast<byte>(r8 + carryFlag), 8));

        m_registers.a = result;
    }
    void SharpSM83::sub_a_r8(byte r8)
    {
        const byte result = m_registers.a - r8;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(m_registers.a, r8, 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_sub(m_registers.a, r8, 8));

        m_registers.a = result;
    }
    void SharpSM83::sbc_a_r8(byte r8)
    {
        const auto& carryFlag = m_registers.flag(Registers::Flag::Carry);
        const byte  result    = m_registers.a - r8 - carryFlag;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(m_registers.a, static_cast<byte>(r8 + carryFlag), 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_sub(m_registers.a, static_cast<byte>(r8 + carryFlag), 8));

        m_registers.a = result;
    }
    void SharpSM83::and_a_r8(byte r8)
    {
        const byte result = m_registers.a & r8;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, true);
        m_registers.set_flag(Registers::Flag::Carry,     false);

        m_registers.a = result;
    }
    void SharpSM83::xor_a_r8(byte r8)
    {
        const byte result = m_registers.a ^ r8;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     false);

        m_registers.a = result;
    }
    void SharpSM83::or_a_r8(byte r8)
    {
        const byte result = m_registers.a | r8;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     false);

        m_registers.a = result;
    }
    void SharpSM83::cp_a_r8(byte r8)
    {
        const byte result = m_registers.a - r8;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(m_registers.a, r8, 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_sub(m_registers.a, r8, 8));
    }
    void SharpSM83::add_a_d8()
    {
        const auto& value  = m_memory.at(m_registers.pc++);
        const byte  result = m_registers.a + value;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(m_registers.a, value, 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_add(m_registers.a, value, 8));

        m_registers.a = result;
    }
    void SharpSM83::adc_a_d8()
    {
        const auto& value     = m_memory.at(m_registers.pc++);
        const auto& carryFlag = m_registers.flag(Registers::Flag::Carry);
        const byte  result    = m_registers.a + value + carryFlag;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(m_registers.a, static_cast<byte>(value + carryFlag), 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_add(m_registers.a, static_cast<byte>(value + carryFlag), 8));

        m_registers.a = result;
    }
    void SharpSM83::sub_a_d8()
    {
        const auto& value  = m_memory.at(m_registers.pc++);
        const byte  result = m_registers.a - value;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(m_registers.a, value, 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_sub(m_registers.a, value, 8));

        m_registers.a = result;
    }
    void SharpSM83::sbc_a_d8()
    {
        const auto& value     = m_memory.at(m_registers.pc++);
        const auto& carryFlag = m_registers.flag(Registers::Flag::Carry);
        const byte  result    = m_registers.a - value - carryFlag;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(m_registers.a, static_cast<byte>(value + carryFlag), 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_sub(m_registers.a, static_cast<byte>(value + carryFlag), 8));

        m_registers.a = result;
    }
    void SharpSM83::and_a_d8()
    {
        const auto& value  = m_memory.at(m_registers.pc++);
        const byte  result = m_registers.a & value;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, true);
        m_registers.set_flag(Registers::Flag::Carry,     false);

        m_registers.a = result;
    }
    void SharpSM83::xor_a_d8()
    {
        const auto& value  = m_memory.at(m_registers.pc++);
        const byte  result = m_registers.a ^ value;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     false);

        m_registers.a = result;
    }
    void SharpSM83::or_a_d8()
    {
        const auto& value  = m_memory.at(m_registers.pc++);
        const byte  result = m_registers.a | value;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, false);
        m_registers.set_flag(Registers::Flag::Carry,     false);

        m_registers.a = result;
    }
    void SharpSM83::cp_a_d8()
    {
        const auto& value  = m_memory.at(m_registers.pc++);
        const byte  result = m_registers.a - value;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(m_registers.a, value, 4));
        m_registers.set_flag(Registers::Flag::Carry,     carry_occurred_sub(m_registers.a, value, 8));
    }
    void SharpSM83::ret_cond(bool flag)
    {
        if (flag)
        {
            const auto& lsb = m_memory.at(m_registers.sp++);
            const auto& msb = m_memory.at(m_registers.sp++);

            m_registers.pc = shifted_composite<word>(msb, lsb);
        }
    }
    void SharpSM83::ret()
    {
        const auto& lsb = m_memory.at(m_registers.sp++);
        const auto& msb = m_memory.at(m_registers.sp++);

        m_registers.pc = shifted_composite<word>(msb, lsb);
    }
    void SharpSM83::reti()
    {
        const auto& lsb = m_memory.at(m_registers.sp++);
        const auto& msb = m_memory.at(m_registers.sp++);

        m_registers.pc = shifted_composite<word>(msb, lsb);
        m_ime          = true;
    }
    void SharpSM83::jp_cond_imm16(bool flag)
    {
        const auto& lsb = m_memory.at(m_registers.pc++);
        const auto& msb = m_memory.at(m_registers.pc++);

        if (flag) m_registers.pc = shifted_composite<word>(msb, lsb);
    }
    void SharpSM83::jp_imm16()
    {
        const auto& lsb = m_memory.at(m_registers.pc++);
        const auto& msb = m_memory.at(m_registers.pc++);

        m_registers.pc = shifted_composite<word>(msb, lsb);
    }
    void SharpSM83::jp_hl()
    {
        m_registers.pc = m_registers.hl;
    }
    void SharpSM83::call_cond_imm16(bool flag)
    {
        const auto& lsb = m_memory.at(m_registers.pc++);
        const auto& msb = m_memory.at(m_registers.pc++);

        if (flag)
        {
            --m_registers.sp;

            m_memory.at(m_registers.sp--) = static_cast<byte>((m_registers.pc >> 8) & 0x00FF);
            m_memory.at(m_registers.sp)   = static_cast<byte>( m_registers.pc       & 0x00FF);

            m_registers.pc = shifted_composite<word>(msb, lsb);
        }
    }
    void SharpSM83::call_imm16()
    {
        const auto& lsb = m_memory.at(m_registers.pc++);
        const auto& msb = m_memory.at(m_registers.pc++);

        --m_registers.sp;

        m_memory.at(m_registers.sp--) = static_cast<byte>((m_registers.pc >> 8) & 0x00FF);
        m_memory.at(m_registers.sp)   = static_cast<byte>( m_registers.pc       & 0x00FF);

        m_registers.pc = shifted_composite<word>(msb, lsb);
    }
    void SharpSM83::rst_tgt3(byte address)
    {
        --m_registers.sp;

        m_memory.at(m_registers.sp--) = static_cast<byte>((m_registers.pc >> 8) & 0x00FF);
        m_memory.at(m_registers.sp)   = static_cast<byte>( m_registers.pc       & 0x00FF);

        m_registers.pc = address;
    }
    void SharpSM83::pop_r16stk(word& r16)
    {
        const auto& lsb = m_memory.at(m_registers.sp++);
        const auto& msb = m_memory.at(m_registers.sp++);

        r16 = shifted_composite<word>(msb, lsb);
    }
    void SharpSM83::push_r16stk(word r16)
    {
        --m_registers.sp;

        m_memory.at(m_registers.sp--) = static_cast<byte>((r16 >> 8) & 0x00FF);
        m_memory.at(m_registers.sp)   = static_cast<byte>( r16       & 0x00FF);

    }
    void SharpSM83::prefix()
    {
        //ni hopen
        __debugbreak();
    }

    void SharpSM83::ldh_c_mem_a()
    {
        const auto& address = shifted_composite<word>(static_cast<byte>(0xFF), m_registers.c);

        m_memory.at(address) = m_registers.a;
    }
    void SharpSM83::ldh_imm8_mem_a()
    {
        const auto& value   = m_memory.at(m_registers.pc++);
        const auto& address = shifted_composite<word>(static_cast<byte>(0xFF), value);

        m_memory.at(address) = m_registers.a;
    }
    void SharpSM83::ld_imm16_mem_a()
    {
        const auto& lsb     = m_memory.at(m_registers.pc++);
        const auto& msb     = m_memory.at(m_registers.pc++);
        const auto& address = shifted_composite<word>(msb, lsb);

        m_memory.at(address) = m_registers.a;
    }
    void SharpSM83::ldh_a_c_mem()
    {
        const auto& address = shifted_composite<word>(static_cast<byte>(0xFF), m_registers.c);

        m_registers.a = m_memory.at(address);
    }
    void SharpSM83::ldh_a_imm8_mem()
    {
        const auto& value   = m_memory.at(m_registers.pc++);
        const auto& address = shifted_composite<word>(static_cast<byte>(0xFF), value);

        m_registers.a = m_memory.at(address);
    }
    void SharpSM83::ld_a_imm16_mem()
    {
        const auto& lsb     = m_memory.at(m_registers.pc++);
        const auto& msb     = m_memory.at(m_registers.pc++);
        const auto& address = shifted_composite<word>(msb, lsb);

        m_registers.a = m_memory.at(address);
    }

    void SharpSM83::add_sp_imm8()
    {
        const auto& value  = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));
        const auto& result = m_registers.sp + value;

        m_registers.set_flag(Registers::Flag::Zero,      false);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, (value & 0xFF) + (m_registers.sp & 0xFF) >= 0x100); //TODO: change to carry_occurred_xxx
        m_registers.set_flag(Registers::Flag::Carry,     (value & 0x0F) + (m_registers.sp & 0x0F) >= 0x010);  

        m_registers.sp = result;
    }
    void SharpSM83::ld_hl_sp_imm8()
    {
        const auto& value  = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));
        const auto& result = m_registers.sp + value;

        m_registers.set_flag(Registers::Flag::Zero,      false);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, (value & 0xFF) + (m_registers.sp & 0xFF) >= 0x100); //TODO: change to carry_occurred_xxx
        m_registers.set_flag(Registers::Flag::Carry,     (value & 0x0F) + (m_registers.sp & 0x0F) >= 0x010);  

        m_registers.hl = result;
    }
    void SharpSM83::ld_sp_hl()
    {
        m_registers.sp = m_registers.hl;
    }
    void SharpSM83::di()
    {
        m_ime = false;
    }
    void SharpSM83::ei()
    {
        m_ime = true;
    }
    void SharpSM83::invalid()
    {

    }
}
