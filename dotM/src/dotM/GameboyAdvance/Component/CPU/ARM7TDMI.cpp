#include <stdafx.hpp>

#include "ARM7TDMI.hpp"

namespace dot::gba
{
    void ARM7TDMI::cycle()
    {
        if (m_cycles)
        {
            --m_cycles;
            return;
        }



		const auto& cpsr = m_registers.get_cpsr();
        bool tFlag = cpsr.flag(PSR::Flag::Thumb);

        tFlag ? m_pipelineTHUMB.shift() : m_pipelineARM.shift();
        
        execute();
        decode();
        fetch();
    }
    void ARM7TDMI::reset()
    {
        m_pipelineARM.flush();                                                 //Remove all instructions present in the pipeline
        m_pipelineTHUMB.flush();

        m_registers.reset();                                                   //All registers are set to 0, some values are adjusted based on reset specification
    }

	void ARM7TDMI::connect(std::shared_ptr<BUS<32>> bus)
	{
        m_BUS = bus;
	}

    void ARM7TDMI::fetch()
    {
        const auto& cpsr = m_registers.get_cpsr();
        dword address{};

		

        const auto tFlag = cpsr.flag(PSR::Flag::Thumb);
		
        if (tFlag)
        {
            Instruction<word> instruction{};
            static dword value{};

            address = m_registers.pc & ~0x1;                                   //0xFFFFFFFE, 2 byte alignment
            const auto fetch = !get_bit(m_registers.pc.get(), 1);              //Only fetch if the "2" bit is not set

            if (fetch)                                                         
            {
                value = m_BUS->read<dword>(address);
                instruction = static_cast<word>(value);
            }
            else
            {
                instruction = static_cast<word>(value >> 16);
            }

            instruction.address = address;

            m_pipelineTHUMB.push(instruction);
            m_registers.pc += 2;
        }
        else
        {
            Instruction<dword> instruction{};

            address = m_registers.pc & ~0x3;                                   //0xFFFFFFFC, 4 byte alignment
            instruction = m_BUS->read<dword>(address);
            instruction.address = address;

            m_pipelineARM.push(instruction);
            m_registers.pc += 4;
        }
    }
    void ARM7TDMI::decode()
    {
		const auto& cpsr = m_registers.get_cpsr();
        const auto tFlag = cpsr.flag(PSR::Flag::Thumb);



        if (tFlag)
        {
            if (m_pipelineTHUMB.load() < 2) return;

            const auto& instruction = m_pipelineTHUMB[1];

			for (int i = architecture::THUMB_FORMAT_COUNT - 1; i >= 0; --i)
			{
				const auto mask = instruction & architecture::THUMB_INSTRUCTION_MASK[i];
				const bool match = (mask == architecture::THUMB_INSTRUCTION_FORMAT[i]);

				if (match)
				{
					m_operationTHUMB = static_cast<OperationTHUMB>(i);

					return;
				}
			}
        }
        else
        {
            if (m_pipelineARM.load() < 2) return;

			const auto& instruction = m_pipelineARM[1];

			for (int i = architecture::ARM_FORMAT_COUNT - 1; i >= 0; --i)
			{
				const auto mask = instruction & architecture::ARM_INSTRUCTION_MASK[i];
				const bool match = (mask == architecture::ARM_INSTRUCTION_FORMAT[i]);

				if (match)
				{
					m_operationARM = static_cast<OperationARM>(i);

					return;
				}
			}
        }

        throw std::runtime_error("Invalid instruction format");                //TODO => undefined?
    }
    void ARM7TDMI::execute()
    {
        const auto& cpsr = m_registers.get_cpsr();
        const auto tFlag = cpsr.flag(PSR::Flag::Thumb);
        


        if (tFlag)
		{
            if (m_pipelineTHUMB.load() < 3) return;

            const auto& instruction = m_pipelineTHUMB[2];



#if _DEBUG
			const std::unordered_map<OperationTHUMB, const char*> operationToInstructionName
			{
			    { OperationTHUMB::MoveShiftedRegister,                "MoveShiftedRegister", },
                { OperationTHUMB::AddSubtract,                        "AddSubtract", },
                { OperationTHUMB::MoveCompareAddSubtractImmediate,    "MoveCompareAddSubtractImmediate", },
                { OperationTHUMB::ALUOperations,                      "ALUOperations,"  },
                { OperationTHUMB::HiRegisterOperationsBranchExchange, "HiRegisterOperationsBranchExchange,"  },
                { OperationTHUMB::LoadPCRelative,                     "LoadPCRelative", },
                { OperationTHUMB::LoadStoreRegisterOffset,            "LoadStoreRegisterOffset,"  },
                { OperationTHUMB::LoadStoreSignExtended,              "LoadStoreSignExtended,"  },
                { OperationTHUMB::LoadStoreImmediateOffset,           "LoadStoreImmediateOffset", },
                { OperationTHUMB::LoadStoreHalfword,                  "LoadStoreHalfword", },
                { OperationTHUMB::LoadStoreSPRelative,                "LoadStoreSPRelative", },
                { OperationTHUMB::LoadAddress,                        "LoadAddress,"  },
                { OperationTHUMB::AddOffsetToStackPointer,            "AddOffsetToStackPointer", },
                { OperationTHUMB::PushPopRegisters,                   "PushPopRegisters", },
                { OperationTHUMB::MultipleLoadStore,                  "MultipleLoadStore", },
                { OperationTHUMB::ConditionalBranch,                  "ConditionalBranch", },
                { OperationTHUMB::SoftwareInterrupt,                  "SoftwareInterrupt", },
                { OperationTHUMB::UnconditionalBranch,                "UnconditionalBranch", },
                { OperationTHUMB::LongBranchWithLink,                 "LongBranchWithLink", },
			};



			std::cout << std::format("[{:#010x}] ", instruction.address) << std::format("{0}\n", operationToInstructionName.at(m_operationTHUMB));
#endif



			using instructionFunc = void (ARM7TDMI::*)(const Instruction<word>&);
			const std::unordered_map<OperationTHUMB, instructionFunc> operationToInstruction
			{
                { OperationTHUMB::MoveShiftedRegister,                &ARM7TDMI::move_shifted_register }, 
				{ OperationTHUMB::AddSubtract,                        &ARM7TDMI::add_subtract }, 
				{ OperationTHUMB::MoveCompareAddSubtractImmediate,    &ARM7TDMI::move_compare_add_subtract_immediate }, 
				{ OperationTHUMB::ALUOperations,                      &ARM7TDMI::alu_operations }, 
				{ OperationTHUMB::HiRegisterOperationsBranchExchange, &ARM7TDMI::hi_register_operations__branch_exchange }, 
				{ OperationTHUMB::LoadPCRelative,                     &ARM7TDMI::pc_relative_load }, 
				{ OperationTHUMB::LoadStoreRegisterOffset,            &ARM7TDMI::load_store_register_offset }, 
				{ OperationTHUMB::LoadStoreSignExtended,              &ARM7TDMI::load_store_sign_extended }, 
				{ OperationTHUMB::LoadStoreImmediateOffset,           &ARM7TDMI::load_store_immediate_offet }, 
				{ OperationTHUMB::LoadStoreHalfword,                  &ARM7TDMI::load_store_halfword }, 
				{ OperationTHUMB::LoadStoreSPRelative,                &ARM7TDMI::sp_relative_load_store }, 
				{ OperationTHUMB::LoadAddress,                        &ARM7TDMI::load_address }, 
				{ OperationTHUMB::AddOffsetToStackPointer,            &ARM7TDMI::add_offset_stack_pointer }, 
				{ OperationTHUMB::PushPopRegisters,                   &ARM7TDMI::push_pop_registers },
				{ OperationTHUMB::MultipleLoadStore,                  &ARM7TDMI::multiple_load_store }, 
				{ OperationTHUMB::ConditionalBranch,                  &ARM7TDMI::conditional_branch }, 
				{ OperationTHUMB::SoftwareInterrupt,                  &ARM7TDMI::software_interrupt }, 
				{ OperationTHUMB::UnconditionalBranch,                &ARM7TDMI::unconditional_branch }, 
				{ OperationTHUMB::LongBranchWithLink,                 &ARM7TDMI::long_branch_link }, 
			};

			std::invoke(operationToInstruction.at(m_operationTHUMB), this, instruction);
        }
        else
        {
            if (m_pipelineARM.load() < 3) return;

			const auto& instruction = m_pipelineARM[2];

            if (!check_condition(instruction, cpsr))
            {
                //std::cout << std::format("[{:#010x}] Not executed\n", instruction.address);

                return;
            }



#if _DEBUG
            const std::unordered_map<OperationARM, const char*> operationToInstructionName
            {
                { OperationARM::DataProcessing,                      "DataProcessing" },
                { OperationARM::Multiply,                            "Multiply" },
                { OperationARM::MultiplyLong,                        "MultiplyLong" },
                { OperationARM::SingleDataSwap,                      "SingleDataSwap" },
                { OperationARM::BranchExchange,                      "BranchExchange" },
                { OperationARM::HalfwordDataTransferRegisterOffset,  "HalfwordDataTransferRegisterOffset" },
                { OperationARM::HalfwordDataTransferImmediateOffset, "HalfwordDataTransferImmediateOffset" },
                { OperationARM::SingleDataTransfer,                  "SingleDataTransfer" },
                { OperationARM::Undefined,                           "Undefined" },
                { OperationARM::BlockDataTransfer,                   "BlockDataTransfer" },
                { OperationARM::Branch,                              "Branch" },
                { OperationARM::CoprocessorDataTransfer,             "CoprocessorDataTransfer" },
                { OperationARM::CoprocessorDataOperation,            "CoprocessorDataOperation" },
                { OperationARM::CoprocessorRegisterTransfer,         "CoprocessorRegisterTransfer" },
                { OperationARM::SoftwareInterrupt,                   "SoftwareInterrupt" }
            };



            std::cout << std::format("[{:#010x}] ", instruction.address) << std::format("{0}\n", operationToInstructionName.at(m_operationARM));
#endif



            using instructionFunc = void (ARM7TDMI::*)(const Instruction<dword>&);
            const std::unordered_map<OperationARM, instructionFunc> operationToInstruction
            {
                { OperationARM::DataProcessing,                      &ARM7TDMI::data_processing               },
                { OperationARM::Multiply,                            &ARM7TDMI::multiply                      },
                { OperationARM::MultiplyLong,                        &ARM7TDMI::multiply_long                 },
                { OperationARM::SingleDataSwap,                      &ARM7TDMI::single_data_swap              },
                { OperationARM::BranchExchange,                      &ARM7TDMI::branch_exchange               },
                { OperationARM::HalfwordDataTransferRegisterOffset,  &ARM7TDMI::halfword_data_transfer        },
                { OperationARM::HalfwordDataTransferImmediateOffset, &ARM7TDMI::halfword_data_transfer        },
                { OperationARM::SingleDataTransfer,                  &ARM7TDMI::single_data_transfer          },
                { OperationARM::Undefined,                           &ARM7TDMI::undefined                     },
                { OperationARM::BlockDataTransfer,                   &ARM7TDMI::block_data_transfer           },
                { OperationARM::Branch,                              &ARM7TDMI::branch                        },
                { OperationARM::CoprocessorDataTransfer,             &ARM7TDMI::coprocessor_data_transfer     },
                { OperationARM::CoprocessorDataOperation,            &ARM7TDMI::coprocessor_data_operation    },
                { OperationARM::CoprocessorRegisterTransfer,         &ARM7TDMI::coprocessor_register_transfer },
                { OperationARM::SoftwareInterrupt,                   &ARM7TDMI::software_interrupt            },
            };

            std::invoke(operationToInstruction.at(m_operationARM), this, instruction);
        }
    }

    void ARM7TDMI::switch_mode(OperatingMode operatingMode)
	{
		auto& cpsr = m_registers.get_cpsr();

		cpsr = set_bits<dword>(cpsr, 0, 5, static_cast<int>(operatingMode));
		m_registers.bank(operatingMode);
	}
	
    void ARM7TDMI::exception()
    {
        //p.62;
    }
    void ARM7TDMI::abort()
    {

    }



    bool ARM7TDMI::check_condition(const Instruction<dword>& instruction, const PSR& cpsr)
	{
		const auto condition = get_bits(instruction, 28, 4);

		const auto N = cpsr.flag(PSR::Flag::Negative);
		const auto Z = cpsr.flag(PSR::Flag::Zero);
		const auto C = cpsr.flag(PSR::Flag::Carry);
		const auto V = cpsr.flag(PSR::Flag::Overflow);

		switch (condition)
		{
			case 0xE: return true;			                               //AL
			case 0xF: return false;			                               //RESERVED

			case 0x0: return  Z;			                               //EQ
			case 0x1: return !Z;			                               //NE
			case 0x2: return  C;			                               //CS
			case 0x3: return !C;			                               //CC
			case 0x4: return  N;			                               //MI
			case 0x5: return !N;			                               //PL
			case 0x6: return  V;			                               //VS
			case 0x7: return !V;			                               //VC
			case 0x8: return  C && !Z;		                               //HI
			case 0x9: return !C || Z;		                               //LS
			case 0xA: return  N == V;		                               //GE
			case 0xB: return  N != V;		                               //LT
			case 0xC: return !Z && (N == V);                               //GT
			case 0xD: return  Z || (N != V);                               //LE

			default:  return false;
		}
	}
    std::pair<dword, bit> ARM7TDMI::shift_carry(dword value, ShiftType type, dword amount, bit carryFlag)
    {
        dword result{};
        bit carryOut{};

        const auto dwordSize = sizeof(dword) * 8;


        
        using shiftFunc = std::function<void()>;
        
        const auto lsl_eq0  = [&]()
        {
            result = value;
            carryOut = carryFlag;
        };
        const auto lsl_lt32 = [&]()
        {
            result = value << amount;
            carryOut = get_bit((value >> (dwordSize - amount)), 0);
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
            result = (value >> amount) | (value << (dwordSize - amount));
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
    void ARM7TDMI::data_processing(const Instruction<dword>& instruction)
    {
        const auto rdIndex  = get_bits(instruction, 12,  4);
        const auto rnIndex  = get_bits(instruction, 16,  4);
        const auto operand2 = get_bits(instruction,  0, 12);
        const auto opcode   = get_bits(instruction, 21,  4);
        const auto iFlag    = get_bit (instruction, 25);
        const auto sFlag    = get_bit (instruction, 20);

        auto& rd = m_registers[rdIndex];
        dword rn = m_registers[rnIndex];
        dword op2{};
		
        Register<dword> result{};

        auto& cpsr = m_registers.get_cpsr();
        auto carry = cpsr.flag(PSR::Flag::Carry);


        
        if (iFlag)                    
        {
            byte imm    = get_bits(operand2, 0, 8);
            byte rotate = get_bits(operand2, 8, 4) * 2;
            
            if (rotate) std::tie(op2, carry) = shift_carry(imm, ShiftType::ROR, rotate, carry);
            else        op2 = imm;
        }
        else                                                                   
        {
            const auto rmIndex = get_bits(instruction, 0, 4);
            const auto shiftOp = get_bits(instruction, 5, 2);
            const auto rFlag   = get_bit (instruction, 4);
            
            dword rm = m_registers[rmIndex];
            dword shiftAmount{};
            const auto shiftType = static_cast<ShiftType>(shiftOp);
            
            if (rnIndex == 15) rn += 4;                                        //"If a register is used to specify the shift amount, the PC will be 12 bytes ahead"
            if (rFlag)
            {
                const auto rsIndex = get_bits(instruction, 8, 4);
                
                dword rs    = m_registers[rsIndex];
                shiftAmount = rs & 0xFF;
            }
            else
            {
                shiftAmount = get_bits(operand2, 7, 5);
            }

            std::tie(op2, carry) = shift_carry(rm, shiftType, shiftAmount, carry);
        }


		
        const auto isTest = get_bit(opcode, 3) && !get_bit(opcode, 2);
        if (isTest && !sFlag) return psr_transfer(instruction);
		
		switch (opcode)
		{
			case 0b0000: rd = rn & op2;             break;                     //AND
			case 0b0001: rd = rn ^ op2;             break;                     //EOR
			case 0b0010: rd = rn - op2;             break;                     //SUB
			case 0b0011: rd = op2 - rn;             break;                     //RSB
			case 0b0100: rd = rn + op2;             break;                     //ADD
			case 0b0101: rd = rn + op2 + carry;     break;                     //ADC
			case 0b0110: rd = rn - op2 - carry - 1; break;                     //SBC, TODO: -1? + carry bit
			case 0b0111: rd = op2 - rn + carry - 1; break;                     //RSC
			case 0b1000: result = rn & op2;         break;                     //TST
			case 0b1001: result = rn ^ op2;         break;                     //TEQ
			case 0b1010: result = rn - op2;         break;                     //CMP
			case 0b1011: result = rn + op2;         break;                     //CMN
			case 0b1100: rd = rn | op2;             break;                     //ORR
			case 0b1101: rd = op2;                  break;                     //MOV
			case 0b1110: rd = rn & ~op2;            break;                     //BIC
			case 0b1111: rd = ~op2;                 break;                     //MVN

			default:                                throw std::invalid_argument("Invalid opcode");
		}

        if (!isTest) result = rd;                                              //Result will be used for flags

        

		enum class FunctionType { Logical, Arithmetic };
		const std::unordered_map<unsigned int, FunctionType> opcodeToFunctionType
		{
			{ 0b0000, FunctionType::Logical },
			{ 0b0001, FunctionType::Logical },
			{ 0b0010, FunctionType::Arithmetic },
			{ 0b0011, FunctionType::Arithmetic },
			{ 0b0100, FunctionType::Arithmetic },
			{ 0b0101, FunctionType::Arithmetic },
			{ 0b0110, FunctionType::Arithmetic },
			{ 0b0111, FunctionType::Arithmetic },
			{ 0b1000, FunctionType::Logical },
			{ 0b1001, FunctionType::Logical },
			{ 0b1010, FunctionType::Arithmetic },
			{ 0b1011, FunctionType::Arithmetic },
			{ 0b1100, FunctionType::Logical },
			{ 0b1101, FunctionType::Logical },
			{ 0b1110, FunctionType::Logical },
			{ 0b1111, FunctionType::Logical },
		};
		
        if (sFlag)
        {
            if (rdIndex == 15)                                                 //Special case if the destination register is the PC
            {
                cpsr = m_registers.get_spsr(cpsr.mode());

                const auto tFlag = cpsr[5];
				
                if (tFlag) m_pipelineTHUMB.flush();
                else       m_pipelineARM.flush();
            }
            else
            {
				const auto functionType = opcodeToFunctionType.at(opcode);

                cpsr.set_flag(PSR::Flag::Negative, result[31]);  
                cpsr.set_flag(PSR::Flag::Zero,     result == 0); 
                cpsr.set_flag(PSR::Flag::Carry,    carry);       

                if (functionType == FunctionType::Arithmetic)                          
                {
                    const auto signRn     = get_bit(rn,     31);               //Check for overflow by comparing the sign of the operands
                    const auto signOp2    = get_bit(op2,    31);               //If the signs are the same, but the result has a different sign, overflow has occurred
                    const auto signResult = get_bit(result, 31);               //(i.e. the sign of the result is different from the signs of the operands)

                    const auto v = ((signRn == signOp2) && (signResult != signRn));
                    cpsr.set_flag(PSR::Flag::Overflow, v);
                }
            }
        }
    }
    void ARM7TDMI::psr_transfer(const Instruction<dword>& instruction)
    {
        const auto pFlag = get_bit(instruction, 22);
        const auto mFlag = get_bit(instruction, 21);
        
        auto& cpsr = m_registers.get_cpsr();
        auto& spsr = m_registers.get_spsr(cpsr.mode());


		
        if (mFlag)
        {
            const auto rmIndex = get_bits(instruction,  0, 4);
            const auto iFlag   = get_bit (instruction, 25);

            dword mask{};
            dword result{};

            if (iFlag)
            {
                const auto operand2 = get_bits(instruction, 0, 12);

                dword imm    = get_bits(operand2, 0,  8);
                dword rotate = get_bits(operand2, 8,  4) * 2;

                result = shift(imm, ShiftType::ROR, rotate);
            }
            else
            {
                result = m_registers[rmIndex];
            }

            
            
            for (int i = 0; i < 4; ++i)                                        //cpsr_fsxc or something
            {
                if (get_bit(instruction, i + 16))
                {
                    mask |= 0xFF;
                    mask <<= 4;
                }
            }


			
            if (pFlag)
            {
				spsr = (spsr & ~mask) | (result & mask);
            }
            else
            {
                if (cpsr.mode() == OperatingMode::USR) mask   &= 0xFF000000;
                if (mask & 0xFF)                       result |= 0x00000010;   //idk

                __debugbreak();
                //TODO: switch mode?
				//cpsr = (cpsr & ~mask) | (result & mask); //remove this
            }
        }
        else
        {
            const auto rdIndex = get_bits(instruction, 12, 4);
			
            auto& rd = m_registers[rdIndex];

            if (pFlag) rd = spsr;
            else       rd = cpsr;
        }
    }
    void ARM7TDMI::multiply(const Instruction<dword>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 16, 4);
        const auto rnIndex = get_bits(instruction, 12, 4);
        const auto rmIndex = get_bits(instruction,  0, 4);
        const auto rsIndex = get_bits(instruction,  8, 4);
        const auto aFlag   = get_bit (instruction, 21);
		const auto sFlag   = get_bit (instruction, 20);

        auto& rd = m_registers[rdIndex];
        auto& rn = m_registers[rnIndex];
        dword rm = m_registers[rmIndex];
        dword rs = m_registers[rsIndex];


        
        if (aFlag)
        {
            rd = rm * rs + rn;
        }
        else
        {
            rd = rm * rs;
            rn = 0;                                                            //"Rn is ignored and should be set to zero for compatibility"
        }



        if (sFlag)
        {
            auto& cpsr = m_registers.get_cpsr();
			
            cpsr.set_flag(PSR::Flag::Negative, rd[31]);          
            cpsr.set_flag(PSR::Flag::Zero,     rd == 0);         
            cpsr.set_flag(PSR::Flag::Carry,    false);
            cpsr.set_flag(PSR::Flag::Overflow, false);
        }

        if (rdIndex == 15) m_pipelineARM.flush();
    }	
    void ARM7TDMI::multiply_long(const Instruction<dword>& instruction)
    {
        const auto rdhiIndex = get_bits(instruction, 16, 4);
		const auto rdloIndex = get_bits(instruction, 12, 4);
		const auto rmIndex   = get_bits(instruction,  0, 4);
		const auto rsIndex   = get_bits(instruction,  8, 4);
		const auto uFlag     = get_bit (instruction, 22);
		const auto aFlag     = get_bit (instruction, 21);
		const auto sFlag     = get_bit (instruction, 20);
		
        auto& rdhi = m_registers[rdhiIndex];
        auto& rdlo = m_registers[rdloIndex];
        dword rm   = m_registers[rmIndex];
        dword rs   = m_registers[rsIndex];
		
        long long result{};



        if (uFlag) result = static_cast<long long>(static_cast<int>(rm)) * static_cast<long long>(static_cast<int>(rs)); //Intermediate cast is required
        else       result = static_cast<qword>(rm) * static_cast<qword>(rs);
        
        if (aFlag) result += ((static_cast<long long>(rdhi) << 32) | static_cast<long long>(rdlo));

		rdhi = static_cast<dword>(result >> 32);
		rdlo = static_cast<dword>(result);
		

        
        if (sFlag)
        {
            auto& cpsr = m_registers.get_cpsr();
            const auto z = (rdhi == 0) && (rdlo == 0);

            cpsr.set_flag(PSR::Flag::Negative, rdhi[31]);
            cpsr.set_flag(PSR::Flag::Zero,     z);
            cpsr.set_flag(PSR::Flag::Carry,    false);
            cpsr.set_flag(PSR::Flag::Overflow, false);
        }

        if (rdhiIndex == 15 || rdloIndex == 15) m_pipelineARM.flush();
    }
    void ARM7TDMI::single_data_swap(const Instruction<dword>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 12, 4);
        const auto rnIndex = get_bits(instruction, 16, 4);
        const auto rmIndex = get_bits(instruction,  0, 4);
		const auto bFlag   = get_bit (instruction, 22);

        auto& rd = m_registers[rdIndex];
        dword rn = m_registers[rnIndex];
        dword rm = m_registers[rmIndex];

		dword value{};
        dword address = rn;
		
		
		
        if (bFlag)
        {
            value = m_BUS->read<byte>(address);
            m_BUS->write<byte>(address, rm);
        }
        else
        {
            value = m_BUS->read<dword>(address);                                 //TODO: readwordrotate?
            m_BUS->write<dword>(address, rm);
        }

        rd = value;


		
        if (rdIndex == 15) m_pipelineARM.flush();
    }
    void ARM7TDMI::branch_exchange(const Instruction<dword>& instruction)
    {
		const auto rnIndex = get_bits(instruction, 0, 4);

        dword rn = m_registers[rnIndex];

		

		const auto tFlag = get_bit(rn, 0);

        if (tFlag)
        {
			m_registers.pc = rn & ~0x1;
            m_pipelineTHUMB.flush();
        }
        else
        {
            m_registers.pc = rn & ~0x3;
            m_pipelineARM.flush();
        }
		

		
		auto& cpsr = m_registers.get_cpsr();
        cpsr.set_flag(PSR::Flag::Thumb, tFlag);
    }
    void ARM7TDMI::halfword_data_transfer(const Instruction<dword>& instruction)
    {
		const auto rdIndex = get_bits(instruction, 12, 4);
		const auto rnIndex = get_bits(instruction, 16, 4);
		const auto rmIndex = get_bits(instruction,  0, 4);
		const auto pFlag   = get_bit (instruction, 24);
		const auto uFlag   = get_bit (instruction, 23);
        const auto iFlag   = get_bit (instruction, 22);
		const auto wFlag   = get_bit (instruction, 21);
		const auto lFlag   = get_bit (instruction, 20);
		const auto sFlag   = get_bit (instruction,  6);
		const auto hFlag   = get_bit (instruction,  5);

        auto& rd = m_registers[rdIndex];
        auto& rn = m_registers[rnIndex];
        dword rm = m_registers[rmIndex];
        
        dword offset{};
        dword address = rn;
        
        

        if (iFlag)  offset   = ((instruction >> 4) & 0xF0) | (instruction & 0xF);
        else        offset   = rm;
        if (!uFlag) offset  *= -1;
        if (pFlag)  address += offset;



        const auto mask = (sFlag << 1) | static_cast<int>(hFlag);

        switch (mask) //TODO: double check
        {
            case 0b00: return single_data_swap(instruction);
			case 0b01:
            {
                if (lFlag) rd = m_BUS->read<word>(address);                      //TODO: readhalfrotate
                else       m_BUS->write<word>(address, rd);

                break;
            }
			case 0b10: 
            {
                if (lFlag) rd = m_BUS->read<byte>(address);
                else       m_BUS->write<byte>(address, rd);

                break;
            }
			case 0b11: 
            {
                if (lFlag) rd = sign_extend<int, 15>(m_BUS->read<word>(address));
                else       m_BUS->write<word>(address, rd);

                break;
            }
        }

		
		
        if (!pFlag && wFlag)        rn += offset;
        if (lFlag && rdIndex == 15) m_pipelineARM.flush();
    }
    void ARM7TDMI::single_data_transfer(const Instruction<dword>& instruction)
    {
		const auto rdIndex  = get_bits(instruction, 12, 4);
		const auto rnIndex  = get_bits(instruction, 16, 4);
		const auto offset12 = get_bits(instruction,  0, 12);
		const auto iFlag    = get_bit (instruction, 25);
		const auto pFlag    = get_bit (instruction, 24);
		const auto uFlag    = get_bit (instruction, 23);
		const auto bFlag    = get_bit (instruction, 22);
		const auto wFlag    = get_bit (instruction, 21);
		const auto lFlag    = get_bit (instruction, 20);

        auto& rd = m_registers[rdIndex];
        auto& rn = m_registers[rnIndex];

        dword address = rn;
        dword offset{};
		


        if (iFlag)
        {
            const auto rmIndex = get_bits(instruction, 0, 4);
            const auto shift   = get_bits(instruction, 5, 2);
			const auto amount  = get_bits(instruction, 7, 5);

			const auto& cpsr = m_registers.get_cpsr();
            auto carry       = cpsr.flag(PSR::Flag::Carry);
            
            dword rm       = m_registers[rmIndex];
            auto shiftType = static_cast<ShiftType>(shift);

            offset = shift_carry(rm, shiftType, amount, carry).first;
        }
        else
        {
            offset = offset12;
        }

        if (!uFlag) offset  *= -1;
        if (pFlag)  address += offset;
        

        
        if (lFlag)
        {
			if (bFlag)
			{
                rd = m_BUS->read<byte>(address);
			}
			else
			{
				auto rotate = (address & 0x3) << 3;                            //Check for unaligned address
				auto value = m_BUS->read<dword>(address);                        //TODO: rotate lambda function
				
				rd = (value >> rotate) | (value << (32 - rotate));             //Rotate the value to the correct position in case of unaligned access
			}

            if (rdIndex == 15) m_pipelineARM.flush();
        }
        else
        {
            Register<dword> value = rd;
            if (rdIndex == 15) value += 4;

			if (bFlag)
			{
				byte bt = rd & 0xFF;
				dword result = (bt << 24 | bt << 16 | bt << 8 | bt);           //Bits [0-7] are repeated across entire dword

				m_BUS->write<dword>(address, result);
			}
			else
			{
				m_BUS->write<dword>(address, rd);
			}
        }


        
        if (!pFlag && wFlag) rn += offset12;
        if (lFlag && rdIndex == 15) m_pipelineARM.flush();
    }
    void ARM7TDMI::undefined(const Instruction<dword>& instruction)
    {
        auto& cpsr = m_registers.get_cpsr();
        auto& spsr = m_registers.get_spsr(OperatingMode::UND);
		
        spsr = cpsr;
		
		switch_mode(OperatingMode::UND);
        cpsr.set_flag(PSR::Flag::IRQ, true);

        m_registers.lr = m_registers.pc - 4;
		m_registers.pc = 0x04;
		
        m_pipelineARM.flush();
    }
    void ARM7TDMI::block_data_transfer(const Instruction<dword>& instruction)
    {
		const auto rnIndex      = get_bits(instruction, 16, 4);
		const auto registerList = get_bits(instruction, 0, 16);
		const auto pFlag        = get_bit(instruction, 24);
		const auto uFlag        = get_bit(instruction, 23);
		const auto sFlag        = get_bit(instruction, 22);
		const auto wFlag        = get_bit(instruction, 21);
		const auto lFlag        = get_bit(instruction, 20);

        auto& rn = m_registers[rnIndex];
        auto list = registerList;

        dword address = rn;
        dword offset = 4;

        std::vector<std::reference_wrapper<Register<dword>>> registers{};

        

        if (list != 0)
        {
            const auto wordBits = sizeof(word) * 8;
            
            for (unsigned int i = 0; i < wordBits; ++i)
            {
                if (list & (1ul << i))                                           
                {
                    registers.emplace_back(m_registers[i]);
                }
            }
        }
        else //TODO: the whole transfer thingy => nba github
        {
            __debugbreak();

            list |= 1ul << 15;                                                   //Only r15 will be stored, but the base will be incremented as if every register is stored (+64);
            registers.emplace_back(m_registers[15]);
        }

        

        const auto dwordSize = static_cast<dword>(sizeof(dword));
        const auto registerCount = static_cast<dword>(registers.size());
        
        if (!uFlag)
        {
            rn -= dwordSize;                                                   //4 byte offset so that Rn points to the value before the first register (after writeback)
            address -= registerCount * dwordSize;							   //Adjust the first address to point to the first register
            offset *= -1;													   //In case of decrementing, the initial offset is negative
        }
        if (pFlag) address += offset;  
        if (sFlag) __debugbreak();//m_registers.bank(m_registers.get_cpsr().mode());
        


        if (lFlag) //TODO: cleanup
        {
			dword offsetasd{};

			for (auto& reg : registers)
			{
				reg.get() = m_BUS->read<dword>(static_cast<qword>(address) + offsetasd);

				offsetasd += 4;
			}
        }
        else
        {
			dword offsetdef{};

			for (const auto& reg : registers)
			{
				m_BUS->write<dword>(static_cast<qword>(address) + offsetdef, reg.get());

				offsetdef += 4;
			}
        }

        

        if (!pFlag && wFlag) rn += (offset * registerCount);
        //TODO: nba github adds some code at the end
    }
    void ARM7TDMI::branch(const Instruction<dword>& instruction)
    {
		const auto offset24 = get_bits(instruction,  0, 24);
		const auto lFlag    = get_bit (instruction, 24);

        dword offset = offset24;
        

		
        offset <<= 2;
        offset = sign_extend<int, 25>(offset);                                 //Offset: [0:23] << 2, so bit 25 is the sign bit

                                                    

        if (lFlag)
        {
			m_registers.lr = m_registers.pc - 4;
			m_registers.pc += offset;
        }
        else
        {
            m_registers.pc += offset;
        }

        m_pipelineARM.flush();
    }
    void ARM7TDMI::coprocessor_data_transfer(const Instruction<dword>& instruction)
    {
        __debugbreak();
    }
    void ARM7TDMI::coprocessor_data_operation(const Instruction<dword>& instruction)
    {
        __debugbreak();
    }
    void ARM7TDMI::coprocessor_register_transfer(const Instruction<dword>& instruction)
    {
        __debugbreak();
    }
    void ARM7TDMI::software_interrupt(const Instruction<dword>& instruction)
    {
		auto& cpsr = m_registers.get_cpsr();
		auto& spsr = m_registers.get_spsr(OperatingMode::SVC);

		spsr = cpsr;
		
		switch_mode(OperatingMode::SVC);
		cpsr.set_flag(PSR::Flag::IRQ, true);

		m_registers.lr = m_registers.pc - 4;
		m_registers.pc = 0x08;

		m_pipelineARM.flush();
    }


	
    //THUMB Instructions
    void ARM7TDMI::move_shifted_register(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  0, 3);
        const auto rsIndex = get_bits(instruction,  3, 3);
        const auto offset5 = get_bits(instruction,  6, 5);
        const auto shiftOp = get_bits(instruction, 11, 2);

        auto& rd = m_registers[rdIndex];
        dword rs = m_registers[rsIndex];

        auto& cpsr = m_registers.get_cpsr();
        auto carry = cpsr.flag(PSR::Flag::Carry);
		


        const auto shiftType = static_cast<ShiftType>(shiftOp);
        std::tie(rd, carry) = shift_carry(rs, shiftType, offset5, carry);



		cpsr.set_flag(PSR::Flag::Negative, rd[31]);
        cpsr.set_flag(PSR::Flag::Zero,     rd == 0);
        cpsr.set_flag(PSR::Flag::Carry,    carry);
    }
    void ARM7TDMI::add_subtract(const Instruction<word>& instruction)
    {
        const auto rdIndex  = get_bits(instruction,  0, 3);
        const auto rsIndex  = get_bits(instruction,  3, 3);
        const auto operand2 = get_bits(instruction,  6, 3);
        const auto opcode   = get_bit (instruction,  9);
        const auto iFlag    = get_bit (instruction, 10);

        auto& rd = m_registers[rdIndex];
        dword rs = m_registers[rsIndex];
        dword op2{};



        if (iFlag) op2 = operand2;
        else       op2 = m_registers[operand2];

        if (opcode) rd = rs - op2;
        else        rd = rs + op2;
    }
    void ARM7TDMI::move_compare_add_subtract_immediate(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  8, 3);
        const auto offset8 = get_bits(instruction,  0, 8);
        const auto opcode  = get_bits(instruction, 11, 2);

        auto& rd   = m_registers[rdIndex];
        Register<dword> result{};

        auto& cpsr = m_registers.get_cpsr();



        switch (opcode)
        {
            case 0b00: rd = offset8;          break;                           //MOV
			case 0b01: result = rd - offset8; break;                           //CMP, may not write to rd
            case 0b10: rd += offset8;         break;                           //ADD
            case 0b11: rd -= offset8;         break;                           //SUB

			default:                          throw std::runtime_error("Invalid opcode");
        }
		
		if (opcode != 0b01) result = rd;                                       //Set the cpsr flags based on the result

		

		cpsr.set_flag(PSR::Flag::Negative, result[31]);
		cpsr.set_flag(PSR::Flag::Zero,     result == 0);
    }
    void ARM7TDMI::alu_operations(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 0, 3);
        const auto rsIndex = get_bits(instruction, 3, 3);
        const auto opcode  = get_bits(instruction, 6, 4);

        auto& rd = m_registers[rdIndex];
        dword rs = m_registers[rsIndex];
        Register<dword> result{};

        auto& cpsr = m_registers.get_cpsr();
		auto carry = cpsr.flag(PSR::Flag::Carry);
		
		
		
        switch (opcode)
        {
			case 0b0000: rd = rd & rs;                            break;       //AND
			case 0b0001: rd = rd ^ rs;                            break;       //EOR
			case 0b0010: std::tie(rd, carry) = shift_carry(rd, CPU::ShiftType::LSL, rs, carry); break; //LSL
			case 0b0011: std::tie(rd, carry) = shift_carry(rd, CPU::ShiftType::LSR, rs, carry); break; //LSR
            case 0b0100: std::tie(rd, carry) = shift_carry(rd, CPU::ShiftType::ASR, rs, carry); break; //ASR
			case 0b0101: rd = rd + rs + carry;                    break;       //ADC
			case 0b0110: rd = rd - rs - !carry;                   break;       //SBC
			case 0b0111: std::tie(rd, carry) = shift_carry(rd, CPU::ShiftType::ROR, rs, carry); break; //ROR
			case 0b1000: result = rd & rs;                        break;       //TST
			case 0b1001: rd = rs * -1;                            break;       //NEG
			case 0b1010: result = rd -rs;                         break;       //CMP
			case 0b1011: result = rd + rs;                        break;       //CMN
			case 0b1100: rd = rd | rs;                            break;       //ORR
			case 0b1101: rd = rd * rs;                            break;       //MUL
			case 0b1110: rd = rd & (~rs);                         break;       //BIC
			case 0b1111: rd = ~rs;                                break;       //MVN

			default:                                              throw std::runtime_error("Invalid opcode");
        }

		if (opcode != 0b1000 && opcode != 0b1010 && opcode != 0b1011) result = rd; //Set the cpsr flags based on the result



		cpsr.set_flag(PSR::Flag::Negative, result[31]);
		cpsr.set_flag(PSR::Flag::Zero,     result == 0);
		cpsr.set_flag(PSR::Flag::Carry,    carry);

        //TODO: ...?
		//const auto signRn = get_bit(rn, 31);               //Check for overflow by comparing the sign of the operands
		//const auto signOp2 = get_bit(op2, 31);               //If the signs are the same, but the result has a different sign, overflow has occurred
		//const auto signResult = get_bit(result, 31);               //(i.e. the sign of the result is different from the signs of the operands)

		//const auto v = ((signRn == signOp2) && (signResult != signRn));
		//cpsr = set_bit<dword>(cpsr, PSR::Flag::Overflow, v);
    }
    void ARM7TDMI::hi_register_operations__branch_exchange(const Instruction<word>& instruction)
    {
		const auto rdIndex = get_bits(instruction, 0, 3);
		const auto rsIndex = get_bits(instruction, 3, 3);
		const auto opcode  = get_bits(instruction, 8, 2);
        const auto h1Flag  = get_bit (instruction, 7);                         //Registers 0-7
        const auto h2Flag  = get_bit (instruction, 6);                         //Registers 8-15

		auto& rd = m_registers[rdIndex | (h1Flag << 3)];                       //The H flag selects the high or low registers
		dword rs = m_registers[rsIndex | (h2Flag << 3)];                       //If it is set, the register index is offset by 8
        Register<dword> result{};

        auto& cpsr = m_registers.get_cpsr();


		
        if (rsIndex == 15)
        {
            rs &= ~0x1;                                                        //2 byte alignment
            rs += 4;                                                           //TODO: correct? prefetch adjusted operand
        }

		

        switch (opcode)
        {
            case 0b00: rd = rd + rs; break;                                    //ADD
            case 0b01:                                                         //CMP
            {
                result = rd - rs; 

				cpsr.set_flag(PSR::Flag::Negative, result[31]);
				cpsr.set_flag(PSR::Flag::Zero,     result == 0);

                break;
            }
            case 0b10: rd = rs; break;                                         //MOV
			case 0b11:                                                         //BX
			{
                const auto tFlag = get_bit(rs, 0);

                if (tFlag)
                {
                    m_registers.pc = rs & ~0x1;
                    m_pipelineTHUMB.flush();
                }
                else
                {
                    m_registers.pc = rs & ~0x3;
                    m_pipelineARM.flush();
                }

				cpsr.set_flag(PSR::Flag::Thumb, tFlag);

				break;
			}
        }
    }
    void ARM7TDMI::pc_relative_load(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 8, 3);
        const auto word8   = get_bits(instruction, 0, 8);

        auto& rd = m_registers[rdIndex];
        dword address{};



        address  = m_registers.pc & ~0x2;                                      //Bit 1 of the PC is forced to 0 to ensure it is word aligned
        address += word8 << 2;

        rd = m_BUS->read<word>(address);
    }
    void ARM7TDMI::load_store_register_offset(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  0, 3);
        const auto rbIndex = get_bits(instruction,  3, 3);
        const auto roIndex = get_bits(instruction,  6, 3);
        const auto bFlag   = get_bit (instruction, 10);
        const auto lFlag   = get_bit (instruction, 11);

        auto& rd = m_registers[rdIndex];
        dword rb = m_registers[rbIndex];
        dword ro = m_registers[roIndex];
        dword address = rb + ro;

        

        const auto opcode = (lFlag << 1) | static_cast<int>(bFlag);

        switch (opcode)
        {
            case 0b00: m_BUS->write<word>(address, rd); break;
            case 0b01: m_BUS->write<byte>(address, rd); break;
            case 0b10: rd = m_BUS->read<word>(address); break; //TODO: halfword rotate as usual
            case 0b11: rd = m_BUS->read<byte>(address); break;
        }
    }
    void ARM7TDMI::load_store_sign_extended(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  0, 3);
        const auto rbIndex = get_bits(instruction,  3, 3);
        const auto roIndex = get_bits(instruction,  6, 3);
        const auto hFlag   = get_bit (instruction, 11);
        const auto sFlag   = get_bit (instruction, 11);

        auto& rd = m_registers[rdIndex];
        dword rb = m_registers[rbIndex];
        dword ro = m_registers[roIndex];
        dword address = rb + ro;



        const auto opcode = (hFlag << 1) | static_cast<int>(sFlag);

        switch (opcode)
        {
            case 0b00: m_BUS->write<word>(address, rd);                       break;
            case 0b01: rd = m_BUS->read<word>(address);                       break; //Bits [16:31] are automatically set to 0
            case 0b10: rd = sign_extend<int, 7> (m_BUS->read<byte>(address)); break;
            case 0b11: rd = sign_extend<int, 15>(m_BUS->read<word>(address)); break;
        }
    }
    void ARM7TDMI::load_store_immediate_offet(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  0, 3);
        const auto rbIndex = get_bits(instruction,  3, 3);
        const auto offset5 = get_bits(instruction,  6, 5);
        const auto bFlag   = get_bit (instruction, 12);
        const auto lFlag   = get_bit (instruction, 12);

        auto& rd = m_registers[rdIndex];
        dword rb = m_registers[rbIndex];
        dword address{};



        if (bFlag) address = rb + offset5;
        else       address = rb + (offset5 << 2);

        const auto opcode = (bFlag << 1) | static_cast<int>(lFlag);

        switch (opcode)
        {
            case 0b00: m_BUS->write<word>(address, rd); break;
            case 0b01: m_BUS->write<byte>(address, rd); break;
            case 0b10: rd = m_BUS->read<word>(address); break;                   //TODO: READ WORD ROTATE 
            case 0b11: rd = m_BUS->read<byte>(address); break;
        }
    }
    void ARM7TDMI::load_store_halfword(const Instruction<word>& instruction)
    {
		const auto rdIndex = get_bits(instruction,  0, 3);
		const auto rbIndex = get_bits(instruction,  3, 3);
		const auto offset5 = get_bits(instruction,  6, 5);
		const auto lFlag   = get_bit (instruction, 11);

        auto& rd = m_registers[rdIndex];
        dword rb = m_registers[rbIndex];
        dword address = rb + (offset5 << 1);



        if (lFlag) rd = m_BUS->read<word>(address);                              //TODO: rotate
        else       m_BUS->write<word>(address, rd);
    }
    void ARM7TDMI::sp_relative_load_store(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 8, 3);
        const auto word8   = get_bits(instruction, 0, 8);
        const auto lFlag   = get_bit (instruction, 11);

        auto& rd = m_registers[rdIndex];
        dword address = m_registers.sr + (word8 << 2);



        if (lFlag) rd = m_BUS->read<word>(address);                              //TODO: rotate
        else       m_BUS->write<word>(address, rd);
    }
    void ARM7TDMI::load_address(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 8, 3);
        const auto word8   = get_bits(instruction, 0, 8);
        const auto sFlag   = get_bit(instruction, 11);

        auto& rd = m_registers[rdIndex];
        dword offset = word8 << 2;



        if (sFlag) rd = m_registers.sr + offset;
        else       rd = (m_registers.pc & ~0x2) + offset;
    }
    void ARM7TDMI::add_offset_stack_pointer(const Instruction<word>& instruction)
    {
        const auto sword7 = get_bits(instruction, 0, 7);
        const auto sFlag  = get_bit (instruction, 7);

        dword offset = sword7 << 2;



        if (sFlag) m_registers.sr += offset;
        else       m_registers.sr -= offset;
    }
    void ARM7TDMI::push_pop_registers(const Instruction<word>& instruction)
    {
        const auto registerList = get_bits(instruction,  0, 8);
        const auto lFlag        = get_bit (instruction, 11);
        const auto rFlag        = get_bit (instruction,  8);

        dword address = m_registers.sr.get();


		
        if (registerList == 0 && !rFlag)                                       //Empty list special case
        {
            if (lFlag)
            {
                m_registers.pc = m_BUS->read<word>(address);
                m_registers.sr += 0x40;
                
                m_pipelineTHUMB.flush();
            }
            else
            {
                m_registers.sr -= 0x40;
				m_BUS->write<word>(address, m_registers.pc.get());
            }

            return;
        }

		
		
        const auto listSize = 8;

        if (lFlag)
        {
            for (int i = 0; i < listSize; ++i)
            {
                if (registerList & (0x1 << i))
                {
					m_registers[i] = m_BUS->read<word>(address);
					address += 4;
                }
            }

            if (rFlag)
            {
                m_registers.pc = m_BUS->read<word>(address) & ~0x1;
                address += 4;
				
                m_pipelineTHUMB.flush();
            }
			
            m_registers.sr = address;
        }
        else
        {
			for (int i = 0; i < listSize; ++i)
			{
                if (registerList & (0x1 << i)) address -= 4;
			}

            if (rFlag) address -= 4;
            m_registers.sr = address;

			
			
			for (int i = 0; i < listSize; ++i)
			{
                if (registerList & (0x1 << i))
                {
                    m_BUS->write<word>(address, m_registers[i]);
                    address += 4;
                }
			}

			if (rFlag) m_BUS->write<word>(address, m_registers.lr.get());
        }
    }
    void ARM7TDMI::multiple_load_store(const Instruction<word>& instruction)
    {
		const auto rbIndex      = get_bits(instruction,  8, 3);
		const auto registerList = get_bits(instruction,  0, 8);
        const auto lFlag        = get_bit (instruction, 11);

        auto& rb = m_registers[rbIndex];
        dword address = rb;
		


        if (registerList == 0)
        {
            if (lFlag)
            {
				m_registers.pc = m_BUS->read<word>(address);
				
                m_pipelineTHUMB.flush();
            }
            else
            {
                m_BUS->write<word>(address, m_registers.pc.get());
            }

            rb += 0x40;
			
            return;
        }


		
        const auto listSize = 8;
		
        if (lFlag)
        {
            for (int i = 0; i < listSize; ++i)
            {
                if (registerList & (0x1 << i))
                {
					m_registers[i] = m_BUS->read<word>(address);
                    address += 4;
                }
            }

            if (~registerList & (0x1 << rbIndex)) rb = address;                //TODO: const auto writeback = get_bit etc. then if (...) rb = address;
        }
        else 
        {
            auto count = 0;
            auto first = 0;

            for (int i = listSize - 1; i >= 0; --i)
            {
				if (registerList & (0x1 << i))
				{
					++count;
					first = i;
				}
            }



            dword offset = address + (count * 4);

            m_BUS->write<word>(address, m_registers[first]);
            rb = offset;
            address += 4;
			
            for (int i = first + 1; i <= listSize - 1; ++i)                    //Im going to be honest, I have no idea what's happening anymore at this point
            {
                if (registerList & (0x1 << i))                                 //Also big TODO: refactor this instead of flat out copying nba github
                {
					m_BUS->write<word>(address, m_registers[i]);
					address += 4;
                }
            }
        }
    }
    void ARM7TDMI::conditional_branch(const Instruction<word>& instruction)
    {
        const auto soffset8 = get_bits(instruction, 0, 8);
        const auto cond     = get_bits(instruction, 8, 4);

        dword offset = sign_extend<int, 7>(soffset8) << 1;
        const auto& cpsr = m_registers.get_cpsr();



		if (!check_condition(instruction << 20, cpsr)) return;                 //The condition in this instruction is at bits [8:11] => shift to bit 31

        m_registers.pc += offset;
        m_pipelineTHUMB.flush();
    }
    void ARM7TDMI::software_interrupt(const Instruction<word>& instruction)
    {
        auto& cpsr = m_registers.get_cpsr();
        auto& spsr = m_registers.get_spsr(OperatingMode::SVC);

        spsr = cpsr;
		
        switch_mode(OperatingMode::SVC);
        cpsr.set_flag(PSR::Flag::IRQ, true);
        cpsr.set_flag(PSR::Flag::Thumb, false);

		m_registers.lr = m_registers.pc - 2;
		m_registers.pc = 0x8;

		m_pipelineTHUMB.flush();
    }
    void ARM7TDMI::unconditional_branch(const Instruction<word>& instruction)
    {
        const auto offset11 = get_bits(instruction, 0, 11);

        dword offset = sign_extend<int, 10>(offset11) << 1;



        m_registers.pc += offset;
        m_pipelineTHUMB.flush();
    }
    void ARM7TDMI::long_branch_link(const Instruction<word>& instruction)
    {
        const auto offset11 = get_bits(instruction,  0, 11);
        const auto hFlag    = get_bit (instruction, 11);

        dword offset{};



        if (hFlag)
        {
            const auto nextInstruction = m_registers.pc - 2;
            offset = (offset11 << 1);
			
            m_registers.pc = (m_registers.lr + offset) & ~0x1;
            m_registers.lr = nextInstruction | 0x1;
        }
        else
        {
            offset = sign_extend<int, 22>(offset11 << 12);                     //Offset MSB is at index 10, ls 12 => 22;
            m_registers.lr = m_registers.pc + offset;
        }
    }
}
