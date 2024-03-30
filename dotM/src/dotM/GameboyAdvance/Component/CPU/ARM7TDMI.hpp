#pragma once

#include <stdafx.hpp>

#include "dotM/Architecture/EXCEPT/VECTOR.hpp"
#include "dotM/Architecture/LUT/ARM.hpp"
#include "dotM/Architecture/LUT/THUMB.hpp"
#include "dotM/Architecture/MEMORY/MAP.hpp"
#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/GameboyAdvance/Component/BUS/IConnectable.hpp"
#include "dotM/Model/Component/CPU/CPU.hpp"

namespace dot::gba
{
    class ARM7TDMI : public CPU, public IConnectable<32>
    {
    public:
        using ins32_t = dword;
        using ins16_t =  word;
		
		enum OperatingState
		{
			ARM,
			THUMB,
		};
		enum OperatingMode
		{
			USR = 0b10000,                                                     //User
			FIQ = 0b10001,                                                     //Fast Interrupt Request
			IRQ = 0b10010,                                                     //Interrupt Request
			SVC = 0b10011,                                                     //Supervisor
			ABT = 0b10111,                                                     //Abort
			UND = 0b11011,                                                     //Undefined
			SYS = 0b11111,                                                     //System
		};
		enum class ExceptionType
		{
			Reset, 
			Undefined, 
			SoftwareInterrupt, 
			PrefetchAbort, 
			DataAbort, 
			Reserved, 
			IRQ, 
			FIQ, 
		};
		enum class InterruptType
		{
			IRQ, 
			FIQ, 
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

				IRQ      = 7,												   //I
				FIQ      = 6,												   //F
				Thumb    = 5,												   //T
			};

			PSR() = default;
			~PSR() = default;

			bool flag(Flag flag) const
			{
				return get_bit(m_value, static_cast<unsigned int>(flag));
			}
			void set(Flag flag, bool state)
			{
				m_value = set_bit(m_value, static_cast<unsigned int>(flag), state);
			}

			OperatingMode mode() const
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
			Registers()
			{
				reset();
			}
			~Registers() {}

			Register<dword>& rx(unsigned int index)
			{
				return m_rx[index];
			}
			PSR& cpsr()
			{
				return m_cpsr;
			}
			PSR& spsr(OperatingMode operatingMode)
			{
				switch (operatingMode)
				{
					case OperatingMode::FIQ: return m_spsrFIQ;
					case OperatingMode::IRQ: return m_spsrIRQ;
					case OperatingMode::SVC: return m_spsrSVC;
					case OperatingMode::ABT: return m_spsrABT;
					case OperatingMode::UND: return m_spsrUND;

					default:                 throw std::invalid_argument("Invalid operating mode!");
				}
			}

			void bank(OperatingMode operatingMode)
			{
				const auto& bank_rxp = [this](unsigned int iterations, unsigned int xOffset, unsigned int pOffset)
				{
					for (unsigned int i = 0; i < iterations; ++i) 
						m_rxp[i + pOffset] = m_rx.data() + i + xOffset;
				};



				bank_rxp(16, 0, 0);                                            //Bank to USR mode => Some registers may need to be remapped to their default

				switch (operatingMode)
				{
					case USR:                      break;                      //This operation would be redundant
					case FIQ: bank_rxp(7, 16,  8); break;
					case IRQ: bank_rxp(2, 23, 13); break;
					case SVC: bank_rxp(2, 25, 13); break;
					case ABT: bank_rxp(2, 27, 13); break;
					case UND: bank_rxp(2, 29, 13); break;
					case SYS:                      break;                      //SYS mode registers do not exist => same operation as USR mode

					default:                       throw std::invalid_argument("Invalid operating mode!");
				}
			}
			void reset()
			{
				m_rSVC[1] = m_rx[15];                                          //The PC is stored in a SVC register
				m_rx[15]  = 0x0;                                               //The PC is set to the reset vector TODO

				m_spsrSVC = m_cpsr;                                            //The current CPSR is copied
				m_cpsr    = set_bits<dword>(m_cpsr, 0, 5, 0xD3);               //SVC mode, FIQ + IRQ set, T cleared

				bank(m_cpsr.mode());                                           //Bank to the new mode
			}

			Register<dword>& operator[](unsigned int index)
			{
				return *(m_rxp[index & 0xF]);
			}

		private:
			std::array<Register<dword>*, 16> m_rxp{};
			union
			{
				std::array<Register<dword>, 31> m_rx{};
				struct
				{
					std::array<Register<dword>, 16> m_rUSR;
					std::array<Register<dword>,  7> m_rFIQ;
					std::array<Register<dword>,  2> m_rIRQ, m_rSVC, m_rABT, m_rUND;
				};
			};
			union
			{
				PSR m_srx[6]{};
				struct{ PSR m_cpsr, m_spsrFIQ, m_spsrIRQ, m_spsrSVC, m_spsrABT, m_spsrUND; };
			};
		};



		struct State
		{
		public:
			State(const Pipeline<ins32_t, dword, 3>& pipeline, const Registers& registers, unsigned int cycles)
				: pipeline{ pipeline }, registers{ registers }, cycles{ cycles } {}
			~State() = default;

			const Pipeline<ins32_t, dword, 3>& pipeline;
			const Registers& registers;
			const unsigned int& cycles;
		};
		
		ARM7TDMI();
		~ARM7TDMI() = default;

        void cycle();
        void reset();
		
		void signal_irq();
		void signal_fiq();

		State state();

    protected:
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

		void fetch();
		void decode();
		void execute();

		void handle_interrupt();
		void exception();
		void abort();
		void switch_mode(OperatingMode operatingMode);
        
        bool check_condition(ins32_t instruction, const PSR& cpsr);
        std::tuple<dword, bool>shift_carry(dword value, ShiftType shift, dword amount, bool carryFlag = false);

#pragma region ARM INSTRUCTIONS
        void data_processing                        (ins32_t instruction);
        void psr_transfer                           (ins32_t instruction);
        void multiply                               (ins32_t instruction);
        void multiply_long                          (ins32_t instruction);
        void single_data_swap                       (ins32_t instruction);
        void branch_exchange                        (ins32_t instruction);
        void halfword_data_transfer                 (ins32_t instruction);
        void single_data_transfer                   (ins32_t instruction);
        void undefined                              (ins32_t instruction);
        void block_data_transfer                    (ins32_t instruction);
        void branch                                 (ins32_t instruction);
        void coprocessor_data_transfer              (ins32_t instruction);
        void coprocessor_data_operation             (ins32_t instruction);
        void coprocessor_register_transfer          (ins32_t instruction);
        void software_interrupt                     (ins32_t instruction);
#pragma endregion
#pragma region THUMB INSTRUCTIONS
        void move_shifted_register                  (ins16_t instruction);
        void add_subtract                           (ins16_t instruction);
        void move_compare_add_subtract_immediate    (ins16_t instruction);
        void alu_operations                         (ins16_t instruction);
        void hi_register_operations__branch_exchange(ins16_t instruction);
        void pc_relative_load                       (ins16_t instruction);
        void load_store_register_offset             (ins16_t instruction);
        void load_store_sign_extended               (ins16_t instruction);
        void load_store_immediate_offet             (ins16_t instruction);
        void load_store_halfword                    (ins16_t instruction);
        void sp_relative_load_store                 (ins16_t instruction);
        void load_address                           (ins16_t instruction);
        void add_offset_stack_pointer               (ins16_t instruction);
        void push_pop_registers                     (ins16_t instruction);
        void multiple_load_store                    (ins16_t instruction);
        void conditional_branch                     (ins16_t instruction);
        void software_interrupt                     (ins16_t instruction);
        void unconditional_branch                   (ins16_t instruction);
        void long_branch_link                       (ins16_t instruction);
#pragma endregion

        Pipeline<ins32_t, dword, 3> m_pipeline{};
        
        Registers m_registers{};
        OperationARM m_operationARM{};                                         
        OperationTHUMB m_operationTHUMB{};

        unsigned int m_waitCycles{};
		bool m_irqRequest{};
		bool m_fiqRequest{};
		bool m_halted{};
    };
}
