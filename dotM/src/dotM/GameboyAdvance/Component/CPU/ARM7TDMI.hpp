#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/BUS/SLB.hpp"
#include "dotM/Library/Bitwise.hpp"
#include "dotM/Model/Component/CPU/CPU.hpp"

namespace dot::gba
{
	class ARM7TDMI : public CPU
	{
	public:
		template<typename T>
		struct Register
		{
		public:
			operator T() const
			{
				return m_value;
			}
			operator T&()
			{
				return m_value;
			}
			
			bit operator[](unsigned int index)
			{
				return (m_value >> index) & 0x1;
			}

			void operator=(const T& other)
			{
				m_value = other;
			}
			void operator+(const T& other)
			{
				m_value += other;
			}
			void operator-(const T& other)
			{
				m_value -= other;
			}

		private:
			T m_value;
		};		
		struct Registers
		{
		public:
			Registers() = default;
			~Registers() = default;



			//User
			Register<dword> r0;
			Register<dword> r1;
			Register<dword> r2;
			Register<dword> r3;
			Register<dword> r4;
			Register<dword> r5;
			Register<dword> r6;
			Register<dword> r7;

			Register<dword> r8;
			Register<dword> r9;
			Register<dword> r10;
			Register<dword> r11;
			Register<dword> r12;

			union { Register<dword> r13; Register<dword> sp; };
			union { Register<dword> r14; Register<dword> lr; };
			union { Register<dword> r15; Register<dword> pc; };



			//Fast Interrupt Request
			Register<dword> r8_fiq;
			Register<dword> r9_fiq;
			Register<dword> r10_fiq;
			Register<dword> r11_fiq;
			Register<dword> r12_fiq;
			Register<dword> r13_fiq;
			Register<dword> r14_fiq;

			//Interrupt Request
			Register<dword> r13_irq;
			Register<dword> r14_irq;
			
			//Supervisor
			Register<dword> r13_svc;
			Register<dword> r14_svc;

			//Abort
			Register<dword> r13_abt;
			Register<dword> r14_abt;

			//Undefined
			Register<dword> r13_und;
			Register<dword> r14_und;



			//Status
			Register<dword> cpsr;

			Register<dword> spsr_fiq;
			Register<dword> spsr_irq;
			Register<dword> spsr_svc;
			Register<dword> spsr_abt;
			Register<dword> spsr_und;



			Register<dword>& operator[](unsigned int index)
			{
				if (index > 15) throw std::out_of_range("Only general registers can be indexed");

				return *(reinterpret_cast<Register<dword>*>(&this->r0) + index);
			}
		};
		struct Instruction
		{
		public:
	
			Instruction() = default;
			~Instruction() = default;

			inline bool accumulate() const
			{
				return a;
			}
			inline bool swap() const
			{
				return b;
			}
			inline bool unsigned_halfword() const
			{
				return h;
			}
			inline bool immediate() const
			{
				return i;
			}
			inline bool link() const
			{
				return l;
			}
			inline bool load_store() const
			{
				return l;
			}
			inline bool transfer_length() const
			{
				return n;
			}
			inline bool pre_post_index() const
			{
				return p;
			}
			inline bool status() const
			{
				return s;
			}
			inline bool up_down() const
			{
				return u;
			}
			//rename or create enum to get values (signed is a keyword)
			inline bool sign() const
			{
				return u;
			}
			inline bool writeback() const
			{
				return w;
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



			operator dword() const
			{
				return m_value;
			}
			void operator=(dword value)
			{
				m_value = value;
			}

		private:
			dword m_value{};
		};
		
		ARM7TDMI();
		~ARM7TDMI() = default;

		void cycle();
		void reset();

		std::shared_ptr<BUS> bus;

	protected:
		void fetch();
		void decode();
		void execute();

		void exception();
		void abort();

	private:
		enum class OperatingState
		{
			ARM, 
			THUMB, 
		};
		enum class OperatingMode
		{
			USR = 0b10000, //User
			FIQ = 0b10001, //Fast Interrupt Request
			IRQ = 0b10010, //Interrupt Request
			SVC = 0b10011, //Supervisor
			ABT = 0b10111, //Abort
			UND = 0b11011, //Undefined
			SYS = 0b11111, //System
		};
		enum class AddressingMode
		{
			MD1,
			MD2,
			MD3,
			MD4,
			MD5,
		};
		enum class InstructionFormat
		{
			DataProcessing, 
			Multiply, 
			MultiplyLong, 
			SingleDataSwap, 
			BranchExchange, 
			HalfwordDataTransfer, 
			SingleDataTransfer, 
			Undefined, 
			BlockDataTransfer, 
			Branch, 
			CoprocessorDataTransfer, 
			CoprocessorDataOperation, 
			CoprocessorRegisterTransfer, 
			SoftwareInterrupt, 
		};


		dword shift(dword value, ShiftType shift, byte amount);
		
		template<typename T, size_t bits>
		inline T extend(dword value)
		{
			struct ext { T v : bits; };

			return ext{ value }.v;
		}

		template<typename T>
		inline bool all_set(T value, unsigned int index, bit state) const
		{
			T mask = (1ul << index) - 1;
			value &= mask;

			return value == state;
		}


		
		bool is_condition_passed() const;
		void set_instruction_values();	
		
		
		
	#pragma region Data Processing
		void data_processing();

		void i_and(Register<dword>& rd, dword op1, dword op2); //0000
		void i_eor(Register<dword>& rd, dword op1, dword op2); //0001
		void i_sub(Register<dword>& rd, dword op1, dword op2); //0010
		void i_rsb(Register<dword>& rd, dword op1, dword op2); //0011
		void i_add(Register<dword>& rd, dword op1, dword op2); //0100
		void i_adc(Register<dword>& rd, dword op1, dword op2); //0101
		void i_sbc(Register<dword>& rd, dword op1, dword op2); //0110
		void i_rsc(Register<dword>& rd, dword op1, dword op2); //0111
		void i_tst(Register<dword>& rd, dword op1, dword op2); //1000
		void i_teq(Register<dword>& rd, dword op1, dword op2); //1001
		void i_cmp(Register<dword>& rd, dword op1, dword op2); //1010
		void i_cmn(Register<dword>& rd, dword op1, dword op2); //1011
		void i_orr(Register<dword>& rd, dword op1, dword op2); //1100
		void i_mov(Register<dword>& rd, dword op1, dword op2); //1101
		void i_bic(Register<dword>& rd, dword op1, dword op2); //1110
		void i_mvn(Register<dword>& rd, dword op1, dword op2); //1111
	#pragma endregion
	#pragma region PSR Transfer
		void psr_transfer();
		
		void i_mrs(Register<dword>& rd, dword psr);
		void i_msr(Register<dword>& psr, dword op2, bool flags);
	#pragma endregion
	#pragma region Multiply
		void multiply();
		
		void i_mul(Register<dword>& rd, dword rm, dword rs);
		void i_mla(Register<dword>& rd, dword rn, dword rm, dword rs);
	#pragma endregion
	#pragma region Multiply Long
		void multiply_long();
		
		void i_umull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
		void i_smull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
		void i_umlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
		void i_smlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs);
	#pragma endregion
	#pragma region Single Data Swap
		void single_data_swap();
		
		void i_swp(Register<dword>& rd, Register<dword>& rm, dword address, bool type);
	#pragma endregion
	#pragma region Branch and Exchange
		void branch_exchange();
		
		void i_bx(dword op1);
	#pragma endregion
	#pragma region Halfword Data Transfer
		void halfword_data_transfer();
		
		void i_ldrh(Register<dword>& rd);
		void i_strh(Register<dword>& rd);
		void i_ldrsb(Register<dword>& rd);
		void i_ldrsh(Register<dword>& rd);
	#pragma endregion
	#pragma region Single Data Transfer
		void single_data_transfer();

		void i_ldr(Register<dword>& rd, bool b);
		void i_str(Register<dword>& rd, bool b);
	#pragma endregion
	#pragma region Undefined
		void undefined();
		
		void i_und();
	#pragma endregion
	#pragma region Block Data Transfer
		void block_data_transfer();
		
		void i_ldm(std::vector<std::reference_wrapper<Register<dword>>> registers, dword address);
		void i_stm(std::vector<std::reference_wrapper<Register<dword>>> registers, dword address);
	#pragma endregion
	#pragma region Branch
		void branch();

		void i_b(dword offset);
		void i_bl(dword offset);
	#pragma endregion
	#pragma region Coprocessor Data Transfer
		void coprocessor_data_transfer();
		
		void i_ldc();
		void i_stc();
	#pragma endregion
	#pragma region Coprocessor Data Operation
		void coprocessor_data_operation();
		
		void i_cdp();
	#pragma endregion
	#pragma region Coprocessor Register Transfer
		void coprocessor_register_transfer();
		
		void i_mcr();
		void i_mrc();
	#pragma endregion
	#pragma region Software Interrupt
		void software_interrupt();
		
		void i_swi();
	#pragma endregion



		OperatingState m_operatingState;
		OperatingMode m_operatingMode;
		
		Registers m_registers;
		
		Instruction m_instruction;
		InstructionFormat m_decodedInstruction;
		
		

		bit m_carry{};

		
		
		unsigned int m_cycles{};
	};
}
