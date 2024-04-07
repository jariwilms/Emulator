#pragma once

#include <stdafx.hpp>

#include "dotM/Gameboy/Architecture/IO.hpp"

namespace dot::gb
{
    class SharpSM83
    {
    public:
        SharpSM83(std::array<byte, 0x10000>& memory);
        ~SharpSM83() = default;

        void cycle();
        void reset();

        void signal_interrupt()
        {
            m_interruptSignaled = true;
        }

    //private:
        struct Registers
        {
        public:
            enum class Flag
            {
                Zero      = 7, 
                Negative  = 6, 
                HalfCarry = 5, 
                Carry     = 4, 
            };

            Registers() = default;

            void reset()
            {
                af = bc = de = hl = 0;
                sp = pc           = 0;
            }

            bool flag(Flag flag) const
            {
                switch (flag)
                {
                    case Flag::Zero:      return get_bit(f, 7);
                    case Flag::Negative:  return get_bit(f, 6);
                    case Flag::HalfCarry: return get_bit(f, 5);
                    case Flag::Carry:     return get_bit(f, 4);
                }

                throw std::invalid_argument{ "Invalid flag!" };
            }
            void set_flag(Flag flag, bool state)
            {
                const auto& index = static_cast<byte>(flag);
                f = set_bit(f, index, state);
            }

            union //TODO: Will not work correctly depending on endian-ness
            {
                word af{};
                struct { byte f, a; };
            };
            union
            {
                word bc{};
                struct { byte c, b; };
            };
            union
            {
                word de{};
                struct { byte e, d; };
            };
            union
            {
                word hl{};
                struct { byte l, h; };
            };

            word sp{};
            word pc{};
        };

        void fetch();
        void execute();



        //https://gist.github.com/meganesu/9e228b6b587decc783aa9be34ae27841
        bool half_carry_occurred_8b_add(const byte& l, const byte& r)
        {
            return (((l & 0x0F) + (r & 0x0F)) & 0x10) == 0x10;
        }
        bool half_carry_occurred_8b_sub(const byte& l, const byte& r)
        {
            return static_cast<int>(l & 0x0F) - static_cast<int>(r & 0x0F) < 0;
        }
        bool half_carry_occurred_16b_add(const word& l, const word& r)
        {
            return (((l & 0x00FF) + (r & 0x00FF)) & 0x0100) == 0x0100;
        }
        bool half_carry_occurred_16b_sub(const word& l, const word& r)
        {
            return static_cast<int>(l & 0x00FF) - static_cast<int>(r & 0x00FF) < 0;
        }
        bool carry_occurred_8b_add(const byte& l, const byte& r)
        {
            return (((l & 0xFF) + (r & 0xFF)) & 0x100) == 0x100;
        }
        bool carry_occurred_8b_sub(const byte& l, const byte& r)
        {
            return static_cast<int>(l & 0xFF) - static_cast<int>(r & 0xFF) < 0;
        }
        bool carry_occurred_16b_add(const word& l, const word& r)
        {
            return (((l & 0xFFFF) + (r & 0xFFFF)) & 0x10000) == 0x10000;
        }
        bool carry_occurred_16b_sub(const word& l, const word& r)
        {
            return static_cast<int>(l & 0xFFFF) - static_cast<int>(r & 0xFFFF) < 0;
        }



        void nop();

        void ld_r16_d16(word& r16);
        void ld_r16_mem_a(word r16);
        void ld_a_r16_mem(word r16);
        void ld_imm16_mem_sp();
        void ld_hli_mem_a();
        void ld_hld_mem_a();
        void ld_a_hli_mem();
        void ld_a_hld_mem();
        void ld_hl_mem_d8();

        void inc_r16(word& r16);
        void dec_r16(word& r16);
        void inc_hl_mem();
        void dec_hl_mem();
        void add_hl_r16(word r16);

        void inc_r8(byte& r8);
        void dec_r8(byte& r8);

        void ld_r8_d8(byte& r8);

        void rlca();
        void rrca();
        void rla();
        void rra();
        void daa();
        void cpl();
        void scf();
        void ccf();

        void jr_imm8();
        void jr_cond_imm8(bool flag);

        void stop();

        void ld_r8_r8(byte& a, byte& b);

        void halt();

        void add_a_r8(byte r8); //TODO: These can be merged with the xxx_a_d8 instructions below?
        void adc_a_r8(byte r8);
        void sub_a_r8(byte r8);
        void sbc_a_r8(byte r8);
        void and_a_r8(byte r8);
        void xor_a_r8(byte r8);
        void or_a_r8(byte r8);
        void cp_a_r8(byte r8);
         
        void add_a_d8();
        void adc_a_d8();
        void sub_a_d8();
        void sbc_a_d8();
        void and_a_d8();
        void xor_a_d8();
        void or_a_d8();
        void cp_a_d8();

        void ret_cond(bool flag);
        void ret();
        void reti();
        void jp_cond_imm16(bool flag);
        void jp_imm16();
        void jp_hl_mem();
        void call_cond_imm16(bool flag);
        void call_imm16();
        void rst_tgt3(byte address);

        void pop_r16stk(word& r16);
        void push_r16stk(word r16);

        void prefix();


        void ldh_c_mem_a();
        void ldh_imm8_mem_a();
        void ld_imm16_mem_a();
        void ldh_a_c_mem();
        void ldh_a_imm8_mem();
        void ld_a_imm16_mem();


        void add_sp_imm8();
        void ld_hl_sp_imm8();
        void ld_sp_hl();

        void di();
        void ei();

        void invalid();

        ////CB Prefix
        //void rlc_r8();
        //void rrc_r8();
        //void rl_r8();
        //void rr_r8();
        //void sla_r8();
        //void sra_r8();
        //void swap_r8();
        //void srl_r8();

        //void bit_b3_r8();
        //void res_b3_r8();
        //void set_b3_r8();





        Registers m_registers{};
        Registers m_lastRegisters{};
        ins8_t    m_opcode{};

        bool m_ime{};
        bool m_imeNext{};
        bool m_hardLocked{}; //For invalid instructions
        bool m_interruptSignaled{};

        std::array<byte, 0x10000>& m_memory;
    };
}
