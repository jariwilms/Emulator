#include "stdafx.hpp"

#include "SharpSM83.hpp"

namespace dot::gb
{
    void SharpSM83::fetch()
    {
        const auto& read = []() -> ins8_t
            {
                return 0;
            };

        m_opcode = read();
        ++m_registers.pc;
    }
    void SharpSM83::decode()
    {

    }
    void SharpSM83::execute()
    {
        switch (m_opcode)
        {
            case 0x00: //NOP
            {
                break;
            }
            case 0x01: //LD BC, n16
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                m_registers.bc = (msb << 8) | lsb;

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
                ++m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.b == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.b, 3));

                break;
            }
            case 0x05: //DEC B
            {
                --m_registers.b;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.b == 0);
                m_registers.set_flag(Registers::Flag::Negative,  true);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.b, 3));

                break;
            }
            case 0x06: //LD B, n8
            {
                m_registers.b = m_memory.at(m_registers.pc++);

                break;
            }
            case 0x07: //RLCA
            {
                m_registers.a = shift(m_registers.a, ShiftType::ROR, 7);

                m_registers.set_flag(Registers::Flag::Zero,      false);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, false);
                m_registers.set_flag(Registers::Flag::Carry,     get_bit(m_registers.a, 0));

                break;
            }
            case 0x08: //LD [a16], SP
            {
                const auto& lsb = m_memory.at(m_registers.pc++);
                const auto& msb = m_memory.at(m_registers.pc++);

                const word nn = (msb << 8) | lsb;

                m_memory.at(nn)     = static_cast<byte>(m_registers.sp);
                m_memory.at(nn + 1) = static_cast<byte>(m_registers.sp >> 8);

                break;
            }
            case 0x09: //ADD HL, BC
            {
                m_registers.hl += m_registers.bc;

                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.hl, 11));
                m_registers.set_flag(Registers::Flag::Carry,     get_bit(m_registers.hl, 15));

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
            case 0x0C: //INC C, TODO: idem as 0x04
            {
                ++m_registers.c;

                m_registers.set_flag(Registers::Flag::Zero,      m_registers.c == 0);
                m_registers.set_flag(Registers::Flag::Negative,  false);
                m_registers.set_flag(Registers::Flag::HalfCarry, get_bit(m_registers.c, 3));

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
                //TODO
                //https://gbdev.io/pandocs/Reducing_Power_Consumption.html

                __debugbreak();

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
                //TODO
                __debugbreak();

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
                const auto& address = 0x00;

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
                const auto& address = 0x08;

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
                const auto& address = 0x10;

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
                const auto& address = 0x18;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }



            case 0xE0: //LDH [a8], A
            {
                const auto& value = m_memory.at(m_registers.pc++);
                const auto& address = (0xFF << 8) | value;

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
                const auto& address = (0xFF << 8) | m_registers.c;

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
                const auto& address = 0x20;

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
                const auto& address = (msb << 8) | lsb;

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
                const auto& address = 0x28;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }



            case 0xF0: //LDH A, [a8]
            {
                const auto& value   = m_memory.at(m_registers.pc++);
                const auto& address = (0xFF << 8) | value;

                m_registers.a = m_memory.at(address);

                break;
            }
            case 0xF1: //POP AF
            {
                const auto& lsb = m_memory.at(m_registers.sp++);
                const auto& msb = m_memory.at(m_registers.sp++);

                m_registers.af = (msb << 8) | lsb;

                break;
            }
            case 0xF2: //LD A, [C]
            {
                const auto& address = (0xFF << 8) | m_registers.c;

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
                const auto& address = 0x30;

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
                const auto& address = 0x38;

                --m_registers.sp;

                m_memory.at(m_registers.sp--) = (m_registers.pc >> 8) & 0x00FF;
                m_memory.at(m_registers.sp)   =  m_registers.pc       & 0x00FF;

                m_registers.pc = address;

                break;
            }
        }
    }
}
