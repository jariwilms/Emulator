#pragma once

#include <stdafx.hpp>

#include "dotM/Architecture/ARM/ARM.hpp"
#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"
#include "dotM/Library/Bitwise.hpp"
#include "dotM/Model/Component/CPU/CPU.hpp"

namespace dot::gba
{
	class ARM7TDMI : public CPU
	{
	public:
		ARM7TDMI();
		~ARM7TDMI() = default;

		void cycle();
		void reset();

		std::shared_ptr<BUS<32>> bus; //TODO: private

	//protected:
		void fetch();
		void decode();
		void execute();

		void exception();
		void abort();

	//private:
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
		enum class Operation
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

		struct PSR : public Register<dword> //TODO: remove this and use methods in registers idk
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
			Registers() : r12{ m_rx[12] }, r13 { m_rx[13] }, r14{ m_rx[14] }, r15{ m_rx[15] }
			{
				reset();
			}
			~Registers() = default;

			void map(dot::gba::ARM7TDMI::OperatingMode mode)
			{
				switch (m_cpsr.mode())
				{
					case dot::gba::ARM7TDMI::OperatingMode::USR:
					{
						for (unsigned int i = 0; i < 7; ++i)
						{
							*(&r8 + i) = *(m_rx.data() + 8 + i);
						}

						break;
					}
					case dot::gba::ARM7TDMI::OperatingMode::FIQ:
					{
						for (unsigned int i = 0; i < 7; ++i)
						{
							*(&r8 + i) = *(&m_r8FIQ + i);
						}

						break;
					}
					case dot::gba::ARM7TDMI::OperatingMode::IRQ:
					{
						for (unsigned int i = 0; i < 2; ++i)
						{
							*(&r13 + i) = *(&m_r13IRQ + i);
						}

						break;
					}
					case dot::gba::ARM7TDMI::OperatingMode::SVC:
					{
						for (unsigned int i = 0; i < 2; ++i)
						{
							*(&r13 + i) = *(&m_r13SVC + i);
						}

						break;
					}
					case dot::gba::ARM7TDMI::OperatingMode::ABT:
					{
						for (unsigned int i = 0; i < 2; ++i)
						{
							*(&r13 + i) = *(&m_r13ABT + i);
						}

						break;
					}
					case dot::gba::ARM7TDMI::OperatingMode::UND:
					{
						for (unsigned int i = 0; i < 2; ++i)
						{
							*(&r13 + i) = *(&m_r13UND + i);
						}

						break;
					}
					case dot::gba::ARM7TDMI::OperatingMode::SYS:
					{
						map(dot::gba::ARM7TDMI::OperatingMode::USR);

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

				m_cpsr &= 0xFFFFFFE0;                                          //Clear M[4:0]
				m_cpsr |= 0x00000013;                                          //set   M[4:0] to b10011, SVC
				m_cpsr |= 0x000000C0;                                          //Set   I and F bit
				m_cpsr &= 0xFFFFFFDF;                                          //Clear T bit

				pc = 0x0;			                                           
			}


			
			PSR& get_cpsr()
			{
				return m_cpsr;
			}
			PSR& get_spsr()
			{
				switch (m_cpsr.mode())
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
			RegisterReference<dword> r0{ m_rx[0] };
			RegisterReference<dword> r1{ m_rx[1] };
			RegisterReference<dword> r2{ m_rx[2] };
			RegisterReference<dword> r3{ m_rx[3] };
			RegisterReference<dword> r4{ m_rx[4] };
			RegisterReference<dword> r5{ m_rx[5] };
			RegisterReference<dword> r6{ m_rx[6] };
			RegisterReference<dword> r7{ m_rx[7] };

			RegisterReference<dword> r8{ m_rx[8] };
			RegisterReference<dword> r9{ m_rx[9] };
			RegisterReference<dword> r10{ m_rx[10] };
			RegisterReference<dword> r11{ m_rx[11] };

			union { RegisterReference<dword> r12; RegisterReference<dword> ip; };
			union { RegisterReference<dword> r13; RegisterReference<dword> sp; };
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
		struct Instruction
		{
		public:
			enum class Flag
			{
				Accumulate,
				ByteWord,
				Immediate,
				Link,
				LoadStore,
				PrePost,
				PSRForce,
				Set,
				TransferLength,
				Unsigned,
				UpDown,
				Writeback,
			};

			Instruction() = default;
			Instruction(dword value) : m_value(value) {}
			~Instruction() = default;

			bool flag(Flag flag) const
			{
				switch (flag)
				{
					case Flag::Accumulate:	   return a;
					case Flag::ByteWord:	   return b;
					case Flag::Immediate:      return i;
					case Flag::Link:           return l;
					case Flag::LoadStore:      return l;
					case Flag::PrePost:		   return p;
					case Flag::PSRForce:       return s;
					case Flag::Set:			   return s;
					case Flag::TransferLength: return n;
					case Flag::Unsigned:	   return u;
					case Flag::UpDown:		   return u;
					case Flag::Writeback:	   return w;

					default:                   throw std::runtime_error("Invalid instruction flag");
				}
			}

			inline void clear()
			{
				std::memset(this, 0, sizeof(Instruction));
			}



			//Registers
			dword rd{};
			dword rn{};
			dword rm{};
			dword rs{};

			dword rdhi{};
			dword rdlo{};

			dword crd{};
			dword crn{};
			dword crm{};



			//Condition, Opcode, and Operand2
			byte cond : 4{};
			byte opcode : 4{};
			word operand2 : 12{};



			//Coprocessor Info and Opcode
			byte cp : 4{};
			byte cpn : 4{};
			byte cpOpc : 4{};



			//Registerlist, generic offset
			word registerlist : 16{};
			dword offset : 28{};



			//Flag bits
			union
			{
				bit a : 1{};
				bit w : 1;
			};
			bit b : 1{};
			bit h : 1{};
			bit i : 1{};
			bit l : 1{};
			bit n : 1{};
			bit p : 1{};
			bit s : 1{};
			bit u : 1{};



			dword address{};
			
			

			operator dword() const
			{
				return m_value;
			}
			void operator=(dword value)
			{
				m_value = value;
			}
			bit operator[](unsigned int index)
			{
				return (m_value >> index) & 0x1;
			}

		private:
			dword m_value{};
		};


		
		std::pair<dword, bit>shift_carry(dword value, ShiftType shift, dword amount, bit carryFlag = false);
		

		
//ARM Instructions
#pragma region Data Processing
		void data_processing(const Instruction& instruction);

		void i_and(Register<dword>& rd, dword rn, dword op2);
		void i_eor(Register<dword>& rd, dword rn, dword op2);
		void i_sub(Register<dword>& rd, dword rn, dword op2);
		void i_rsb(Register<dword>& rd, dword rn, dword op2);
		void i_add(Register<dword>& rd, dword rn, dword op2);
		void i_adc(Register<dword>& rd, dword rn, dword op2);
		void i_sbc(Register<dword>& rd, dword rn, dword op2);
		void i_rsc(Register<dword>& rd, dword rn, dword op2);
		void i_tst(Register<dword>& rd, dword rn, dword op2);
		void i_teq(Register<dword>& rd, dword rn, dword op2);
		void i_cmp(Register<dword>& rd, dword rn, dword op2);
		void i_cmn(Register<dword>& rd, dword rn, dword op2);
		void i_orr(Register<dword>& rd, dword rn, dword op2);
		void i_mov(Register<dword>& rd, dword rn, dword op2);
		void i_bic(Register<dword>& rd, dword rn, dword op2);
		void i_mvn(Register<dword>& rd, dword rn, dword op2);
#pragma endregion
#pragma region PSR Transfer
		void psr_transfer(const Instruction& instruction);

		void i_mrs(Register<dword>& rd, const PSR& psr);
		void i_msr(PSR& psr, dword op2, dword mask);
#pragma endregion
#pragma region Multiply
		void multiply(const Instruction& instruction);

		void i_mul(Register<dword>& rd, dword rm, dword rs);
		void i_mla(Register<dword>& rd, dword rn, dword rm, dword rs);
#pragma endregion
#pragma region Multiply Long
		void multiply_long(const Instruction& instruction);

		void i_umull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
		void i_smull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
		void i_umlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
		void i_smlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
#pragma endregion
#pragma region Single Data Swap
		void single_data_swap(const Instruction& instruction);

		void i_swp(Register<dword>& rd, Register<dword>& rm, size_t address, bool isByte);
#pragma endregion
#pragma region Branch and Exchange
		void branch_exchange(const Instruction& instruction);

		void i_bx(dword rn);
#pragma endregion
#pragma region Halfword Data Transfer
		void halfword_data_transfer(const Instruction& instruction);

		void i_ldrh(Register<dword>& rd, size_t address);
		void i_strh(Register<dword>& rd, size_t address);
		void i_ldrsb(Register<dword>& rd, size_t address);
		void i_ldrsh(Register<dword>& rd, size_t address);
#pragma endregion
#pragma region Single Data Transfer
		void single_data_transfer(const Instruction& instruction);

		void i_ldr(Register<dword>& rd, size_t address, bool isByte);
		void i_str(Register<dword>& rd, size_t address, bool isByte);
#pragma endregion
#pragma region Undefined
		void undefined(const Instruction& instruction);

		void i_und();
#pragma endregion
#pragma region Block Data Transfer
		void block_data_transfer(const Instruction& instruction);

		void i_ldm(std::vector<std::reference_wrapper<Register<dword>>> registers, size_t address);
		void i_stm(std::vector<std::reference_wrapper<Register<dword>>> registers, size_t address);
#pragma endregion
#pragma region Branch
		void branch(const Instruction& instruction);

		void i_b(dword offset);
		void i_bl(dword offset);
#pragma endregion
#pragma region Coprocessor Data Transfer
		void coprocessor_data_transfer(const Instruction& instruction);

		void i_ldc();
		void i_stc();
#pragma endregion
#pragma region Coprocessor Data Operation
		void coprocessor_data_operation(const Instruction& instruction);

		void i_cdp();
#pragma endregion
#pragma region Coprocessor Register Transfer
		void coprocessor_register_transfer(const Instruction& instruction);

		void i_mcr();
		void i_mrc();
#pragma endregion
#pragma region Software Interrupt
		void software_interrupt(const Instruction& instruction);

		void i_swi();
#pragma endregion

//THUMB Instructions, these will get better names...
#pragma region Move Shifted Register
		void move_shifted_register();

		void i_lsl();
		void i_lsr();
		void i_asr();
#pragma endregion
#pragma region Add/Subtract
		void add_subtract();
#pragma endregion
#pragma region Move/Compare/Add/Subtract Immediate
		void move_compare_add_subtract_immediate();
#pragma endregion
#pragma region ALU Operations
		void alu_operations();
#pragma endregion
#pragma region High Register Operations / Branch Exchange
		void hi_register_operations__branch_exchange();
#pragma endregion
#pragma region PC-relative Load
		void pc_relative_load();
#pragma endregion
#pragma region Load/Store Register Offset
		void load_store_register_offset();
#pragma endregion
#pragma region Load/Store Sign Extended
		void load_store_sign_extended();
#pragma endregion
#pragma region Load/Store Immediate Offset
		void load_store_immediate_offet();
#pragma endregion
#pragma region Load/Store Halfword
		void load_store_halfword();
#pragma endregion
#pragma region SP-relative Load/Store
		void sp_relative_load_store();
#pragma endregion
#pragma region Load Address
		void load_address();
#pragma endregion
#pragma region Add Offset Stack Pointer
		void add_offset_stack_pointer();
#pragma endregion
#pragma region Push/Pop Registers
		void push_pop_registers();
#pragma endregion
#pragma region Multiple Load/Store
		void multiple_load_store();
#pragma endregion
#pragma region Conditional Branch
		void conditional_branch();
#pragma endregion
#pragma region Software Interrupt
		void software_interrupt();
#pragma endregion
#pragma region Unconditional Branch
		void unconditional_branch();
#pragma endregion
#pragma region Long Branch Link
		void long_branch_link();
#pragma endregion
		



		Pipeline<Instruction, 3> m_pipeline{};                                 //Construct a pipeline with 3 stages
		Registers m_registers{};                                               //The registry has 37 registers
		Operation m_operation{};                                               //The operation to be done on the current instruction value

		unsigned int m_cycles{};                                               //The number of cycles the CPU will idle for
	};
}
