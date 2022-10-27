#include <stdafx.hpp>

#include "ARM7TDMI.hpp"

#define _DEBUG_OUTPUT true

namespace dot::gba
{
	ARM7TDMI::ARM7TDMI()
	{
		reset();
	}

	void ARM7TDMI::cycle()
	{
		if (m_cycles)                                                          //If we still have cycles to wait, return
		{
			--m_cycles;
			return;
		}

		m_pipeline.shift();                                                    //Shift the pipeline forward at the start of every cycle
                                                                               //Execute and decode functions only run if an instruction
																			   //is present on positions [2] and [1] of the pipeline
		
		if (m_pipeline.load() >= 3) execute();                                 //Execute pipeline instruction 3
		if (m_pipeline.load() >= 2) decode();								   //Decode  pipeline instruction 2
		                            fetch();								   //Fetch   pipeline instruction 1
	}
	void ARM7TDMI::reset()
	{
		m_pipeline.flush();                                                    //Remove all instructions present in the pipeline
		m_registers.reset();                                                   //All registers are set to 0, some values are adjusted based on reset specification

		//m_cycles += 2;                                                         //Wait 2 cycles before fetching the first instruction
	}

	void ARM7TDMI::fetch()
	{
		Instruction instruction{};                                             

		auto& cpsr = m_registers.get_cpsr();
		auto thumb = cpsr.flag(PSR::Flag::Thumb);
		
		if (thumb)
		{
			dword address = m_registers.pc & 0xFFFFFFFE; 				       //2 byte alignment

			instruction = bus->read<word>(address);
			instruction.address = address;

			m_pipeline.push(instruction);

			m_registers.pc += 2;

			__debugbreak();
		}
		else
		{
			dword address = m_registers.pc & 0xFFFFFFFC; 				       //4 byte alignment

			instruction = bus->read<dword>(address);
			instruction.address = address;

			m_pipeline.push(instruction);

			m_registers.pc += 4;
		}
	}
	void ARM7TDMI::decode()
	{
		using setFunc = std::function<void()>;                                 //We set the instruction values according to the operation we will perform
		auto& instruction = m_pipeline[1];                                     //The second instruction in the pipeline is being decoded
		


		const auto set_dataprocessing                          = [&]()
		{
			instruction.i        = (instruction >> 25) & 0x1;
			instruction.opcode   = (instruction >> 21) & 0xF;
			instruction.s        = (instruction >> 20) & 0x1;
			instruction.rn       = (instruction >> 16) & 0xF;
			instruction.rd       = (instruction >> 12) & 0xF;
			instruction.operand2 = instruction & 0xFFF;
		};
		const auto set_multiply                                = [&]()
		{
			instruction.a  = (instruction >> 21) & 0x1;
			instruction.s  = (instruction >> 20) & 0x1;
			instruction.rd = (instruction >> 16) & 0xF;
			instruction.rn = (instruction >> 12) & 0xF;
			instruction.rs = (instruction >> 8) & 0xF;
			instruction.rm = (instruction >> 0) & 0xF;
		};
		const auto set_multiply_long                           = [&]()
		{
			instruction.u    = (instruction >> 22) & 0x1;
			instruction.a    = (instruction >> 21) & 0x1;
			instruction.s    = (instruction >> 20) & 0x1;
			instruction.rdhi = (instruction >> 16) & 0xF;
			instruction.rdlo = (instruction >> 12) & 0xF;
			instruction.rn   = (instruction >> 8) & 0xF;
			instruction.rm   = (instruction >> 0) & 0xF;
		};
		const auto set_single_data_swap                        = [&]()
		{
			instruction.b  = (instruction >> 22) & 0x1;
			instruction.rn = (instruction >> 16) & 0xF;
			instruction.rd = (instruction >> 12) & 0xF;
			instruction.rm = (instruction >> 0) & 0xF;
		};
		const auto set_branch_and_exchange                     = [&]()
		{
			instruction.rn = (instruction >> 0) & 0xF;
		};
		const auto set_halfword_data_transfer_register_offset  = [&]()
		{
			instruction.p  = (instruction >> 24) & 0x1;
			instruction.u  = (instruction >> 23) & 0x1;
			instruction.w  = (instruction >> 21) & 0x1;
			instruction.l  = (instruction >> 20) & 0x1;
			instruction.rn = (instruction >> 16) & 0xF;
			instruction.rd = (instruction >> 12) & 0xF;
			instruction.s  = (instruction >> 6) & 0x1;
			instruction.h  = (instruction >> 5) & 0x1;
			instruction.rm = (instruction >> 0) & 0xF;
		};
		const auto set_halfword_data_transfer_immediate_offset = [&]()
		{
			instruction.p      = (instruction >> 24) & 0x1;
			instruction.u      = (instruction >> 23) & 0x1;
			instruction.w      = (instruction >> 21) & 0x1;
			instruction.l      = (instruction >> 20) & 0x1;
			instruction.rn     = (instruction >> 16) & 0xF;
			instruction.rd     = (instruction >> 12) & 0xF;
			instruction.offset = ((instruction >> 4) & 0xF0) | ((instruction >> 0) & 0xF);
			instruction.s      = (instruction >> 6) & 0x1;
			instruction.h      = (instruction >> 5) & 0x1;
		};
		const auto set_single_data_transfer                    = [&]()
		{
			instruction.i      = (instruction >> 25) & 0x1;
			instruction.p      = (instruction >> 24) & 0x1;
			instruction.u      = (instruction >> 23) & 0x1;
			instruction.b      = (instruction >> 22) & 0x1;
			instruction.w      = (instruction >> 21) & 0x1;
			instruction.l      = (instruction >> 20) & 0x1;
			instruction.rn     = (instruction >> 16) & 0xF;
			instruction.rd     = (instruction >> 12) & 0xF;
			instruction.offset = (instruction >> 0) & 0xFFF;
		};
		const auto set_undefined                               = [&]()
		{

		};
		const auto set_block_data_transfer                     = [&]()
		{
			instruction.p            = (instruction >> 24) & 0x1;
			instruction.u            = (instruction >> 23) & 0x1;
			instruction.s            = (instruction >> 22) & 0x1;
			instruction.w            = (instruction >> 21) & 0x1;
			instruction.l            = (instruction >> 20) & 0x1;
			instruction.rn           = (instruction >> 16) & 0xF;
			instruction.registerlist = (1 >> 0) & 0xFFFF;
		};
		const auto set_branch                                  = [&]()
		{
			instruction.l      = (instruction >> 24) & 0x1;
			instruction.offset = (instruction >> 0) & 0xFFFFFF;
		};
		const auto set_coprocessor_data_transfer               = [&]()
		{
			instruction.p      = (instruction >> 24) & 0x1;
			instruction.u      = (instruction >> 23) & 0x1;
			instruction.n      = (instruction >> 22) & 0x1;
			instruction.w      = (instruction >> 21) & 0x1;
			instruction.l      = (instruction >> 20) & 0x1;
			instruction.rn     = (instruction >> 16) & 0xF;
			instruction.crd    = (instruction >> 12) & 0xF;
			instruction.cpn    = (instruction >> 8) & 0xF;
			instruction.offset = (instruction >> 0) & 0xFF;
		};
		const auto set_coprocessor_data_operation              = [&]()
		{
			instruction.cpOpc = (instruction >> 20) & 0xF;
			instruction.crn   = (instruction >> 16) & 0xF;
			instruction.crd   = (instruction >> 12) & 0xF;
			instruction.cpn   = (instruction >> 8) & 0xF;
			instruction.cp    = (instruction >> 5) & 0x7;
			instruction.crm   = (instruction >> 0) & 0xF;
		};
		const auto set_coprocessor_register_transfer           = [&]()
		{
			instruction.cpOpc = (instruction >> 21) & 0x7;
			instruction.l     = (instruction >> 20) & 0x1;
			instruction.crn   = (instruction >> 16) & 0xF;
			instruction.rd    = (instruction >> 12) & 0xF;
			instruction.cpn   = (instruction >> 8) & 0xF;
			instruction.cp    = (instruction >> 5) & 0x7;
			instruction.crm   = (instruction >> 0) & 0xF;
		};
		const auto set_software_interrupt                      = [&]()
		{

		};

		const std::array<setFunc, 15> setFuncs
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

		

		for (int i = 14; i > -1; --i)                                          //We match the functions in reverse order: specific to general
		{																	   
			auto mask = instruction & architecture::ARM_INSTRUCTION_MASK[i];   
			bool match = (mask == architecture::ARM_INSTRUCTION_FORMAT[i]);    

			if (match)                                                         //If a match is found, we know which operation to perform
			{
				m_operation = static_cast<Operation>(i);
				instruction.cond = (instruction >> 28) & 0xF;

				return std::invoke(setFuncs[i]);
			}
		}

		throw std::runtime_error("Invalid instruction format");                //If no match is found, the instruction is invalid
	}
	void ARM7TDMI::execute()
	{
		const auto& instruction = m_pipeline[2];                               
		
		const auto condition_passed = [&]() -> bool
		{
			const auto& cpsr = m_registers.get_cpsr();

			const auto n = cpsr[31];
			const auto z = cpsr[30];
			const auto c = cpsr[29];
			const auto v = cpsr[28];

			switch (instruction.cond)
			{
				case 0xE: return true;			                               //AL
				case 0xF: return false;			                               //RESERVED
												                               
				case 0x0: return  z;			                               //EQ
				case 0x1: return !z;			                               //NE
				case 0x2: return  c;			                               //CS
				case 0x3: return !c;			                               //CC
				case 0x4: return  n;			                               //MI
				case 0x5: return !n;			                               //PL
				case 0x6: return  v;			                               //VS
				case 0x7: return !v;			                               //VC
				case 0x8: return  c && !z;		                               //HI
				case 0x9: return !c || z;		                               //LS
				case 0xA: return  n == v;		                               //GE
				case 0xB: return  n != v;		                               //LT
				case 0xC: return !z && (n == v);                               //GT
				case 0xD: return  z || (n != v);                               //LE

				default:  return false;
			}
		}();
		if (!condition_passed) return;                                         //Every instruction has a 4-bit condition code, which determines if that instruction is executed



#if _DEBUG_OUTPUT
		std::unordered_map<Operation, const char*> operationToStringMap
		{
			{ Operation::DataProcessing, "DataProcessing" },
			{ Operation::Multiply, "Multiply" },
			{ Operation::MultiplyLong, "MultiplyLong" },
			{ Operation::SingleDataSwap, "SingleDataSwap" },
			{ Operation::BranchExchange, "BranchExchange" },
			{ Operation::HalfwordDataTransferRegisterOffset, "HalfwordDataTransferRegisterOffset" },
			{ Operation::HalfwordDataTransferImmediateOffset, "HalfwordDataTransferImmediateOffset" },
			{ Operation::SingleDataTransfer, "SingleDataTransfer" },
			{ Operation::Undefined, "Undefined" },
			{ Operation::BlockDataTransfer, "BlockDataTransfer" },
			{ Operation::Branch, "Branch" },
			{ Operation::CoprocessorDataTransfer, "CoprocessorDataTransfer" },
			{ Operation::CoprocessorDataOperation, "CoprocessorDataOperation" },
			{ Operation::CoprocessorRegisterTransfer, "CoprocessorRegisterTransfer" },
			{ Operation::SoftwareInterrupt, "SoftwareInterrupt" }
		};

		std::cout << "[0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << instruction.address << "] " << "0x" << std::setw(8) << std::setfill('0') << instruction << " \"" << operationToStringMap[m_operation] << "\"" << std::endl;
#endif



		using instructionFormat = void (ARM7TDMI::*)(const Instruction&);
		const std::unordered_map<Operation, instructionFormat> operationToInstructionMap
		{
			{ Operation::DataProcessing,                      &ARM7TDMI::data_processing }, 
			{ Operation::Multiply,                            &ARM7TDMI::multiply }, 
			{ Operation::MultiplyLong,                        &ARM7TDMI::multiply_long }, 
			{ Operation::SingleDataSwap,                      &ARM7TDMI::single_data_swap }, 
			{ Operation::BranchExchange,                      &ARM7TDMI::branch_exchange }, 
			{ Operation::HalfwordDataTransferRegisterOffset,  &ARM7TDMI::halfword_data_transfer }, 
			{ Operation::HalfwordDataTransferImmediateOffset, &ARM7TDMI::halfword_data_transfer }, 
			{ Operation::SingleDataTransfer,                  &ARM7TDMI::single_data_transfer }, 
			{ Operation::Undefined,                           &ARM7TDMI::undefined }, 
			{ Operation::BlockDataTransfer,                   &ARM7TDMI::block_data_transfer }, 
			{ Operation::Branch,                              &ARM7TDMI::branch }, 
			{ Operation::CoprocessorDataTransfer,             &ARM7TDMI::coprocessor_data_transfer }, 
			{ Operation::CoprocessorDataOperation,            &ARM7TDMI::coprocessor_data_operation }, 
			{ Operation::CoprocessorRegisterTransfer,         &ARM7TDMI::coprocessor_register_transfer }, 
			{ Operation::SoftwareInterrupt,                   &ARM7TDMI::software_interrupt }, 
		};

		

		std::invoke(operationToInstructionMap.at(m_operation), this, instruction);
	}

	void ARM7TDMI::exception()
	{
		//p.62;
	}
	void ARM7TDMI::abort()
	{

	}


	
	std::pair<dword, bit> ARM7TDMI::shift_carry(dword value, ShiftType type, dword amount, bit carryFlag)
	{
		dword result{};                                                        //Result after shift function
		bit carryOut{};                                                        //Carryout bit

		auto size = sizeof(dword) * 8;                                         //Total amount of bits in value


		
		using shiftFunc = std::function<void()>;                               //TODO: if destination register is r15, increment its value by 4
		
		const auto lsl_eq0  = [&]()
		{
			result = value;
			carryOut = carryFlag;
		};
		const auto lsl_lt32 = [&]()
		{
			result = value << amount;
			carryOut = get_bit((value >> (size - amount)), 0);
		};
		const auto lsl_eq32 = [&]()
		{
			result = 0;
			carryOut = get_bit(value, 0);
		};
		const auto lsl_gt32 = [&]()
		{
			result = 0;
			carryOut = 0;
		};

		const auto lsr_eq0  = [&]()
		{
			lsl_eq0();
		};
		const auto lsr_lt32 = [&]()
		{
			result = value >> amount;
			carryOut = get_bit((value >> (amount - 1)), 0);
		};
		const auto lsr_eq32 = [&]()
		{
			result = 0;
			carryOut = get_bit(value, 31);
		};
		const auto lsr_gt32 = [&]()
		{
			result = 0;
			carryOut = 0;
		};

		const auto asr_eq0  = [&]()
		{
			lsl_eq0();
		};
		const auto asr_lt32 = [&]()
		{
			result = get_bit(value, 31) ? ~(~value >> amount) : value >> amount;
			carryOut = get_bit((value >> (amount - 1)), 0);
		};
		const auto asr_eq32 = [&]()
		{
			result = get_bit(value, 31) ? ~0 : 0;
			carryOut = get_bit(value, 31);
		};
		const auto asr_gt32 = [&]()
		{
			asr_eq32();
		};

		const auto ror_rrx  = [&]()
		{
			result = value >> 1;
			result = set_bit(result, 31, carryFlag);
			carryOut = get_bit(value, 0);
		};
		const auto ror_eq0  = [&]()
		{
			ror_rrx();
		};
		const auto ror_lt32 = [&]()
		{
			result = (value >> amount) | (value << (size - amount));
			carryOut = get_bit(value, amount - 1);
		};
		const auto ror_eq32 = [&]()
		{
			result = value;
			carryOut = get_bit(value, 31);
		};
		const auto ror_gt32 = [&]()
		{
			while (amount > 32) 
				amount -= 32;

			if (amount < 32) ror_lt32();
			else             ror_eq32();
		};

		

		const shiftFunc shiftFuncMatrix[4][4] =
		{
			lsl_eq0, lsl_lt32, lsl_eq32, lsl_gt32, 
			lsr_eq0, lsr_lt32, lsr_eq32, lsr_gt32,
			asr_eq0, asr_lt32, asr_eq32, asr_gt32,
			ror_eq0, ror_lt32, ror_eq32, ror_gt32, 
		};
		
		auto x = static_cast<unsigned int>(type);
		auto y = [amount]() -> unsigned int
		{
			if (amount ==  0) return 0;
			if (amount  < 32) return 1;
			if (amount == 32) return 2;
			if (amount  > 32) return 3;

			throw std::invalid_argument("Invalid shift amount");
		}();

		std::invoke(shiftFuncMatrix[x][y]);


		
		return { result, carryOut };
	}



//ARM Instructions
#pragma region Data Processing
	void ARM7TDMI::data_processing(const Instruction& instruction)
	{
		enum class FunctionType { Logical, Arithmetic };

		using dataProcessingFunc = void (ARM7TDMI::*)(Register<dword>&, dword, dword);
		const std::array<std::pair<dataProcessingFunc, FunctionType>, 16> functionByType
		{
			std::make_pair( &ARM7TDMI::i_and, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_eor, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_sub, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_rsb, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_add, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_adc, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_sbc, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_rsc, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_tst, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_teq, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_cmp, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_cmn, FunctionType::Arithmetic ), 
			std::make_pair( &ARM7TDMI::i_orr, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_mov, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_bic, FunctionType::Logical ), 
			std::make_pair( &ARM7TDMI::i_mvn, FunctionType::Logical ), 
		};


		
		auto& rd = m_registers[instruction.rd];                                
		dword rn = m_registers[instruction.rn];                                
		dword op2{};														   
		
		const auto& cpsr = m_registers.get_cpsr();
		auto carry = cpsr.flag(PSR::Flag::Carry);


		
		if (instruction.flag(Instruction::Flag::Immediate))                    
		{
			byte imm = instruction.operand2 & 0xFF;                            
			byte rotate = (instruction.operand2 >> 8) & 0xF;                   

			if (rotate) std::tie(op2, carry) = shift_carry(imm, ShiftType::ROR, 2 * rotate, carry);
			else        op2 = imm;
		}
		else                                                                   
		{
			if (instruction.rn == 15) rn += 4;                                 //Adjust for prefetch if Rn is PC
			
			dword rm = m_registers[instruction & 0xF];                         
			auto type = static_cast<ShiftType>((instruction >> 5) & 0x3);      
			byte shiftAmount{};                                                     

			auto isRegister = get_bit(instruction, 4);                         

			if (isRegister) shiftAmount = m_registers[instruction.rs] & 0xFF;  
			else            shiftAmount = (instruction.operand2 >> 7) & 0x1F;  

			std::tie(op2, carry) = shift_carry(rm, type, shiftAmount, carry);
		}

		
		
		const auto opcode = instruction.opcode;                                //4 bit opcode which determines the operation to perform
		const auto& [func, type] = functionByType.at(opcode);                  
		Register<dword> result{};                                              //Store the result in a placeholder value => not all operations may write back to the destination register

		auto isTest = get_bit(opcode, 3) && !get_bit(opcode, 2);               //TST, TEQ, CMP and CMN instructions have bit pattern [0b10xx]

		if (isTest)                                                            
		{
			if (instruction.flag(Instruction::Flag::Set)) std::invoke(func, this, result, rn, op2); //Invoke on placeholder value so Rd is not modified
			else                                          return psr_transfer(instruction);         //If the S flag is not set, the instruction is a PSR transfer
		}
		else                                                                   
		{
			std::invoke(func, this, rd, rn, op2);                              //The value of rd will be changed, and we copy the result                             
			result = rd;                                                       
		}
		


		if (instruction.flag(Instruction::Flag::Set))
		{
			auto& cpsr = m_registers.get_cpsr();

			if (instruction.rd == 15)                                          //Special case if the destination register is the PC
			{
				if (cpsr.mode()  == OperatingMode::USR) throw std::runtime_error("SPSR is not accessible in USR mode");
				
				cpsr = m_registers.get_spsr(); //TODO: this whole method needs a rewrite
			}
			else
			{
				cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, result[31]);  
				cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     result == 0); 
				cpsr = set_bit<dword>(cpsr, PSR::Flag::Carry,    carry);       

				if (type == FunctionType::Arithmetic)                          
				{
					auto signRn     = get_bit(rn, 31);                         //Check for overflow by comparing the sign of the operands
					auto signOp2    = get_bit(op2, 31);                        //If the signs are the same, but the result has a different sign, overflow has occurred
					auto signResult = get_bit(result, 31);                     //(i.e. the sign of the result is different from the signs of the operands)
					
					auto v = ((signRn == signOp2) && (signResult != signRn));
					cpsr = set_bit<dword>(cpsr, PSR::Flag::Overflow, v);
				}
			}
		}



#if _DEBUG_OUTPUT
		std::unordered_map<unsigned int, const char*> opcodeToFunctionMap
		{
			{  0, "AND" },  { 1, "EOR" }, {  2, "SUB" }, {  3, "RSB" }, 
			{  4, "ADD" },  { 5, "ADC" }, {  6, "SBC" }, {  7, "RSC" }, 
			{  8, "TST" },  { 9, "TEQ" }, { 10, "CMP" }, { 11, "CMN" }, 
			{ 12, "ORR" }, { 13, "MOV" }, { 14, "BIC" }, { 15, "MVN" }, 
		};

		std::cout.copyfmt(std::ios(nullptr));

		std::cout << '(' << opcodeToFunctionMap[instruction.opcode] << ") ";
		std::cout << "Rd[" << std::to_string(instruction.rd) << "] = {" << std::to_string(rd) << "} | ";
		std::cout << "Rn[" << std::to_string(instruction.rn) << "] = {" << std::to_string(rn) << "} | ";
		std::cout << "Operand2 = {" << op2 << "} | ";
		std::cout << "Result = {" << result << '}' << '\n';
		
		std::bitset<4> bitset = m_registers.get_cpsr() >> 28;
		std::cout << "Set: " << std::boolalpha << instruction.flag(Instruction::Flag::Set) << ", CPSR = {" << bitset << "}\n\n";
#endif
	}

	void ARM7TDMI::i_and(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn & op2;
	}
	void ARM7TDMI::i_eor(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn ^ op2;
	}
	void ARM7TDMI::i_sub(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn - op2;
	}
	void ARM7TDMI::i_rsb(Register<dword>& rd, dword rn, dword op2)
	{
		rd = op2 - rn;
	}
	void ARM7TDMI::i_add(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn + op2;
	}
	void ARM7TDMI::i_adc(Register<dword>& rd, dword rn, dword op2)
	{
		auto carry = m_registers.get_cpsr().flag(PSR::Flag::Carry);
		rd = rn + op2 + carry;
	}
	void ARM7TDMI::i_sbc(Register<dword>& rd, dword rn, dword op2)
	{
		auto carry = m_registers.get_cpsr().flag(PSR::Flag::Carry);
		rd = rn - op2 + carry - 1;
	}
	void ARM7TDMI::i_rsc(Register<dword>& rd, dword rn, dword op2)
	{
		auto carry = m_registers.get_cpsr().flag(PSR::Flag::Carry);
		rd = op2 - rn + carry - 1;
	}
	void ARM7TDMI::i_tst(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn & op2;
	}
	void ARM7TDMI::i_teq(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn ^ op2;
	}
	void ARM7TDMI::i_cmp(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn - op2;
	}
	void ARM7TDMI::i_cmn(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn + op2;
	}
	void ARM7TDMI::i_orr(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn | op2;
	}
	void ARM7TDMI::i_mov(Register<dword>& rd, dword rn, dword op2)
	{
		rd = op2;
	}
	void ARM7TDMI::i_bic(Register<dword>& rd, dword rn, dword op2)
	{
		rd = rn & ~op2;
	}
	void ARM7TDMI::i_mvn(Register<dword>& rd, dword rn, dword op2)
	{
		rd = ~op2;
	}
#pragma endregion
#pragma region PSR Transfer
	void ARM7TDMI::psr_transfer(const Instruction& instruction)
	{
		auto& cpsr = m_registers.get_cpsr();
		auto& spsr = m_registers.get_spsr();

		auto isMSR   = get_bit(instruction, 21);
		auto useSPSR = get_bit(instruction, 22);
		
		if (isMSR)
		{
			dword result{};
			dword mask{};


			
			auto isImmediate = get_bit(instruction, 25);

			if (isImmediate)
			{
				dword op2 = instruction & 0xFFF;
				
				dword imm = op2 & 0xFF;
				dword rotate = ((op2 >> 8) & 0xF) * 2;

				result = shift(imm, ShiftType::ROR, rotate);
			}
			else
			{
				result = m_registers[instruction.rm];
			}

			
			
			for (int i = 16; i < 20; ++i)                                      //cpsr_fsxc or something => NBA github
			{
				if (get_bit(instruction, i))
				{
					mask |= 0xFF;
					mask <<= 4;
				}
			}

			if (useSPSR)
			{
				i_msr(spsr, result, mask);
			}
			else
			{
				if (cpsr.mode() == OperatingMode::USR) mask   &= 0xFF000000;
				if (mask & 0xFF)                       result |= 0x00000010;   //idk

				//TODO: switch mode?

				i_msr(cpsr, result, mask);

			}
		}
		else
		{
			auto& rd = m_registers[instruction.rd];

			if (useSPSR) i_mrs(rd, spsr);
			else         i_mrs(rd, cpsr);
		}
	}

	void ARM7TDMI::i_mrs(Register<dword>& rd, const PSR& psr)
	{
		rd = psr;
	}
	void ARM7TDMI::i_msr(PSR& psr, dword op2, dword mask)
	{
		psr = (psr & ~mask) | (op2 & mask);
	}
#pragma endregion
#pragma region Multiply
	void ARM7TDMI::multiply(const Instruction& instruction)
	{
		auto& rd = m_registers[instruction.rd];
		dword rn = m_registers[instruction.rn];
		dword rm = m_registers[instruction.rm];
		dword rs = m_registers[instruction.rs];


		
		if (instruction.flag(Instruction::Flag::Accumulate))
		{
			i_mla(rd, rn, rm, rs);
		}
		else
		{
			i_mul(rd, rn, rm);
			
			auto& rnz = m_registers[instruction.rn];                           //TODO: explain
			rn = 0;
		}



		if (instruction.flag(Instruction::Flag::Set))
		{
			auto& cpsr = m_registers.get_cpsr();

			cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, rd[31]);          //Result is negative
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     rd == 0);         //Result equals zero
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Carry,    false);           //Meaningless value
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Overflow, false);           //Meaningless value
		}
	}

	void ARM7TDMI::i_mul(Register<dword>& rd, dword rm, dword rs)
	{
		rd = rm * rs;
	}
	void ARM7TDMI::i_mla(Register<dword>& rd, dword rn, dword rm, dword rs)
	{
		rd = rm * rs + rn;
	}
#pragma endregion
#pragma region Multiply Long
	void ARM7TDMI::multiply_long(const Instruction& instruction)
	{
		auto& rdhi = m_registers[instruction.rdhi];
		auto& rdlo = m_registers[instruction.rdlo];
		dword rm = m_registers[instruction.rm];
		dword rs = m_registers[instruction.rs];

		const byte bits = (instruction >> 21) & 0x3;



		using mulFunc = void(ARM7TDMI::*)(Register<dword>&, Register<dword>&, dword, dword);
		const std::unordered_map<unsigned int, mulFunc> mulFuncs
		{
			{ 0b00, &ARM7TDMI::i_umull }, 
			{ 0b01, &ARM7TDMI::i_smull }, 
			{ 0b10, &ARM7TDMI::i_umlal }, 
			{ 0b11, &ARM7TDMI::i_smlal }, 
		};

		std::invoke(mulFuncs.at(bits), this, rdhi, rdlo, rm, rs);

		
		
		if (instruction.flag(Instruction::Flag::Set))
		{
			auto& cpsr = m_registers.get_cpsr();

			cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, rdhi[31]);
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     rdhi == 0 && rdlo == 0);
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Carry,    false);
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Overflow, false);
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
		auto srm = static_cast<long long>(rm);                                 //TODO: cast needed?
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
		auto srm = static_cast<long long>(rm);                                 //TODO: idem as i_smull
		auto srs = static_cast<long long>(rs);
		long long result = srm * srs;

		rdhi = ((result >> 32) & ~0U) + rdhi;
		rdlo = (result & ~0U) + rdlo;
	}
#pragma endregion
#pragma region Single Data Swap
	void ARM7TDMI::single_data_swap(const Instruction& instruction)
	{
		auto& rd = m_registers[instruction.rd];
		auto& rm = m_registers[instruction.rm];
		dword rn = m_registers[instruction.rn];

		i_swp(rd, rm, rn, instruction.b);
	}

	void ARM7TDMI::i_swp(Register<dword>& rd, Register<dword>& rm, size_t address, bool isByte)
	{
		dword value{};
		
		if (isByte)
		{
			value = bus->read<byte>(address);
			bus->write<byte>(address, rm);
		}
		else
		{
			value = bus->read<dword>(address);
			bus->write<dword>(address, rm);
		}
		
		rd = value;
	}
#pragma endregion
#pragma region Branch and Exchange
	void ARM7TDMI::branch_exchange(const Instruction& instruction)
	{
		dword rn = m_registers[instruction.rn];

		i_bx(rn);
		m_pipeline.flush();                                                    //The branch causes a pipeline flush
	}

	void ARM7TDMI::i_bx(dword rn)
	{
		auto& cpsr = m_registers.get_cpsr();
		auto isThumb = get_bit(rn, 0);

		if (isThumb)
		{
			m_registers.pc = rn & ~0x1;
			cpsr = set_bit<dword>(cpsr, PSR::Flag::Thumb, isThumb);            //If bit 0 is set, the processor is in thumb mode
		}
		else
		{
			m_registers.pc = rn & ~0x3;
		}
	}
#pragma endregion
#pragma region Halfword Data Transfer
	void ARM7TDMI::halfword_data_transfer(const Instruction& instruction)
	{
		__debugbreak();                                                        //TODO: Implement

		auto& rd = m_registers[instruction.rd];
		dword rn = m_registers[instruction.rn];
		dword rm = m_registers[instruction.rm];
		
		dword offset{};
		dword address = rn;
		
		const auto s = get_bit(instruction, 6);
		const auto h = get_bit(instruction, 5);
		const auto i = get_bit(instruction, 22);                               //While not explicitly noted in the manual, this bit determines if the offset is a register or an immediate value
		


		if (i) offset = ((instruction >> 4) & 0xF0) | (instruction & 0xF);
		else   offset = rm;


		
		if (instruction.flag(Instruction::Flag::UpDown))  offset *= -1;
		if (instruction.flag(Instruction::Flag::PrePost)) address += offset;


		
		using halfwordFunc = void(ARM7TDMI::*)(Register<dword>&, size_t);
		const std::unordered_map<unsigned int, halfwordFunc> halfwordFuncs     //TODO: check if mappings are correct (strh == 0b00?)
		{
			{ 0b00, &ARM7TDMI::i_strh },
			{ 0b01, &ARM7TDMI::i_ldrh },
			{ 0b10, &ARM7TDMI::i_strh },
			{ 0b11, &ARM7TDMI::i_ldrsb }
		};

		auto mask = (s << 1) | static_cast<unsigned int>(h);
		std::invoke(halfwordFuncs.at(mask), this, rd, address);
		if (mask == 0b01 || mask == 0b11) m_pipeline.flush();

		
		
		if (!instruction.flag(Instruction::Flag::PrePost) || instruction.flag(Instruction::Flag::Writeback)) rn += offset;
	}

	void ARM7TDMI::i_ldrh(Register<dword>& rd, size_t address) //TODO: zero extend
	{
		rd = bus->read<word>(address);
	}
	void ARM7TDMI::i_strh(Register<dword>& rd, size_t address)
	{
		bus->write<word>(address, rd);
	}
	void ARM7TDMI::i_ldrsb(Register<dword>& rd, size_t address) //TODO: sign extend
	{
		rd = bus->read<byte>(address);
	}
	void ARM7TDMI::i_ldrsh(Register<dword>& rd, size_t address) //TODO: sign extend
	{
		rd = bus->read<word>(address);
	}
#pragma endregion
#pragma region Single Data Transfer
	void ARM7TDMI::single_data_transfer(const Instruction& instruction)
	{
		auto& rd = m_registers[instruction.rd];
		auto& rn = m_registers[instruction.rn];

		dword offset{};
		dword address = rn;
		
		
		
		if (instruction.flag(Instruction::Flag::Immediate))                    //Offset is a register if the immediate flag is set
		{
			auto rm = instruction & 0xF;                                       //TODO: add rm to instruction?
			auto carry = m_registers.get_cpsr().flag(PSR::Flag::Carry);
			
			dword imm   =  m_registers[rm];                                    
			auto type   = static_cast<ShiftType>((instruction >> 5) & 0x3);    
			byte amount = (instruction >> 7) & 0x1F;                           
			
			offset = shift_carry(imm, type, amount, carry).first;              //Carry bit is not needed
		}
		else
		{
			offset = instruction.offset & 0xFFF;                               //Offset is an immediate value
		}

		

		if (instruction.flag(Instruction::Flag::UpDown))  offset *= -1;        
		if (instruction.flag(Instruction::Flag::PrePost)) address += offset;   //Address is updated before load or store
		

		
		if (instruction.flag(Instruction::Flag::LoadStore))
		{
			i_ldr(rd, address, instruction.b);

			if (instruction.rd == 15) m_pipeline.flush();
		}
		else
		{
			Register<dword> value = rd;
			if (instruction.rd == 15) value += 4;                              //TODO: correct?

			i_str(value, offset, instruction.b);
		}


		
		if (!instruction.flag(Instruction::Flag::PrePost) || instruction.flag(Instruction::Flag::Writeback)) rn += offset;


		
#if _DEBUG_OUTPUT
		std::cout.copyfmt(std::ios(nullptr));

		if (instruction.flag(Instruction::Flag::LoadStore)) std::cout << "(LDR) ";
		else                                                std::cout << "(STR) ";
		
		std::cout << "Rd[" << std::to_string(instruction.rd) << "] = {" << std::to_string(rd) << "} | ";
		std::cout << "Rn[" << std::to_string(instruction.rn) << "] = {" << std::to_string(rn) << "} | ";
		std::cout << "Offset = {" << std::to_string(offset) << '}' << "\n\n";
#endif
	}

	void ARM7TDMI::i_ldr(Register<dword>& rd, size_t address, bool isByte)
	{
		if (isByte)
		{
			rd = bus->read<byte>(address) & 0xF;
		}
		else
		{
			auto rotate = (address & 0x3) << 3;                                //Check for unaligned address
			
			auto value = bus->read<dword>(address);                            //TODO: rotate lambda function
			rd = (value >> rotate) | (value << (32 - rotate));                 //Rotate the value to the correct position in case of unaligned access
		}
	}
	void ARM7TDMI::i_str(Register<dword>& rd, size_t address, bool isByte)
	{
		if (isByte)
		{
			dword bt = rd & 0xFF;
			dword result = (bt << 24 | bt << 16 | bt << 8 | bt);                //Bits [0-7] are repeated across entire dword
			
			bus->write<dword>(address, result);
		}
		else
		{
			bus->write<dword>(address, rd);
		}
	}
#pragma endregion
#pragma region Undefined
	void ARM7TDMI::undefined(const Instruction& instruction)
	{
		i_und();
	}

	void ARM7TDMI::i_und()
	{
		m_registers.map(OperatingMode::UND);

		auto& cpsr = m_registers.get_cpsr();
		auto& spsr = m_registers.get_spsr();

		spsr = cpsr;
		cpsr = set_bit<dword>(cpsr, PSR::Flag::IRQ, true);

		m_registers[14] = m_registers[15] - 4;                                 //TODO: -4?
		m_registers[15] = 0x04;                                                //Jump to undefined vector
		
		m_pipeline.flush();
	}
#pragma endregion
#pragma region Block Data Transfer
	void ARM7TDMI::block_data_transfer(const Instruction& instruction)
	{
		auto& rn = m_registers[instruction.rn];
		auto list = instruction.registerlist;
		
		dword address = rn;
		dword offset = 4;

		std::vector<std::reference_wrapper<Register<dword>>> registers{};

		

		if (list != 0)
		{
			const auto wordBits = sizeof(word) * 8;
			
			for (unsigned int i = 0; i < wordBits; ++i)
			{
				if (list & (1 << i))                                           //Always iterate the list starting from the lowest register
				{
					registers.emplace_back(m_registers[i]);
				}
			}
		}
		else //TODO: the whole transfer thingy => nba github
		{
			list |= 1 << 15;                                                   //Only r15 will be stored, but the base will be incremented as if every register is stored (+64);
			registers.emplace_back(m_registers[15]);
		}

		

		const auto dwordSize = static_cast<dword>(sizeof(dword));
		const auto registerCount = static_cast<dword>(registers.size());
		
		if (!instruction.flag(Instruction::Flag::UpDown))                      //This can probably be done in a manner that is 10 times more cohesive, but im retarded so deal with it
		{
			rn -= dwordSize;                                                   //4 byte offset so that Rn points to the value before the first register (after writeback)
			address -= registerCount * dwordSize;							   //Adjust the first address to point to the first register
			offset *= -1;													   //In case of decrementing, the initial offset is negative
		}
		if (instruction.flag(Instruction::Flag::PrePost))  address += offset;  
		if (instruction.flag(Instruction::Flag::PSRForce)) m_registers.map(OperatingMode::USR);
		


		if (instruction.flag(Instruction::Flag::LoadStore)) i_ldm(registers, address);
		else                                                i_stm(registers, address);

		

		if (!instruction.flag(Instruction::Flag::PrePost) || instruction.flag(Instruction::Flag::Writeback)) rn += (offset * registerCount);
	}

	void ARM7TDMI::i_ldm(std::vector<std::reference_wrapper<Register<dword>>> registers, size_t address)
	{
		dword offset{};
		
		for (auto& reg : registers)
		{
			reg.get() = bus->read<dword>(address + offset);

			offset += 4;
		}
	}
	void ARM7TDMI::i_stm(std::vector<std::reference_wrapper<Register<dword>>> registers, size_t address)
	{
		dword offset{};

		for (const auto& reg : registers)
		{
			bus->write<dword>(address + offset, reg.get());

			offset += 4;
		}
	}
#pragma endregion
#pragma region Branch
	void ARM7TDMI::branch(const Instruction& instruction)
	{
		dword offset = instruction.offset;                                     
		
		offset <<= 2;														   //Shift left 2 bits
		offset = sign_extend<int, 25>(offset);							       //Sign extend to 32 bits, TODO: int?



#if _DEBUG_OUTPUT
		if (instruction.flag(Instruction::Flag::Link)) std::cout << "(BL) ";
		else                                           std::cout << "(B) ";
		
		std::cout << "[0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << instruction.address << "] => [0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << m_registers.pc + static_cast<int>(offset) << "]\n\n";
#endif

													

		if (instruction.flag(Instruction::Flag::Link)) i_bl(offset);           
		else                                           i_b(offset);            

		m_pipeline.flush();                                                    
	}

	void ARM7TDMI::i_b(dword offset)
	{
		m_registers.pc += offset;
	}
	void ARM7TDMI::i_bl(dword offset)
	{
		m_registers.lr = m_registers.pc - 4;
		m_registers.pc += offset;
	}
#pragma endregion
#pragma region Coprocessor Data Transfer
	void ARM7TDMI::coprocessor_data_transfer(const Instruction& instruction)
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
	void ARM7TDMI::coprocessor_data_operation(const Instruction& instruction)
	{
		__debugbreak();
	}

	void ARM7TDMI::i_cdp()
	{
		
	}
#pragma endregion
#pragma region Coprocessor Register Transfer
	void ARM7TDMI::coprocessor_register_transfer(const Instruction& instruction)
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
	void ARM7TDMI::software_interrupt(const Instruction& instruction)
	{
		i_swi();
	}

	void ARM7TDMI::i_swi()
	{
		m_registers.map(OperatingMode::SVC);

		auto& cpsr = m_registers.get_cpsr();
		auto& spsr = m_registers.get_spsr();

		spsr = cpsr;
		cpsr = set_bit<dword>(cpsr, PSR::Flag::IRQ, true);

		m_registers[14] = m_registers[15] - 4;                                 //TODO: -4?
		m_registers[15] = 0x08;                                                //Jump to interrupt vector

		m_pipeline.flush();
	}
#pragma endregion

//THUMB Instructions
#pragma region Move Shifted Register
	void ARM7TDMI::move_shifted_register()
	{
		
	}

	void ARM7TDMI::i_lsl()
	{
		
	}
	void ARM7TDMI::i_lsr()
	{

	}
	void ARM7TDMI::i_asr()
	{
		
	}
#pragma endregion
}
