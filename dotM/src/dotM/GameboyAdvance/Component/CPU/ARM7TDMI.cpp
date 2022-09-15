#include <stdafx.hpp>

#include "ARM7TDMI.hpp"

namespace dot::gba
{
	ARM7TDMI::ARM7TDMI()
	{

	}

	void ARM7TDMI::cycle()
	{
		fetch();
		decode();
		execute();
	}
	void ARM7TDMI::reset()
	{
		//m_registers.r14_svc = m_registers.pc;    //Overwrite SVC R14 with PC
		//m_registers.spsr_svc = m_registers.cpsr; //Overwrite SVC SPSR with CPSR

		////Is this even correct? why clear twice? TODO: check
		//m_registers.cpsr &= 0xFFFFFFE0;          //Clear M[4:0]
		//m_registers.cpsr |= 0x00000013;          //Set M[4:0] to Supervisor Mode

		//m_registers.cpsr |= 0x000000C0;          //Set I and F bit;
		//m_registers.cpsr &= 0xFFFFFFDF;          //Clear T bit;

		//m_registers.pc = 0x00000000;			 //Set PC to Reset Vector

		//m_cycles += 2;
	}

	void ARM7TDMI::fetch()
	{
		//auto addr = m_registers.pc & 0xFFFFFFFC; //Align to 4 byte boundary

		//bus->set(addr);
		//m_instruction = bus->read<dword>();

		//if (m_operatingState == OperatingState::ARM) m_registers.pc += 4;
		//else m_registers.pc += 2;
	}
	void ARM7TDMI::decode()
	{
		const std::array<dword, 15> instructionFormat
		{
			0x0000'0000, //Data Processing
			0x0000'0090, //Multiply
			0x0080'0090, //Multiply Long
			0x0100'0090, //Single Data Swap
			0x012F'FF10, //Branch and Exchange
			0x0000'0090, //Halfword Data Transfer: register offset
			0x0040'0090, //Halfword Data Transfer: immediate offset
			0x0400'0000, //Single Data Transfer
			0x0600'0010, //Undefined
			0x0800'0000, //Block Data Transfer
			0x0A00'0000, //Branch
			0x0C00'0000, //Coprocessor Data Transfer
			0x0E00'0000, //Coprocessor Data Operation
			0x0E00'0010, //Coprocessor RegisterTransfer
			0x0F00'0000, //Software Interrupt
		};

		const std::array<dword, 15> instructionMask
		{
			0x0C00'0000, //Data Processing
			0x0FC0'00F0, //Multiply
			0x0F80'0090, //Multiply Long
			0x0F80'0FF0, //Single Data Swap
			0x0FFF'FFF0, //Branch and Exchange
			0x0E40'0F90, //Halfword Data Transfer: register offset
			0x0E40'0090, //Halfword Data Transfer: immediate offset
			0x0C00'0000, //Single Data Transfer
			0x0E00'0010, //Undefined
			0x0E00'0000, //Block Data Transfer
			0x0F00'0000, //Branch
			0x0E00'0000, //Coprocessor Data Transfer
			0x0F00'0010, //Coprocessor Data Operation
			0x0F00'0010, //Coprocessor RegisterTransfer
			0x0F00'0000, //Software Interrupt
		};

		
		
		for (auto i = instructionFormat.size(); i > 0; --i)
		{
			if ((m_instruction & instructionMask[i]) != instructionFormat[i]) continue;

			m_decodedInstruction = static_cast<InstructionFormat>(i);
			set_instruction_values();
			
			return;
		}

		throw std::runtime_error("Invalid instruction format");
	}
	void ARM7TDMI::execute()
	{
		switch (m_decodedInstruction)
		{
			case InstructionFormat::DataProcessing:              data_processing();                         break;
			case InstructionFormat::Multiply:                    multiply();                                break;
			case InstructionFormat::MultiplyLong:                multiply_long();                           break;
			case InstructionFormat::SingleDataSwap:              single_data_swap();                        break;
			case InstructionFormat::BranchExchange:              branch_exchange();                         break;
			case InstructionFormat::HalfwordDataTransfer:        halfword_data_transfer();  break;
			case InstructionFormat::SingleDataTransfer:          single_data_transfer();                    break;
			case InstructionFormat::Undefined:                   undefined();                               break;
			case InstructionFormat::BlockDataTransfer:           block_data_transfer();                     break;
			case InstructionFormat::Branch:                      branch();                                  break;
			case InstructionFormat::CoprocessorDataTransfer:     coprocessor_data_transfer();               break;
			case InstructionFormat::CoprocessorDataOperation:    coprocessor_data_operation();              break;
			case InstructionFormat::CoprocessorRegisterTransfer: coprocessor_register_transfer();           break;
			case InstructionFormat::SoftwareInterrupt:           software_interrupt();                      break;

			default:                                             throw std::runtime_error("Invalid instruction");
		}
	}

	void ARM7TDMI::exception()
	{
		//p.62;
	}
	void ARM7TDMI::abort()
	{

	}

	dword ARM7TDMI::shift(dword value, ShiftType type, byte amount)
	{
		using shiftFunc = std::function<dword()>;
		const size_t size = sizeof(dword) * 8;



		const auto lsl_eq0 = [&]() -> dword
		{
			m_carry = get_bit(m_registers.cpsr, 29);
			return value;
		};
		const auto lsl_lt32 = [&]() -> dword
		{
			auto carryout = shift(value, ShiftType::LSR, static_cast<byte>(size) - amount);
			m_carry = get_bit(carryout, 0);

			return value << amount;
		};
		const auto lsl_eq32 = [&]() -> dword
		{
			m_carry = get_bit(value, 0);
			return 0;
		};
		const auto lsl_gt32 = [&]() -> dword
		{
			m_carry = 0;
			return 0;
		};

		const auto lsr_eq0 = [&]() -> dword
		{
			m_carry = get_bit(m_registers.cpsr, 29);
			return value >> amount;
		};
		const auto lsr_lt32 = [&]() -> dword
		{
			auto carryout = shift(value, ShiftType::LSR, amount - 1);
			m_carry = get_bit(carryout, 0);

			return value >> amount;
		};
		const auto lsr_eq32 = [&]() -> dword
		{
			m_carry = get_bit(value, 31);
			return 0;
		};
		const auto lsr_gt32 = [&]() -> dword
		{
			m_carry = 0;
			return 0;
		};

		const auto asr_eq0 = [&]()->dword
		{
			m_carry = get_bit(m_registers.cpsr, 29);
			return value;
		};
		const auto asr_lt32 = [&]() -> dword
		{
			m_carry = get_bit(value >> (amount - 1), 0);
			return value > 0 ? value >> amount : ~(~value >> amount);
		};
		const auto asr_eq32 = [&]() -> dword
		{
			m_carry = get_bit(value, 31);
			auto flip = ~static_cast<dword>(0); //0u
			return value > 0 ? set_bit(flip, 31, 0) : flip;
		};
		const auto asr_gt32 = [&]() -> dword
		{
			m_carry = get_bit(value, 31);
			auto flip = ~static_cast<dword>(0); //0u
			return value > 0 ? set_bit(flip, 31, 0) : flip;
		};

		const auto ror_eq0 = [&]() -> dword
		{
			m_carry = get_bit(value, 0);
			return (value >> 1) | (get_bit(m_registers.cpsr, 29) << 31);
		};
		const auto ror_lt32 = [&]() -> dword
		{
			m_carry = get_bit((value >> (amount - 1)), 0);
			return (value << amount) | (value >> (size - amount));
		};
		const auto ror_eq32 = [&]() -> dword
		{
			m_carry = get_bit(value, 31);
			return value;
		};
		const auto ror_gt32 = [&]() -> dword
		{
			amount %= 32;
			return ror_lt32();
		};
		


		const std::array<shiftFunc, 4> eq0{ lsl_eq0, lsr_eq0, asr_eq0, ror_eq0 };
		const std::array<shiftFunc, 4> lt32{ lsl_lt32, lsr_lt32, asr_lt32, ror_lt32 };
		const std::array<shiftFunc, 4> eq32{ lsl_eq32, lsr_eq32, asr_eq32, ror_eq32 };
		const std::array<shiftFunc, 4> gt32{ lsl_gt32, lsr_gt32, asr_gt32, ror_gt32 };



		auto index = static_cast<unsigned int>(type);

		if (amount ==  0) return std::invoke( eq0[index]);
		if (amount <  32) return std::invoke(lt32[index]);
		if (amount == 32) return std::invoke(eq32[index]);
		if (amount >  32) return std::invoke(gt32[index]);
		throw std::runtime_error("Invalid amount given");
	}


	
	bool ARM7TDMI::is_condition_passed() const
	{
		byte cond = m_instruction.cond;

		auto N = get_bit(cond, 3);
		auto Z = get_bit(cond, 2);
		auto C = get_bit(cond, 1);
		auto V = get_bit(cond, 0);

		switch (cond)
		{
			case 0xE: return true;			 //AL

			case 0x0: return  Z;			 //EQ
			case 0x1: return !Z;			 //NE
			case 0x2: return  C;			 //CS
			case 0x3: return !C;			 //CC
			case 0x4: return  N;			 //MI
			case 0x5: return !N;			 //PL
			case 0x6: return  V;			 //VS
			case 0x7: return !V;			 //VC
			case 0x8: return  C && !Z;		 //HI
			case 0x9: return !C || Z;		 //LS
			case 0xA: return  N == V;		 //GE
			case 0xB: return  N != V;		 //LT
			case 0xC: return !Z && (N == V); //GT
			case 0xD: return  Z || (N != V); //LE
				
			case 0xF: return false;			 //RESERVED
			default:  return false;
		}
	}
	void ARM7TDMI::set_instruction_values()
	{
		using setFunc = std::function<void()>;
		Instruction& i = m_instruction;



		const auto set_dataprocessing = [&]() -> void
		{
			i.i        = (i >> 25) & 0x1;
			i.opcode   = (i >> 21) & 0xF;
			i.s        = (i >> 20) & 0x1;
			i.rn       = (i >> 16) & 0xF;
			i.rd       = (i >> 12) & 0xF;
			i.operand2 = i & 0xFFF;
		};
		const auto set_multiply = [&]() -> void
		{
			i.a  = (i >> 21) & 0x1;
			i.s  = (i >> 20) & 0x1;
			i.rd = (i >> 16) & 0xF;
			i.rn = (i >> 12) & 0xF;
			i.rs = (i >>  8) & 0xF;
			i.rm = (i >>  0) & 0xF;
		};
		const auto set_multiply_long = [&]() -> void
		{
			i.u    = (i >> 22) & 0x1;
			i.a    = (i >> 21) & 0x1;
			i.s    = (i >> 20) & 0x1;
			i.rdhi = (i >> 16) & 0xF;
			i.rdlo = (i >> 12) & 0xF;
			i.rn   = (i >>  8) & 0xF;
			i.rm   = (i >>  0) & 0xF;
		};
		const auto set_single_data_swap = [&]()
		{
			i.b  = (i >> 22) & 0x1;
			i.rn = (i >> 16) & 0xF;
			i.rd = (i >> 12) & 0xF;
			i.rm = (i >>  0) & 0xF;
		};
		const auto set_branch_and_exchange = [&]()
		{
			i.rn = (i >> 0) & 0xF;
		};
		const auto set_halfword_data_transfer_register_offset = [&]()
		{
			i.p  = (i >> 24) & 0x1;
			i.u  = (i >> 23) & 0x1;
			i.w  = (i >> 21) & 0x1;
			i.l  = (i >> 20) & 0x1;
			i.rn = (i >> 16) & 0xF;
			i.rd = (i >> 12) & 0xF;
			i.s  = (i >>  6) & 0x1;
			i.h  = (i >>  5) & 0x1;
			i.rm = (i >>  0) & 0xF;
		};
		const auto set_halfword_data_transfer_immediate_offset = [&]()
		{
			i.p      = (i >> 24) & 0x1;
			i.u      = (i >> 23) & 0x1;
			i.w      = (i >> 21) & 0x1;
			i.l      = (i >> 20) & 0x1;
			i.rn     = (i >> 16) & 0xF;
			i.rd     = (i >> 12) & 0xF;
			i.offset = ((i >> 4) & 0xF0) | ((i >> 0) & 0xF);
			i.s      = (i >> 6)  & 0x1;
			i.h      = (i >> 5)  & 0x1;
		};
		const auto set_single_data_transfer = [&]()
		{
			i.i      = (i >> 25) & 0x1;
			i.p      = (i >> 24) & 0x1;
			i.u      = (i >> 23) & 0x1;
			i.b      = (i >> 22) & 0x1;
			i.w      = (i >> 21) & 0x1;
			i.l      = (i >> 20) & 0x1;
			i.rn     = (i >> 16) & 0xF;
			i.rd     = (i >> 12) & 0xF;
			i.offset = (i >>  0) & 0xFFF;
		};
		const auto set_undefined = [&]()
		{

		};
		const auto set_block_data_transfer = [&]()
		{
			i.p            = (i >> 24) & 0x1;
			i.u            = (i >> 23) & 0x1;
			i.s            = (i >> 22) & 0x1;
			i.w            = (i >> 21) & 0x1;
			i.l            = (i >> 20) & 0x1;
			i.rn           = (i >> 16) & 0xF;
			i.registerlist = (1 >>  0) & 0xFFFF;
		};
		const auto set_branch = [&]()
		{
			i.l      = (i >> 24) & 0x1;
			i.offset = (i >>  0) & 0xFFFFFF;
		};
		const auto set_coprocessor_data_transfer = [&]()
		{
			i.p      = (i >> 24) & 0x1;
			i.u      = (i >> 23) & 0x1;
			i.n      = (i >> 22) & 0x1;
			i.w      = (i >> 21) & 0x1;
			i.l      = (i >> 20) & 0x1;
			i.rn     = (i >> 16) & 0xF;
			i.crd    = (i >> 12) & 0xF;
			i.cpn    = (i >>  8) & 0xF;
			i.offset = (i >>  0) & 0xFF;
		};
		const auto set_coprocessor_data_operation = [&]()
		{
			i.cpOpc = (i >> 20) & 0xF;
			i.crn   = (i >> 16) & 0xF;
			i.crd   = (i >> 12) & 0xF;
			i.cpn   = (i >>  8) & 0xF;
			i.cp    = (i >>  5) & 0x7;
			i.crm   = (i >>  0) & 0xF;
		};
		const auto set_coprocessor_register_transfer = [&]()
		{
			i.cpOpc = (i >> 21) & 0x7;
			i.l     = (i >> 20) & 0x1;
			i.crn   = (i >> 16) & 0xF;
			i.rd    = (i >> 12) & 0xF;
			i.cpn   = (i >> 8) & 0xF;
			i.cp    = (i >> 5) & 0x7;
			i.crm   = (i >> 0) & 0xF;
		};
		const auto set_software_interrupt = [&]()
		{

		};

		const std::array<setFunc, 16> setFuncs
		{
			set_dataprocessing, 
			set_multiply, 
			set_multiply_long, 
			set_single_data_swap, 
			set_branch_and_exchange, 
			set_halfword_data_transfer_register_offset, 
			set_halfword_data_transfer_immediate_offset, 
			set_single_data_transfer, 
			set_undefined, 
			set_block_data_transfer, 
			set_branch, 
			set_coprocessor_data_transfer, 
			set_coprocessor_data_operation, 
			set_coprocessor_register_transfer, 
			set_software_interrupt, 
		};



		auto index = static_cast<unsigned int>(m_decodedInstruction);
		m_instruction.cond = (m_instruction >> 28) & 0xF;
		
		return std::invoke(setFuncs[index]);
	}

	

#pragma region Data Processing
	void ARM7TDMI::data_processing()
	{
		using dataProcessingFunc = void (ARM7TDMI::*)(Register<dword>&, dword, dword);
		const std::vector<dataProcessingFunc> functions
		{
			i_and,
			i_eor,
			i_sub,
			i_rsb,
			i_add,
			i_adc,
			i_sbc,
			i_rsc,
			i_tst,
			i_teq,
			i_cmp,
			i_cmn,
			i_orr,
			i_mov,
			i_bic,
			i_mvn,
		};



		auto& rd  = m_registers[m_instruction.rd];
		dword op1 = m_registers[m_instruction.rn];
		dword op2{};
		
		auto operand2 = m_instruction.operand2;
		


		if (m_instruction.immediate())                                         //Immediate flag is set
		{
			byte imm    = operand2 & 0xFF;                                     //Unsigned 8bit immediate value
			byte rotate = (operand2 >> 8) & 0xF;                               //Rotation applied to Imm
		
			op2 = shift(imm, ShiftType::ROR, 2 * rotate);                      //TODO: rotate is zero extended => template(uint)
		}
		else                                                                   //Operand2 is a register
		{
			dword value = m_registers[m_instruction & 0xF];                    //Shift register value
			byte type = (m_instruction >> 5) & 0x3;                            //Type of shift
			byte amount{};                                                     //Amount to shift

			bool isRegister = get_bit(m_instruction, 4);                       //Shift amount is a register
			
			if (isRegister) amount = m_registers[m_instruction.rs] & 0xFF;     //Amount is register value after shift
			else            amount = (operand2 >> 7) & 0x1F;                   //Amount is an immediate value
			
			op2 = shift(value, static_cast<ShiftType>(type), amount);          //TODO: idem as above
		}
		


		std::invoke(functions[m_instruction.opcode], this, rd, op1, op2);
	}

	void ARM7TDMI::i_and(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op1 & op2;
	}
	void ARM7TDMI::i_eor(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op1 ^ op2;
	}
	void ARM7TDMI::i_sub(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op1 - op2;
	}
	void ARM7TDMI::i_rsb(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op2 - op1;
	}
	void ARM7TDMI::i_add(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op1 + op2;
	}
	void ARM7TDMI::i_adc(Register<dword>& rd, dword op1, dword op2)
	{
		bit carry = m_registers.cpsr[29];
		rd = op1 + op2 + carry;
	}
	void ARM7TDMI::i_sbc(Register<dword>& rd, dword op1, dword op2)
	{
		bit carry = m_registers.cpsr[29];
		rd = op1 - op2 + carry - 1;
	}
	void ARM7TDMI::i_rsc(Register<dword>& rd, dword op1, dword op2)
	{
		bit carry = m_registers.cpsr[29];
		rd = op2 - op1 + carry - 1;
	}
	void ARM7TDMI::i_tst(Register<dword>& rd, dword op1, dword op2)
	{
		//Set condition codes n & m
	}
	void ARM7TDMI::i_teq(Register<dword>& rd, dword op1, dword op2)
	{
		//Set condition codes n & m
	}
	void ARM7TDMI::i_cmp(Register<dword>& rd, dword op1, dword op2)
	{
		//Set condition codes n & m
	}
	void ARM7TDMI::i_cmn(Register<dword>& rd, dword op1, dword op2)
	{
		//Set condition codes n & m
	}
	void ARM7TDMI::i_orr(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op1 | op2;
	}
	void ARM7TDMI::i_mov(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op2;
	}
	void ARM7TDMI::i_bic(Register<dword>& rd, dword op1, dword op2)
	{
		rd = op1 & ~op2;
	}
	void ARM7TDMI::i_mvn(Register<dword>& rd, dword op1, dword op2)
	{
		rd = ~op2;
	}
#pragma endregion
#pragma region PSR Transfer
	void ARM7TDMI::psr_transfer()
	{
		if (m_operatingMode == OperatingMode::USR) return;


		const auto get_psr = [&]() -> Register<dword>&
		{
			if (get_bit(m_instruction, 22))
			{
				switch (m_operatingMode)
				{
					case OperatingMode::FIQ: return m_registers.spsr_fiq;
					case OperatingMode::IRQ: return m_registers.spsr_irq;
					case OperatingMode::SVC: return m_registers.spsr_svc;
					case OperatingMode::ABT: return m_registers.spsr_abt;
					case OperatingMode::UND: return m_registers.spsr_und;
					default:                 throw std::runtime_error("Invalid operating mode");
				}
			}
			else return m_registers.cpsr;
		};
		auto& psr = get_psr();                                                 //Fetch PSR register depending on operatingMode


		
		if (get_bit(m_instruction, 21))                                        //Bit 21 set => MRS
		{
			auto& rd = m_registers[m_instruction.rd];                          //Destination register

			i_mrs(rd, psr);
		}
		else                                                                   //Bit 21 not set => MSR
		{
			dword op2{};
			bool flags{};
			
			if (get_bit(m_instruction, 16))                                    //Bit 16 set => Register transfer
			{
				op2 = m_registers[m_instruction.rm];
			}
			else                                                               //Bit 16 not set => Flag bits transfer only
			{
				if (m_instruction.immediate())                                 //Operand is an immediate value
				{
					byte imm = m_instruction & 0xFF;                           //Unsigned 8bit immediate value
					byte rotate = (m_instruction >> 8) & 0xF;                  //Rotation applied to Imm

					op2 = shift(imm, ShiftType::ROR, 2 * rotate);              //TODO: rotate is zero extended => template(uint)
				}
				else op2 = m_registers[m_instruction.rm];

				flags = true;                                                  //Only transfer flag bits to PSR
			}

			i_msr(psr, op2, flags);
		}
	}

	void ARM7TDMI::i_mrs(Register<dword>& rd, dword psr)
	{
		rd = psr;
	}
	void ARM7TDMI::i_msr(Register<dword>& psr, dword op2, bool flags)
	{
		if (flags)
		{
			psr &= 0x0FFFFFFF;
			psr |= op2 & 0xF0000000;
		}
		else
		{
			psr = op2;
		}
	}
#pragma endregion
#pragma region Multiply
	void ARM7TDMI::multiply()
	{
		auto& rd = m_registers[m_instruction.rd];
		dword rn = m_registers[m_instruction.rn];
		dword rm = m_registers[m_instruction.rm];
		dword rs = m_registers[m_instruction.rs];

		if (m_instruction.accumulate()) i_mla(rd, rn, rm, rs);
		else                            i_mul(rd, rn, rm);
	}

	void ARM7TDMI::i_mul(Register<dword>& rd, dword rm, dword rs)
	{
		rd = rm * rs;
		
		m_registers[m_instruction.rn] = 0;
	}
	void ARM7TDMI::i_mla(Register<dword>& rd, dword rn, dword rm, dword rs)
	{
		rd = rm * rs + rn;
	}
#pragma endregion
#pragma region Multiply Long
	void ARM7TDMI::multiply_long()
	{
		auto& rdhi = m_registers[m_instruction.rdhi];
		auto& rdlo = m_registers[m_instruction.rdlo];
		dword rm = m_registers[m_instruction.rm];
		dword rs = m_registers[m_instruction.rs];

		const byte bits = (m_instruction >> 21) & 0x3;

		switch (bits)
		{
			case 0b00: i_umull(rdhi, rdlo, rm, rs);
			case 0b01: i_umlal(rdhi, rdlo, rm, rs);
			case 0b10: i_smull(rdhi, rdlo, rm, rs);
			case 0b11: i_smlal(rdhi, rdlo, rm, rs);

			default:   throw std::runtime_error("Invalid instruction bits");
		}
	}

	void ARM7TDMI::i_umull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
	{
		qword result = static_cast<qword>(rm) * static_cast<qword>(rs);

		rdhi = (result >> 32) & ~0U;
		rdlo = result & ~0U;
	}
	void ARM7TDMI::i_smull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
	{
		auto srm = static_cast<long long>(rm);
		auto srs = static_cast<long long>(rs);
		long long result = srm * srs;

		rdhi = (result >> 32) & ~0U;
		rdlo = result & ~0U;
	}
	void ARM7TDMI::i_umlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
	{
		qword result = static_cast<qword>(rm) * static_cast<qword>(rs);

		rdhi = ((result >> 32) & ~0U) + rdhi;
		rdlo = (result & ~0U) + rdlo;
	}
	void ARM7TDMI::i_smlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
	{
		auto srm = static_cast<long long>(rm);
		auto srs = static_cast<long long>(rs);
		long long result = srm * srs;

		rdhi = ((result >> 32) & ~0U) + rdhi;
		rdlo = (result & ~0U) + rdlo;
	}
#pragma endregion
#pragma region Single Data Swap
	void ARM7TDMI::single_data_swap()
	{
		auto& rd = m_registers[m_instruction.rd];
		auto& rm = m_registers[m_instruction.rm];
		dword rn = m_registers[m_instruction.rn];

		i_swp(rd, rm, rn, m_instruction.b);
	}

	void ARM7TDMI::i_swp(Register<dword>& rd, Register<dword>& rm, dword address, bool type)
	{
		bus->set(address);
		dword content{};
		
		if (type)
		{
			content = bus->read<byte>();
			bus->write<byte>(rm);
		}
		else
		{
			content = bus->read<dword>();
			bus->write<dword>(rm);
		}
		
		rd = content;
	}
#pragma endregion
#pragma region Branch and Exchange
	void ARM7TDMI::branch_exchange()
	{
		auto& pc = m_registers.pc;
		dword op1 = m_registers[m_instruction.rn];

		i_bx(op1);
	}

	void ARM7TDMI::i_bx(dword op1)
	{
		m_registers.pc = op1;
		
		if (get_bit(op1, 0)) m_operatingState = OperatingState::THUMB;         //if bit 0 is set, switch to THUMB mode
		else                 m_operatingState = OperatingState::ARM;           //if bit 0 is clear, switch to ARM mode
	}
#pragma endregion
#pragma region Halfword Data Transfer
	void ARM7TDMI::halfword_data_transfer()
	{
		auto& rd = m_registers[m_instruction.rd];
		dword rn = m_registers[m_instruction.rn];
		dword rm = m_registers[m_instruction.rm];
		dword offset{};

		const byte bits = (m_instruction >> 5) & 0x3;
		const int sign = m_instruction.up_down() ? 1 : -1;
		


		if (get_bit(m_instruction, 22)) offset = ((m_instruction & 0xF00) >> 4) | (m_instruction & 0xF);
		else                            offset = rm;
		
		offset *= sign;

		

		if (m_instruction.pre_post_index())                                    //Offset is updated before load or store
		{
			offset += rn;

			if (m_instruction.writeback()) rn = offset;                        //Optional writeback to base register
		}


		
		bus->set(offset);
		
		switch (bits) //TODO: (p.32) 0b00 is SWP instruction?
		{
			case 0b00: i_strh(rd);  break;
			case 0b01: i_ldrh(rd);  break;
			case 0b10: i_strh(rd);  break;
			case 0b11: i_ldrsb(rd); break;

			default:   throw std::runtime_error("Invalid instruction bits");
		}


		
		if (!m_instruction.pre_post_index()) rn = offset;
	}

	void ARM7TDMI::i_ldrh(Register<dword>& rd) //TODO: zero extend
	{
		rd = bus->read<word>();
	}
	void ARM7TDMI::i_strh(Register<dword>& rd)
	{
		bus->write<word>(rd);
	}
	void ARM7TDMI::i_ldrsb(Register<dword>& rd) //TODO: sign extend
	{
		rd = bus->read<byte>();
	}
	void ARM7TDMI::i_ldrsh(Register<dword>& rd) //TODO: sign extend
	{
		rd = bus->read<word>();
	}
#pragma endregion
#pragma region Single Data Transfer
	void ARM7TDMI::single_data_transfer()
	{
		auto& rd = m_registers[m_instruction.rd];
		auto& rn = m_registers[m_instruction.rn];
		int offset{};

		const int sign = m_instruction.up_down() ? 1 : -1;
		
		

		if (m_instruction.immediate())                                         //Offset is a register
		{
			dword value =  m_registers[m_instruction & 0xF];                   //Shift register value
			byte type   = (m_instruction >> 5) & 0x3;                          //Type of shift
			byte amount = (m_instruction >> 7) & 0x1F;                         //Amount to shift

			offset = shift(value, static_cast<ShiftType>(type), amount);       //TODO: idem as above
		}
		else offset = m_instruction.offset & 0xFFF;                            //Offset is an immediate value
		
		offset *= sign;                                                        //Offset is added or subtracted
		

		
		if (m_instruction.pre_post_index())                                    //Offset is updated before load or store
		{
			offset += rn;

			if (m_instruction.writeback()) rn = offset;                        //Optional writeback to base register
		}
		

		
		bus->set(offset);                                                      //Set address to read/write from
		
		if (m_instruction.load_store()) i_ldr(rd, m_instruction.b);            //Load address into register
		else                            i_str(rd, m_instruction.b);            //Store register into address


		
		if (!m_instruction.pre_post_index()) rn = offset;                      //Base register is updated after load or store
	}

	void ARM7TDMI::i_ldr(Register<dword>& rd, bool b)
	{
		if (b) rd = bus->read<byte>();
		else   rd = bus->read<dword>();
	}
	void ARM7TDMI::i_str(Register<dword>& rd, bool b)
	{
		if (b)
		{
			dword value = (rd << 24 | rd << 16 | rd << 8 | rd); //TODO: maybe wrong, dont know if value repeat or [31:8] <= sign bit
			bus->write<dword>(value);
		}
		else bus->write<dword>(rd);
	}
#pragma endregion
#pragma region Undefined
	void ARM7TDMI::undefined()
	{
		i_und();
	}

	void ARM7TDMI::i_und()
	{
		__debugbreak();
	}
#pragma endregion
#pragma region Block Data Transfer
	void ARM7TDMI::block_data_transfer()                                       //TODO: (p.40) S bit and mode changes
	{
		auto& rn = m_registers[m_instruction.rn];
		dword address = rn;

		bool up = m_instruction.up_down();
		int offset = up ? 4 : -4;
		
		std::vector<std::reference_wrapper<Register<dword>>> registers{};



		for (int i = 0; i < sizeof(word) * 8; ++i)                             //Create vector of registers to read/write
		{
			if (get_bit(m_instruction.registerlist, i))
			{
				registers.emplace_back(m_registers[i]);
			}
		}

		

		if (m_instruction.pre_post_index()) address += offset;                 //Address is adjusted by 1 word before instruction
		

		
		if (!up) address -= registers.size() * 4;
		
		if (m_instruction.load_store()) i_ldm(registers, address);             //Load registers from memory
		else                            i_stm(registers, address);             //Store registers to memory
		


		if (!m_instruction.pre_post_index()) address += offset;                //Address is adjusted by 1 word after instruction
		if (m_instruction.writeback()) rn = address;                           //Optional writeback new address to base register
	}

	void ARM7TDMI::i_ldm(std::vector<std::reference_wrapper<Register<dword>>> registers, dword address)
	{
		int offset{};
		
		for (auto& reg : registers)
		{
			bus->set(address + offset);
			reg.get() = bus->read<dword>();

			offset += 4;
		}
	}
	void ARM7TDMI::i_stm(std::vector<std::reference_wrapper<Register<dword>>> registers, dword address)
	{
		int offset{};

		for (const auto& reg : registers)
		{
			bus->set(address + offset);
			bus->write<dword>(reg.get());

			offset += 4;
		}
	}
#pragma endregion
#pragma region Branch
	void ARM7TDMI::branch() //TODO: copy branch code to branch with link => 2 independent functions to represent the 2 instructions
	{
		dword offset = m_instruction & 0xFFFFFF;

		offset <<= 2;
		offset = extend<dword, 25>(offset);

		if (m_instruction.link()) i_b(offset);
		else                      i_bl(offset);
	}

	void ARM7TDMI::i_b(dword offset)
	{
		m_registers.pc += offset;
	}
	void ARM7TDMI::i_bl(dword offset)
	{
		m_registers.lr = static_cast<dword>(m_registers.pc) + 4;
		m_registers.lr &= 0xFFFFFFFC;

		m_registers.pc += offset;
	}
#pragma endregion
#pragma region Coprocessor Data Transfer
	void ARM7TDMI::coprocessor_data_transfer()
	{
		__debugbreak();
	}

	void ARM7TDMI::i_ldc()
	{

	}
	void ARM7TDMI::i_stc()
	{
		
	}
#pragma endregion
#pragma region Coprocessor Data Operation
	void ARM7TDMI::coprocessor_data_operation()
	{
		__debugbreak();
	}

	void ARM7TDMI::i_cdp()
	{
		
	}
#pragma endregion
#pragma region Coprocessor Register Transfer
	void ARM7TDMI::coprocessor_register_transfer()
	{
		__debugbreak();
	}

	void ARM7TDMI::i_mcr()
	{
		
	}
	void ARM7TDMI::i_mrc()
	{
		
	}
#pragma endregion
#pragma region Software Interrupt
	void ARM7TDMI::software_interrupt()
	{
		i_swi();
	}

	void ARM7TDMI::i_swi()
	{
		m_registers.r14_svc = m_registers.pc;
		m_registers.pc = 0x08;
		
		m_registers.spsr_svc = m_registers.cpsr;
	}
#pragma endregion
}
