#pragma once

#include <stdafx.hpp>

namespace dot::gb
{
    class SharpSM83
    {
    public:
        SharpSM83(std::array<byte, 0xFFFF>& memory);
        ~SharpSM83() = default;

        void cycle();
        void reset();

    private:
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
                af = 0;
                bc = 0;
                de = 0;
                hl = 0;
                sp = 0;
                pc = 0;
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

            union
            {
                word af{};
                struct { byte a, f; };
            };
            union
            {
                word bc{};
                struct { byte b, c; };
            };
            union
            {
                word de{};
                struct { byte d, e; };
            };
            union
            {
                word hl{};
                struct { byte h, l; };
            };
            word sp{};
            word pc{};
        };

        void fetch();
        void execute();

        Registers m_registers{};
        ins8_t    m_opcode{};

        bool m_ime{};
        bool m_imeNext{};

        std::array<byte, 0xFFFF>& m_memory;
    };
}
