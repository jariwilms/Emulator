#pragma once

#include <stdafx.hpp>

#include "dotM/Architecture/ARM/ARM.hpp"
#include "dotM/Architecture/THUMB/THUMB.hpp"
#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/Model/Component/CPU/CPU.hpp"

namespace dot::gba
{
    class ARM7TDMI : public CPU
    {
    public:
        ARM7TDMI()  = default;
        ~ARM7TDMI() = default;

        void cycle();
        void reset();

        void connect(std::shared_ptr<BUS<32>> bus);

    protected:
        enum class OperatingState
        {
            ARM, 
            THUMB, 
        };
        enum class OperatingMode
        {
            USR = 0b10000,                                                     //User
            FIQ = 0b10001,                                                     //Fast Interrupt Request
            IRQ = 0b10010,                                                     //Interrupt Request
            SVC = 0b10011,                                                     //Supervisor
            ABT = 0b10111,                                                     //Abort
            UND = 0b11011,                                                     //Undefined
            SYS = 0b11111,                                                     //System
        };
        enum class OperationARM
        {
            DataProcessing, 
            Multiply, 
            MultiplyLong, 
            SingleDataSwap, 
            BranchExchange, 
            HalfwordDataTransferRegisterOffset, 
            HalfwordDataTransferImmediateOffset, 
            SingleDataTransfer, 
            Undefined, 
            BlockDataTransfer, 
            Branch, 
            CoprocessorDataTransfer, 
            CoprocessorDataOperation, 
            CoprocessorRegisterTransfer, 
            SoftwareInterrupt, 
        };
        enum class OperationTHUMB
        {
            MoveShiftedRegister, 
            AddSubtract, 
            MoveCompareAddSubtractImmediate, 
            ALUOperations, 
            HiRegisterOperationsBranchExchange, 
            LoadPCRelative, 
            LoadStoreRegisterOffset, 
            LoadStoreSignExtended, 
            LoadStoreImmediateOffset, 
            LoadStoreHalfword, 
            LoadStoreSPRelative, 
            LoadAddress, 
            AddOffsetToStackPointer, 
            PushPopRegisters, 
            MultipleLoadStore, 
            ConditionalBranch, 
            SoftwareInterrupt, 
            UnconditionalBranch, 
            LongBranchWithLink, 
        };
        
        struct PSR : public Register<dword>
        {
        public:
            enum Flag
            {
                Negative = 31,                                                 //N
                Zero     = 30,												   //Z
                Carry    = 29,												   //C
                Overflow = 28,												   //V
                                                                               
                IRQ      =  7,												   //I
                FIQ      =  6,												   //F
                Thumb    =  5,												   //T
            };																   

            PSR() = default;
            virtual ~PSR() = default;

            

            bool flag(Flag flag) const
            {
                return get_bit(m_value, static_cast<unsigned int>(flag));
            }
            void set_flag(Flag flag, bool state)
            {
                m_value = set_bit(m_value, static_cast<unsigned int>(flag), state);
            }

            OperatingMode mode()
            {
                return static_cast<OperatingMode>(m_value & 0x1F);
            }



            PSR& operator=(const PSR& other)                                   //TODO: fix PSR and base register operators => not clean/cohesive
            {
                m_value = other.m_value;

                return *this;
            }
            PSR& operator=(const dword& value)
            {
                m_value = value;

                return *this;
            }
        };
        struct Registers
        {
        public:
            Registers() : r13 { m_rx[13] }, r14{ m_rx[14] }, r15{ m_rx[15] }
            {
                reset();
            }
            ~Registers() = default;

            void bank(OperatingMode operatingMode)
            {
                switch (operatingMode)
                {
                    case OperatingMode::USR:
                    {
                        for (unsigned int i = 0; i < 7; ++i)
                        {
                            *(&r8 + i) = *(m_rx.data() + 8 + i);
                        }

                        break;
                    }
                    case OperatingMode::FIQ:
                    {
                        for (unsigned int i = 0; i < 7; ++i)
                        {
                            *(&r8 + i) = *(&m_r8FIQ + i);
                        }

                        break;
                    }
                    case OperatingMode::IRQ:
                    {
                        for (unsigned int i = 0; i < 2; ++i)
                        {
                            *(&r13 + i) = *(&m_r13IRQ + i);
                        }

                        break;
                    }
                    case OperatingMode::SVC:
                    {
                        for (unsigned int i = 0; i < 2; ++i)
                        {
                            *(&r13 + i) = *(&m_r13SVC + i);
                        }

                        break;
                    }
                    case OperatingMode::ABT:
                    {
                        for (unsigned int i = 0; i < 2; ++i)
                        {
                            *(&r13 + i) = *(&m_r13ABT + i);
                        }

                        break;
                    }
                    case OperatingMode::UND:
                    {
                        for (unsigned int i = 0; i < 2; ++i)
                        {
                            *(&r13 + i) = *(&m_r13UND + i);
                        }

                        break;
                    }
                    case OperatingMode::SYS:
                    {
                        for (unsigned int i = 0; i < 7; ++i)
                        {
                            *(&r8 + i) = *(m_rx.data() + 8 + i);
                        }

                        break;
                    }

                    default: throw std::invalid_argument("Invalid operating mode");
                }
            }
            void reset()
            {
                const unsigned int registerCount = 16;

                for (unsigned int i = 0; i < registerCount; ++i)
                    *(&r0 + i) = m_rx[i];

                m_r14SVC = pc;
                m_spsrSVC = m_cpsr;                                           

                m_cpsr = set_bits<dword>(m_cpsr, 0, 5, 0xD3);                  //SVC mode, IRQ + FIQ set, T cleared

                pc = 0x0;			                                           //TODO: reset vector
            }


            
            PSR& get_cpsr()
            {
                return m_cpsr;
            }
            PSR& get_spsr(OperatingMode operatingMode)
            {
                switch (operatingMode)
                {
                    case dot::gba::ARM7TDMI::OperatingMode::FIQ: return m_spsrFIQ;
                    case dot::gba::ARM7TDMI::OperatingMode::IRQ: return m_spsrIRQ;
                    case dot::gba::ARM7TDMI::OperatingMode::SVC: return m_spsrSVC;
                    case dot::gba::ARM7TDMI::OperatingMode::ABT: return m_spsrABT;
                    case dot::gba::ARM7TDMI::OperatingMode::UND: return m_spsrUND;

                    default: throw std::invalid_argument("Invalid operating mode");
                }
            }
            


            //User
            RegisterReference<dword> r0 { m_rx[ 0] };
            RegisterReference<dword> r1 { m_rx[ 1] };
            RegisterReference<dword> r2 { m_rx[ 2] };
            RegisterReference<dword> r3 { m_rx[ 3] };
            RegisterReference<dword> r4 { m_rx[ 4] };
            RegisterReference<dword> r5 { m_rx[ 5] };
            RegisterReference<dword> r6 { m_rx[ 6] };
            RegisterReference<dword> r7 { m_rx[ 7] };

            RegisterReference<dword> r8 { m_rx[ 8] };
            RegisterReference<dword> r9 { m_rx[ 9] };
            RegisterReference<dword> r10{ m_rx[10] };
            RegisterReference<dword> r11{ m_rx[11] };
            RegisterReference<dword> r12{ m_rx[12] };

            union { RegisterReference<dword> r13; RegisterReference<dword> sr; }; //TODO: cowbite => init to default value?
            union { RegisterReference<dword> r14; RegisterReference<dword> lr; };
            union { RegisterReference<dword> r15; RegisterReference<dword> pc; };



            Register<dword>& operator[](unsigned int index)
            {
                if (index > 15) throw std::out_of_range("Only general registers can be indexed!");

                return *(&r0 + index);
            }

        private:
            std::array<Register<dword>, 16> m_rx{};

            //FIQ
            Register<dword> m_r8FIQ{};
            Register<dword> m_r9FIQ{};
            Register<dword> m_r10FIQ{};
            Register<dword> m_r11FIQ{};
            Register<dword> m_r12FIQ{};
            Register<dword> m_r13FIQ{};
            Register<dword> m_r14FIQ{};

            //IRQ
            Register<dword> m_r13IRQ{};
            Register<dword> m_r14IRQ{};

            //SVC
            Register<dword> m_r13SVC{};
            Register<dword> m_r14SVC{};

            //ABT
            Register<dword> m_r13ABT{};
            Register<dword> m_r14ABT{};

            //UND
            Register<dword> m_r13UND{};
            Register<dword> m_r14UND{};



            //Status
            PSR m_cpsr{};

            PSR m_spsrFIQ{};
            PSR m_spsrIRQ{};
            PSR m_spsrSVC{};
            PSR m_spsrABT{};
            PSR m_spsrUND{};
        };
        template<typename T>
        struct Instruction
        {
        public:
            Instruction() = default;
            Instruction(const T& value) 
                : m_value{ value } {}
            ~Instruction() = default;

            inline void clear()
            {
                std::memset(this, 0, sizeof(Instruction));
            }
            
            
            dword address{}; //TODO: remove later

            operator T() const
            {
                return m_value;
            }
            operator T& ()
            {
                return m_value;
            }
            void operator=(const T& value)
            {
                m_value = value;
            }
            bit operator[](unsigned int index)
            {
                return (m_value >> index) & 0x1;
            }

        private:
            T m_value{};
        };


		
		void fetch();
		void decode();
		void execute();

		void switch_mode(OperatingMode operatingMode);

		void exception();
		void abort();
        
        bool check_condition(const Instruction<dword>& instruction, const PSR& cpsr);
        std::pair<dword, bit>shift_carry(dword value, ShiftType shift, dword amount, bit carryFlag = false);
        

        
        //ARM Instructions
        void data_processing                        (const Instruction<dword>& instruction);
        void psr_transfer                           (const Instruction<dword>& instruction);
        void multiply                               (const Instruction<dword>& instruction);
        void multiply_long                          (const Instruction<dword>& instruction);
        void single_data_swap                       (const Instruction<dword>& instruction);
        void branch_exchange                        (const Instruction<dword>& instruction);
        void halfword_data_transfer                 (const Instruction<dword>& instruction);
        void single_data_transfer                   (const Instruction<dword>& instruction);
        void undefined                              (const Instruction<dword>& instruction);
        void block_data_transfer                    (const Instruction<dword>& instruction);
        void branch                                 (const Instruction<dword>& instruction);
        void coprocessor_data_transfer              (const Instruction<dword>& instruction);
        void coprocessor_data_operation             (const Instruction<dword>& instruction);
        void coprocessor_register_transfer          (const Instruction<dword>& instruction);
        void software_interrupt                     (const Instruction<dword>& instruction);



        //THUMB Instructions
        void move_shifted_register                  (const Instruction<word>& instruction);
        void add_subtract                           (const Instruction<word>& instruction);
        void move_compare_add_subtract_immediate    (const Instruction<word>& instruction);
        void alu_operations                         (const Instruction<word>& instruction);
        void hi_register_operations__branch_exchange(const Instruction<word>& instruction);
        void pc_relative_load                       (const Instruction<word>& instruction);
        void load_store_register_offset             (const Instruction<word>& instruction);
        void load_store_sign_extended               (const Instruction<word>& instruction);
        void load_store_immediate_offet             (const Instruction<word>& instruction);
        void load_store_halfword                    (const Instruction<word>& instruction);
        void sp_relative_load_store                 (const Instruction<word>& instruction);
        void load_address                           (const Instruction<word>& instruction);
        void add_offset_stack_pointer               (const Instruction<word>& instruction);
        void push_pop_registers                     (const Instruction<word>& instruction);
        void multiple_load_store                    (const Instruction<word>& instruction);
        void conditional_branch                     (const Instruction<word>& instruction);
        void software_interrupt                     (const Instruction<word>& instruction);
        void unconditional_branch                   (const Instruction<word>& instruction);
        void long_branch_link                       (const Instruction<word>& instruction);
        


        Pipeline<Instruction<dword>, 3> m_pipelineARM{};
        Pipeline<Instruction<word>, 3> m_pipelineTHUMB{};
        
        Registers m_registers{};                                               
        OperationARM m_operationARM{};                                         
        OperationTHUMB m_operationTHUMB{};

        std::shared_ptr<BUS<32>> m_BUS;

        unsigned int m_cycles{};
    };
}
