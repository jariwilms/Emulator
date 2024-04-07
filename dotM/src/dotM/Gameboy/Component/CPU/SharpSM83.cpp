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
            case 0x00: nop();                                                      break;
            case 0x01: ld_r16_d16(m_registers.bc);                                 break;
            case 0x02: ld_r16_mem_a(m_registers.bc);                               break;
            case 0x03: inc_r16(m_registers.bc);                                    break;
            case 0x04: inc_r8(m_registers.b);                                      break;
            case 0x05: dec_r8(m_registers.b);                                      break;
            case 0x06: ld_r8_d8(m_registers.b);                                    break;
            case 0x07: rlca();                                                     break;
            case 0x08: ld_imm16_mem_sp();                                          break;
            case 0x09: add_hl_r16(m_registers.bc);                                 break;
            case 0x0A: ld_a_r16_mem(m_registers.bc);                               break;
            case 0x0B: dec_r16(m_registers.bc);                                    break;
            case 0x0C: inc_r8(m_registers.c);                                      break;
            case 0x0D: dec_r8(m_registers.c);                                      break;
            case 0x0E: ld_r8_d8(m_registers.c);                                    break;
            case 0x0F: rrca();                                                     break;
                                                                                   
            case 0x10: stop();                                                     break;
            case 0x11: ld_r16_d16(m_registers.de);                                 break;
            case 0x12: ld_r16_mem_a(m_registers.de);                               break;
            case 0x13: inc_r16(m_registers.de);                                    break;
            case 0x14: inc_r8(m_registers.d);                                      break;
            case 0x15: dec_r8(m_registers.d);                                      break;
            case 0x16: ld_r8_d8(m_registers.d);                                    break;
            case 0x17: rla();                                                      break;
            case 0x18: jr_imm8();                                                  break;
            case 0x19: add_hl_r16(m_registers.de);                                 break;
            case 0x1A: ld_a_r16_mem(m_registers.de);                               break;
            case 0x1B: dec_r16(m_registers.de);                                    break;
            case 0x1C: inc_r8(m_registers.e);                                      break;
            case 0x1D: dec_r8(m_registers.e);                                      break;
            case 0x1E: ld_r8_d8(m_registers.e);                                    break;
            case 0x1F: rra();                                                      break;
                                                                                   
            case 0x20: jr_cond_imm8(!m_registers.flag(Registers::Flag::Zero));     break;
            case 0x21: ld_r16_d16(m_registers.hl);                                 break;
            case 0x22: ld_hli_mem_a();                                             break;
            case 0x23: inc_r16(m_registers.hl);                                    break;
            case 0x24: inc_r8(m_registers.h);                                      break;
            case 0x25: dec_r8(m_registers.h);                                      break;
            case 0x26: ld_r8_d8(m_registers.h);                                    break;
            case 0x27: daa();                                                      break;
            case 0x28: jr_cond_imm8(m_registers.flag(Registers::Flag::Zero));      break;
            case 0x29: add_hl_r16(m_registers.hl);                                 break;
            case 0x2A: ld_a_hli_mem();                                             break;
            case 0x2B: dec_r16(m_registers.hl);                                    break;
            case 0x2C: inc_r8(m_registers.l);                                      break;
            case 0x2D: dec_r8(m_registers.l);                                      break;
            case 0x2E: ld_r8_d8(m_registers.l);                                    break;
            case 0x2F: ccf();                                                      break;
                                                                                   
            case 0x30: jr_cond_imm8(!m_registers.flag(Registers::Flag::Carry));    break;
            case 0x31: ld_r16_d16(m_registers.sp);                                 break;
            case 0x32: ld_hld_mem_a();                                             break;
            case 0x33: inc_r16(m_registers.sp);                                    break;
            case 0x34: inc_hl_mem();                                               break;
            case 0x35: dec_hl_mem();                                               break;
            case 0x36: ld_hl_mem_d8();                                             break;
            case 0x37: scf();                                                      break;
            case 0x38: jr_cond_imm8(m_registers.flag(Registers::Flag::Carry));     break;
            case 0x39: add_hl_r16(m_registers.sp);                                 break;
            case 0x3A: ld_a_hld_mem();                                             break;
            case 0x3B: dec_r16(m_registers.sp);                                    break;
            case 0x3C: inc_r8(m_registers.a);                                      break;
            case 0x3D: dec_r8(m_registers.a);                                      break;
            case 0x3E: ld_r8_d8(m_registers.a);                                    break;
            case 0x3F: ccf();                                                      break;
                                                                                   
            case 0x40: ld_r8_r8(m_registers.b, m_registers.b);                     break;
            case 0x41: ld_r8_r8(m_registers.b, m_registers.c);                     break;
            case 0x42: ld_r8_r8(m_registers.b, m_registers.d);                     break;
            case 0x43: ld_r8_r8(m_registers.b, m_registers.e);                     break;
            case 0x44: ld_r8_r8(m_registers.b, m_registers.h);                     break;
            case 0x45: ld_r8_r8(m_registers.b, m_registers.l);                     break;
            case 0x46: ld_r8_r8(m_registers.b, m_memory.at(m_registers.hl));       break;
            case 0x47: ld_r8_r8(m_registers.b, m_registers.a);                     break;
            case 0x48: ld_r8_r8(m_registers.c, m_registers.b);                     break;
            case 0x49: ld_r8_r8(m_registers.c, m_registers.c);                     break;
            case 0x4A: ld_r8_r8(m_registers.c, m_registers.d);                     break;
            case 0x4B: ld_r8_r8(m_registers.c, m_registers.e);                     break;
            case 0x4C: ld_r8_r8(m_registers.c, m_registers.h);                     break;
            case 0x4D: ld_r8_r8(m_registers.c, m_registers.l);                     break;
            case 0x4E: ld_r8_r8(m_registers.c, m_memory.at(m_registers.hl));       break;
            case 0x4F: ld_r8_r8(m_registers.c, m_registers.a);                     break;
                                                                                   
            case 0x50: ld_r8_r8(m_registers.d, m_registers.b);                     break;
            case 0x51: ld_r8_r8(m_registers.d, m_registers.c);                     break;
            case 0x52: ld_r8_r8(m_registers.d, m_registers.d);                     break;
            case 0x53: ld_r8_r8(m_registers.d, m_registers.e);                     break;
            case 0x54: ld_r8_r8(m_registers.d, m_registers.h);                     break;
            case 0x55: ld_r8_r8(m_registers.d, m_registers.l);                     break;
            case 0x56: ld_r8_r8(m_registers.d, m_memory.at(m_registers.hl));       break;
            case 0x57: ld_r8_r8(m_registers.d, m_registers.a);                     break;
            case 0x58: ld_r8_r8(m_registers.e, m_registers.b);                     break;
            case 0x59: ld_r8_r8(m_registers.e, m_registers.c);                     break;
            case 0x5A: ld_r8_r8(m_registers.e, m_registers.d);                     break;
            case 0x5B: ld_r8_r8(m_registers.e, m_registers.e);                     break;
            case 0x5C: ld_r8_r8(m_registers.e, m_registers.h);                     break;
            case 0x5D: ld_r8_r8(m_registers.e, m_registers.l);                     break;
            case 0x5E: ld_r8_r8(m_registers.e, m_memory.at(m_registers.hl));       break;
            case 0x5F: ld_r8_r8(m_registers.e, m_registers.a);                     break;
                                                                                   
            case 0x60: ld_r8_r8(m_registers.h, m_registers.b);                     break;
            case 0x61: ld_r8_r8(m_registers.h, m_registers.c);                     break;
            case 0x62: ld_r8_r8(m_registers.h, m_registers.d);                     break;
            case 0x63: ld_r8_r8(m_registers.h, m_registers.e);                     break;
            case 0x64: ld_r8_r8(m_registers.h, m_registers.h);                     break;
            case 0x65: ld_r8_r8(m_registers.h, m_registers.l);                     break;
            case 0x66: ld_r8_r8(m_registers.h, m_memory.at(m_registers.hl));       break;
            case 0x67: ld_r8_r8(m_registers.h, m_registers.a);                     break;
            case 0x68: ld_r8_r8(m_registers.l, m_registers.b);                     break;
            case 0x69: ld_r8_r8(m_registers.l, m_registers.c);                     break;
            case 0x6A: ld_r8_r8(m_registers.l, m_registers.d);                     break;
            case 0x6B: ld_r8_r8(m_registers.l, m_registers.e);                     break;
            case 0x6C: ld_r8_r8(m_registers.l, m_registers.h);                     break;
            case 0x6D: ld_r8_r8(m_registers.l, m_registers.l);                     break;
            case 0x6E: ld_r8_r8(m_registers.l, m_memory.at(m_registers.hl));       break;
            case 0x6F: ld_r8_r8(m_registers.l, m_registers.a);                     break;

            case 0x70: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.b);       break;
            case 0x71: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.c);       break;
            case 0x72: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.d);       break;
            case 0x73: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.e);       break;
            case 0x74: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.h);       break;
            case 0x75: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.l);       break;
            case 0x76: halt();                                                     break;
            case 0x77: ld_r8_r8(m_memory.at(m_registers.hl), m_registers.a);       break;
            case 0x78: ld_r8_r8(m_registers.a, m_registers.b);                     break;
            case 0x79: ld_r8_r8(m_registers.a, m_registers.c);                     break;
            case 0x7A: ld_r8_r8(m_registers.a, m_registers.d);                     break;
            case 0x7B: ld_r8_r8(m_registers.a, m_registers.e);                     break;
            case 0x7C: ld_r8_r8(m_registers.a, m_registers.h);                     break;
            case 0x7D: ld_r8_r8(m_registers.a, m_registers.l);                     break;
            case 0x7E: ld_r8_r8(m_registers.a, m_memory.at(m_registers.hl));       break;
            case 0x7F: ld_r8_r8(m_registers.a, m_registers.a);                     break;
                                                                                   
            case 0x80: add_a_r8(m_registers.b);                                    break;
            case 0x81: add_a_r8(m_registers.c);                                    break;
            case 0x82: add_a_r8(m_registers.d);                                    break;
            case 0x83: add_a_r8(m_registers.e);                                    break;
            case 0x84: add_a_r8(m_registers.h);                                    break;
            case 0x85: add_a_r8(m_registers.l);                                    break;
            case 0x86: add_a_r8(m_memory.at(m_registers.hl));                      break;
            case 0x87: add_a_r8(m_registers.a);                                    break;
            case 0x88: adc_a_r8(m_registers.b);                                    break;
            case 0x89: adc_a_r8(m_registers.c);                                    break;
            case 0x8A: adc_a_r8(m_registers.d);                                    break;
            case 0x8B: adc_a_r8(m_registers.e);                                    break;
            case 0x8C: adc_a_r8(m_registers.h);                                    break;
            case 0x8D: adc_a_r8(m_registers.l);                                    break;
            case 0x8E: adc_a_r8(m_memory.at(m_registers.hl));                      break;
            case 0x8F: adc_a_r8(m_registers.a);                                    break;
                                                                                   
            case 0x90: sub_a_r8(m_registers.b);                                    break;
            case 0x91: sub_a_r8(m_registers.c);                                    break;
            case 0x92: sub_a_r8(m_registers.d);                                    break;
            case 0x93: sub_a_r8(m_registers.e);                                    break;
            case 0x94: sub_a_r8(m_registers.h);                                    break;
            case 0x95: sub_a_r8(m_registers.l);                                    break;
            case 0x96: sub_a_r8(m_memory.at(m_registers.hl));                      break;
            case 0x97: sub_a_r8(m_registers.a);                                    break;
            case 0x98: sbc_a_r8(m_registers.b);                                    break;
            case 0x99: sbc_a_r8(m_registers.c);                                    break;
            case 0x9A: sbc_a_r8(m_registers.d);                                    break;
            case 0x9B: sbc_a_r8(m_registers.e);                                    break;
            case 0x9C: sbc_a_r8(m_registers.h);                                    break;
            case 0x9D: sbc_a_r8(m_registers.l);                                    break;
            case 0x9E: sbc_a_r8(m_memory.at(m_registers.hl));                      break;
            case 0x9F: sbc_a_r8(m_registers.a);                                    break;
                                                                                   
            case 0xA0: and_a_r8(m_registers.b);                                    break;
            case 0xA1: and_a_r8(m_registers.c);                                    break;
            case 0xA2: and_a_r8(m_registers.d);                                    break;
            case 0xA3: and_a_r8(m_registers.e);                                    break;
            case 0xA4: and_a_r8(m_registers.h);                                    break;
            case 0xA5: and_a_r8(m_registers.l);                                    break;
            case 0xA6: and_a_r8(m_memory.at(m_registers.hl));                      break;
            case 0xA7: and_a_r8(m_registers.a);                                    break;
            case 0xA8: xor_a_r8(m_registers.b);                                    break;
            case 0xA9: xor_a_r8(m_registers.c);                                    break;
            case 0xAA: xor_a_r8(m_registers.d);                                    break;
            case 0xAB: xor_a_r8(m_registers.e);                                    break;
            case 0xAC: xor_a_r8(m_registers.h);                                    break;
            case 0xAD: xor_a_r8(m_registers.l);                                    break;
            case 0xAE: xor_a_r8(m_memory.at(m_registers.hl));                      break;
            case 0xAF: xor_a_r8(m_registers.a);                                    break;
                                                                                   
            case 0xB0: or_a_r8(m_registers.b);                                     break;
            case 0xB1: or_a_r8(m_registers.c);                                     break;
            case 0xB2: or_a_r8(m_registers.d);                                     break;
            case 0xB3: or_a_r8(m_registers.e);                                     break;
            case 0xB4: or_a_r8(m_registers.h);                                     break;
            case 0xB5: or_a_r8(m_registers.l);                                     break;
            case 0xB6: or_a_r8(m_memory.at(m_registers.hl));                       break;
            case 0xB7: or_a_r8(m_registers.a);                                     break;
            case 0xB8: cp_a_r8(m_registers.b);                                     break;
            case 0xB9: cp_a_r8(m_registers.c);                                     break;
            case 0xBA: cp_a_r8(m_registers.d);                                     break;
            case 0xBB: cp_a_r8(m_registers.e);                                     break;
            case 0xBC: cp_a_r8(m_registers.h);                                     break;
            case 0xBD: cp_a_r8(m_registers.l);                                     break;
            case 0xBE: cp_a_r8(m_memory.at(m_registers.hl));                       break;
            case 0xBF: cp_a_r8(m_registers.a);                                     break;
                                                                                   
            case 0xC0: ret_cond(!m_registers.flag(Registers::Flag::Zero));         break;
            case 0xC1: pop_r16stk(m_registers.bc);                                 break;
            case 0xC2: jp_cond_imm16(!m_registers.flag(Registers::Flag::Zero));    break;
            case 0xC3: jp_imm16();                                                 break;
            case 0xC4: call_cond_imm16(!m_registers.flag(Registers::Flag::Zero));  break;
            case 0xC5: push_r16stk(m_registers.bc);                                break;
            case 0xC6: add_a_d8();                                                 break;
            case 0xC7: rst_tgt3(0x00);                                             break;
            case 0xC8: ret_cond(m_registers.flag(Registers::Flag::Zero));          break;
            case 0xC9: ret();                                                      break;
            case 0xCA: jp_cond_imm16(m_registers.flag(Registers::Flag::Zero));     break;
            case 0xCB: prefix();                                                   break;
            case 0xCC: call_cond_imm16(m_registers.flag(Registers::Flag::Zero));   break;
            case 0xCD: call_imm16();                                               break;
            case 0xCE: adc_a_d8();                                                 break;
            case 0xCF: rst_tgt3(0x08);                                             break;
                                                                                   
            case 0xD0: ret_cond(!m_registers.flag(Registers::Flag::Carry));        break;
            case 0xD1: pop_r16stk(m_registers.de);                                 break;
            case 0xD2: jp_cond_imm16(!m_registers.flag(Registers::Flag::Carry));   break;
            case 0xD3: invalid;                                                    break;
            case 0xD4: call_cond_imm16(!m_registers.flag(Registers::Flag::Carry)); break;
            case 0xD5: push_r16stk(m_registers.de);                                break;
            case 0xD6: sub_a_d8();                                                 break;
            case 0xD7: rst_tgt3(0x10);                                             break;
            case 0xD8: ret_cond(m_registers.flag(Registers::Flag::Carry));         break;
            case 0xD9: reti();                                                     break;
            case 0xDA: jp_cond_imm16(m_registers.flag(Registers::Flag::Carry));    break;
            case 0xDB: invalid();                                                  break;
            case 0xDC: call_cond_imm16(m_registers.flag(Registers::Flag::Carry));  break;
            case 0xDD: invalid();                                                  break;
            case 0xDE: sbc_a_d8();                                                 break;
            case 0xDF: rst_tgt3(0x18);                                             break;

            case 0xE0: ldh_imm8_mem_a();                                           break;
            case 0xE1: pop_r16stk(m_registers.hl);                                 break;
            case 0xE2: ldh_c_mem_a();                                              break;
            case 0xE3: invalid();                                                  break;
            case 0xE4: invalid();                                                  break;
            case 0xE5: push_r16stk(m_registers.hl);                                break;
            case 0xE6: and_a_d8();                                                 break;
            case 0xE7: rst_tgt3(0x20);                                             break;
            case 0xE8: add_sp_imm8();                                              break;
            case 0xE9: jp_hl_mem();                                                break;
            case 0xEA: ld_imm16_mem_a();                                           break;
            case 0xEB: invalid();                                                  break;
            case 0xEC: invalid();                                                  break;
            case 0xED: invalid();                                                  break;
            case 0xEE: xor_a_d8();                                                 break;
            case 0xEF: rst_tgt3(0x28);                                             break;

            case 0xF0: ldh_a_imm8_mem();                                           break;
            case 0xF1: pop_r16stk(m_registers.af);                                 break;
            case 0xF2: ldh_a_c_mem();                                              break;
            case 0xF3: di();                                                       break;
            case 0xF4: invalid();                                                  break;
            case 0xF5: push_r16stk(m_registers.af);                                break;
            case 0xF6: or_a_d8();                                                  break;
            case 0xF7: rst_tgt3(0x30);                                             break;
            case 0xF8: ld_hl_sp_imm8();                                            break;
            case 0xF9: ld_sp_hl();                                                 break;
            case 0xFA: ld_a_imm16_mem();                                           break;
            case 0xFB: ei();                                                       break;
            case 0xFC: invalid();                                                  break;
            case 0xFD: invalid();                                                  break;
            case 0xFE: cp_a_d8();                                                  break;
            case 0xFF: rst_tgt3(0x38);                                             break;

            default: throw std::invalid_argument{ "Invalid opcode!" };
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
    void SharpSM83::ld_imm16_mem_sp()
    {
        const auto& lsb     = m_memory.at(m_registers.pc++);
        const auto& msb     = m_memory.at(m_registers.pc++);
        const auto& address = shifted_composite<word>(msb, lsb);

        m_memory.at(address)     = static_cast<byte>( m_registers.sp       & 0x00FF);
        m_memory.at(address + 1) = static_cast<byte>((m_registers.sp >> 8) & 0x00FF);
    }
    void SharpSM83::ld_hli_mem_a()
    {
        m_memory.at(m_registers.hl++) = m_registers.a;
    }
    void SharpSM83::ld_hld_mem_a()
    {
        m_memory.at(m_registers.hl--) = m_registers.a;
    }
    void SharpSM83::ld_a_hli_mem()
    {
        m_registers.a = m_memory.at(m_registers.hl++);
    }
    void SharpSM83::ld_a_hld_mem()
    {
        m_registers.a = m_memory.at(m_registers.hl--);
    }
    void SharpSM83::ld_hl_mem_d8()
    {
        const auto& value = m_memory.at(m_registers.pc++);

        m_memory.at(m_registers.hl) = value;
    }
    void SharpSM83::inc_r16(word& r16)
    {
        ++r16;
    }
    void SharpSM83::dec_r16(word& r16)
    {
        --r16;
    }
    void SharpSM83::inc_hl_mem()
    {
        const auto& value  = m_memory.at(m_registers.hl);
        const auto& result = value + 1;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  false);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_add(value, static_cast<byte>(1u), 4));

        m_memory.at(m_registers.hl) = result;
    }
    void SharpSM83::dec_hl_mem()
    {
        const auto& value  = m_memory.at(m_registers.hl);
        const auto& result = value - 1;

        m_registers.set_flag(Registers::Flag::Zero,      result == 0);
        m_registers.set_flag(Registers::Flag::Negative,  true);
        m_registers.set_flag(Registers::Flag::HalfCarry, carry_occurred_sub(value, static_cast<byte>(1u), 4));

        m_memory.at(m_registers.hl) = result;
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
    void SharpSM83::jr_imm8()
    {
        const auto& address = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

        m_registers.pc += address;
    }
    void SharpSM83::jr_cond_imm8(bool flag)
    {
        const auto& address = static_cast<std::int8_t>(m_memory.at(m_registers.pc++));

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
    void SharpSM83::jp_hl_mem()
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
