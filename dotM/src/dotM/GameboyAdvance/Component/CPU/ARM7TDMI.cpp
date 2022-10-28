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
        const auto& cpsr = m_registers.get_cpsr();
        const auto tFlag = cpsr.flag(PSR::Flag::Thumb);
        
        dword address{};


		
        if (tFlag)
        {
            Instruction<word> instruction{};
            static dword value{};

            address = m_registers.pc & (~0u - 1);                              //0xFFFFFFFE, 2 byte alignment
            auto fetch = !get_bit(m_registers.pc.get(), 1);                    //Only fetch if the "2" bit is not set

            if (fetch)                                                         
            {
                value = bus->read<dword>(address);
                instruction = static_cast<word>(value);
            }
            else
            {
                instruction = static_cast<word>(value >> 16);
            }

            m_pipeline2.push(instruction);
            m_instructionAddresses.front() = address;

            m_registers.pc += 2;
        }
        else
        {
            Instruction<dword> instruction{};

            address = m_registers.pc & (~0u - 3);                              //0xFFFFFFFC, 4 byte alignment
            instruction = bus->read<dword>(address);

            m_pipeline.push(instruction);
            m_instructionAddresses.front() = address;
            
            m_registers.pc += 4;
        }
    }
    void ARM7TDMI::decode()
    {
        const auto& instruction = m_pipeline[1];                                     


		
        for (int i = architecture::ARM_FORMAT_COUNT - 1; i >= 0; --i)          //Match functions in reverse order: specific to general format
        {																	   
            auto mask = instruction & architecture::ARM_INSTRUCTION_MASK[i];   
            bool match = (mask == architecture::ARM_INSTRUCTION_FORMAT[i]);    

            if (match)                                                         //If a match is found, we know which operation to perform
            {
                m_operation = static_cast<Operation>(i);
                
                return;
            }
        }

        throw std::runtime_error("Invalid instruction format");                //If no match is found, the instruction is invalid TODO => undefined?
    }
    void ARM7TDMI::execute()
    {
        const auto& instruction = m_pipeline[2];                               
        const auto& cpsr = m_registers.get_cpsr();
        
        const auto condition_passed = [](const Instruction<dword>& instruction, const PSR& psr) -> bool
        {
            const auto condition = get_bits(instruction, 28, 4);

            const auto N = psr.flag(PSR::Flag::Negative);
            const auto Z = psr.flag(PSR::Flag::Zero);
            const auto C = psr.flag(PSR::Flag::Carry);
            const auto V = psr.flag(PSR::Flag::Overflow);
            
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
        }(instruction, cpsr); //cursed?
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

        std::cout << "[0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << "address" << "] " << "0x" << std::setw(8) << std::setfill('0') << instruction << " \"" << operationToStringMap[m_operation] << "\"" << std::endl;
#endif



        using instructionFormat = void (ARM7TDMI::*)(const Instruction<dword>&);
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

        const auto dwordSize = sizeof(dword) * 8;                              //Total amount of bits in value


        
        using shiftFunc = std::function<void()>;                               //TODO: if destination register is r15, increment its value by 4
        
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
#pragma region Data Processing
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

        auto& cpsr = m_registers.get_cpsr();
        auto carry = cpsr.flag(PSR::Flag::Carry);


        
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
            const auto shift   = get_bits(instruction, 5, 2);
            const auto rFlag   = get_bit(instruction, 4);
            
            dword rm = m_registers[rmIndex];     
            dword shiftAmount{};
            auto shiftType = static_cast<ShiftType>(shift);
            
            if (rnIndex == 15) rn += 4;                                        //Adjust for prefetch if Rn is PC
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

        
        
        const auto& [dpFunc, type] = functionByType.at(opcode);                  
        Register<dword> result{};                                              //Store the result in a placeholder value => not all operations may write back to the destination register
        
        const auto isTest = get_bit(opcode, 3) && !get_bit(opcode, 2);         //TST, TEQ, CMP and CMN instructions have bit pattern [0b10xx]

        if (isTest)                                                            
        {
            if (sFlag) std::invoke(dpFunc, this, result, rn, op2);               
            else       return psr_transfer(instruction);                       //S flag set => instruction is a PSR transfer
        }
        else                                                                   
        {
            std::invoke(dpFunc, this, rd, rn, op2);                              
            result = rd;                                                       
        }
        
        

        if (sFlag)
        {
            if (rdIndex == 15)                                                 //Special case if the destination register is the PC
            {
                cpsr = m_registers.get_spsr();
            }
            else
            {
                cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, result[31]);  
                cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     result == 0); 
                cpsr = set_bit<dword>(cpsr, PSR::Flag::Carry,    carry);       

                if (type == FunctionType::Arithmetic)                          
                {
                    const auto signRn     = get_bit(rn, 31);                   //Check for overflow by comparing the sign of the operands
                    const auto signOp2    = get_bit(op2, 31);                  //If the signs are the same, but the result has a different sign, overflow has occurred
                    const auto signResult = get_bit(result, 31);               //(i.e. the sign of the result is different from the signs of the operands)

                    const auto v = ((signRn == signOp2) && (signResult != signRn));
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

        std::cout << '(' << opcodeToFunctionMap[opcode] << ") ";
        std::cout << "Rd[" << std::to_string(rdIndex) << "] = {" << std::to_string(rd) << "} | ";
        std::cout << "Rn[" << std::to_string(rnIndex) << "] = {" << std::to_string(rn) << "} | ";
        std::cout << "Operand2 = {" << op2 << "} | ";
        std::cout << "Result = {" << result << '}' << '\n';
        
        std::bitset<4> bitset = m_registers.get_cpsr() >> 28;
        std::cout << "Set: " << std::boolalpha << sFlag << ", CPSR = {" << bitset << "}\n\n";
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
    void ARM7TDMI::psr_transfer(const Instruction<dword>& instruction)
    {
        const auto mFlag = get_bit(instruction, 21);
        const auto pFlag = get_bit(instruction, 22);
        
        auto& cpsr = m_registers.get_cpsr();
        auto& spsr = m_registers.get_spsr();


		
        if (mFlag)
        {
            const auto rmIndex = get_bits(instruction,  0, 4);
            const auto iFlag   = get_bit (instruction, 25);

            dword mask{};
            dword result{};

            if (iFlag)
            {
                dword op2    = get_bits(instruction, 0, 12);
                dword imm    = get_bits(op2,         0,  8);
                dword rotate = get_bits(op2,         8,  4) * 2;

                result = shift(imm, ShiftType::ROR, rotate);
            }
            else
            {
                result = m_registers[rmIndex];
            }

            
            
            for (int i = 16; i < 20; ++i)                                      //cpsr_fsxc or something => NBA github
            {
                if (get_bit(instruction, i))
                {
                    mask |= 0xFF;
                    mask <<= 4;
                }
            }


			
            if (pFlag)
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
            const auto rdIndex = get_bits(instruction, 12, 4);
			
            auto& rd = m_registers[rdIndex];

            if (pFlag) i_mrs(rd, spsr);
            else          i_mrs(rd, cpsr);
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
            i_mla(rd, rn, rm, rs);
        }
        else
        {
            i_mul(rd, rm, rs);
            
            rn = 0;
        }



        if (sFlag)
        {
            auto& cpsr = m_registers.get_cpsr();

            cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, rd[31]);          
            cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     rd == 0);         
            cpsr = set_bit<dword>(cpsr, PSR::Flag::Carry,    false);           
            cpsr = set_bit<dword>(cpsr, PSR::Flag::Overflow, false);           
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
    void ARM7TDMI::multiply_long(const Instruction<dword>& instruction)
    {
        const auto rdhiIndex = get_bits(instruction, 16, 4);
		const auto rdloIndex = get_bits(instruction, 12, 4);
		const auto rmIndex   = get_bits(instruction,  0, 4);
		const auto rsIndex   = get_bits(instruction,  8, 4);
		const auto aFlag     = get_bit(instruction, 21);
		const auto sFlag     = get_bit(instruction, 20);
		
        auto& rdhi = m_registers[rdhiIndex];
        auto& rdlo = m_registers[rdloIndex];
        dword rm   = m_registers[rmIndex];
        dword rs   = m_registers[rsIndex];



        using mullFunc = void(ARM7TDMI::*)(Register<dword>&, Register<dword>&, dword, dword);
        const std::unordered_map<unsigned int, mullFunc> mulFuncs
        {
            { 0b00, &ARM7TDMI::i_umull }, 
            { 0b01, &ARM7TDMI::i_smull }, 
            { 0b10, &ARM7TDMI::i_umlal }, 
            { 0b11, &ARM7TDMI::i_smlal }, 
        };

        const auto pattern = (aFlag << 1) | static_cast<unsigned int>(sFlag);
        std::invoke(mulFuncs.at(pattern), this, rdhi, rdlo, rm, rs);

        
        
        if (sFlag)
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
        auto result = static_cast<qword>(rm) * static_cast<qword>(rs);

        rdhi = get_bits(result, 32, 32);
        rdlo = static_cast<dword>(result);
    }
    void ARM7TDMI::i_smull(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
    {
		auto result = static_cast<long long>(rm) * static_cast<long long>(rs);
		
        rdhi = get_bits(result, 32, 32);
        rdlo = static_cast<dword>(result);
    }
    void ARM7TDMI::i_umlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
    {
		auto value = static_cast<qword>(rdhi) << 32 | rdlo;
        auto result = static_cast<qword>(rm) * static_cast<qword>(rs) + value;

        rdhi = get_bits(result, 32, 32);
        rdlo = static_cast<dword>(result);
    }
    void ARM7TDMI::i_smlal(Register<dword>& rdhi, Register<dword>& rdlo, dword rm, dword rs)
    {
		auto value = static_cast<long long>(rdhi) << 32 | rdlo;
		auto result = static_cast<long long>(rm) * static_cast<long long>(rs) + value;

		rdhi = get_bits(result, 32, 32);
		rdlo = static_cast<dword>(result);
    }
#pragma endregion
#pragma region Single Data Swap
    void ARM7TDMI::single_data_swap(const Instruction<dword>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 12, 4);
        const auto rnIndex = get_bits(instruction, 16, 4);
        const auto rmIndex = get_bits(instruction,  0, 4);
		const auto bFlag   = get_bit (instruction, 22);

        auto& rd = m_registers[rdIndex];
        dword rn = m_registers[rnIndex];
        auto& rm = m_registers[rmIndex];


		
        i_swp(rd, rm, rn, bFlag);
    }

    void ARM7TDMI::i_swp(Register<dword>& rd, Register<dword>& rm, dword rn, bool bFlag)
    {
        Register<dword> value{};
        dword address = rn;
		
        i_ldr(value, address, bFlag);
        i_str(rm, address, bFlag);
		rd = value;
    }
#pragma endregion
#pragma region Branch and Exchange
    void ARM7TDMI::branch_exchange(const Instruction<dword>& instruction)
    {
		const auto rnIndex = get_bits(instruction, 0, 4);

        dword rn = m_registers[rnIndex];


		
        i_bx(rn);
        m_pipeline.flush();                                                    //The branch causes a pipeline flush
    }

    void ARM7TDMI::i_bx(dword rn)
    {
        auto& cpsr = m_registers.get_cpsr();
        const auto tFlag = get_bit(rn, 0);


		
        if (tFlag)
        {
            m_registers.pc = rn & (~0u - 1);
            cpsr = set_bit<dword>(cpsr, PSR::Flag::Thumb, tFlag);              //If bit 0 is set, the processor is in thumb mode
        }
        else
        {
            m_registers.pc = rn & (~0u - 3);
        }
    }
#pragma endregion
#pragma region Halfword Data Transfer
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
        dword rn = m_registers[rnIndex];
        dword rm = m_registers[rmIndex];
        
        dword offset{};
        dword address = rn;
        
        

        if (iFlag) offset   = ((instruction >> 4) & 0xF0) | (instruction & 0xF); //TODO
        else       offset   =     rm;
        if (uFlag) offset  *=     -1;
        if (pFlag) address += offset;


        
        using halfwordFunc = void(ARM7TDMI::*)(Register<dword>&, dword);
        const std::unordered_map<unsigned int, halfwordFunc> halfwordFuncs     //TODO: check if mappings are correct (strh == 0b00?)
        {
            { 0b00, &ARM7TDMI::i_strh },
            { 0b01, &ARM7TDMI::i_ldrh },
            { 0b10, &ARM7TDMI::i_strh },
            { 0b11, &ARM7TDMI::i_ldrsb }
        };

        auto mask = (sFlag << 1) | static_cast<unsigned int>(hFlag);
        std::invoke(halfwordFuncs.at(mask), this, rd, address);
		
        const auto flush = get_bit(mask, 0);
        if (flush) m_pipeline.flush();

        
        
        if (!pFlag && wFlag) rn += offset;
    }

    void ARM7TDMI::i_ldrh(Register<dword>& rd, dword address) //TODO: zero extend
    {
        rd = bus->read<word>(address);
    }
    void ARM7TDMI::i_strh(Register<dword>& rd, dword address)
    {
        bus->write<word>(address, rd);
    }
    void ARM7TDMI::i_ldrsb(Register<dword>& rd, dword address) //TODO: sign extend
    {
        rd = bus->read<byte>(address);
    }
    void ARM7TDMI::i_ldrsh(Register<dword>& rd, dword address) //TODO: sign extend
    {
        rd = bus->read<word>(address);
    }
#pragma endregion
#pragma region Single Data Transfer
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
        word offset{}; //for lack of a better name
		


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

        

        if (uFlag) offset *= -1;        
        if (pFlag) address += offset;
        

        
        if (lFlag)
        {
            i_ldr(rd, address, bFlag);

            if (rdIndex == 15) m_pipeline.flush();
        }
        else
        {
            Register<dword> value = rd;
            if (rdIndex == 15) value += 4;

            i_str(value, offset12, bFlag);
        }


        
        if (!pFlag && wFlag) rn += offset12;


        
#if _DEBUG_OUTPUT
        std::cout.copyfmt(std::ios(nullptr));

        if (lFlag) std::cout << "(LDR) ";
        else                                                std::cout << "(STR) ";
        
        std::cout << "Rd[" << std::to_string(rdIndex) << "] = {" << std::to_string(rd) << "} | ";
        std::cout << "Rn[" << std::to_string(rnIndex) << "] = {" << std::to_string(rn) << "} | ";
        std::cout << "Offset = {" << std::to_string(offset12) << '}' << "\n\n";
#endif
    }

    void ARM7TDMI::i_ldr(Register<dword>& rd, dword address, bool isByte)
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
    void ARM7TDMI::i_str(Register<dword>& rd, dword address, bool isByte)
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
    void ARM7TDMI::undefined(const Instruction<dword>& instruction)
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
    void ARM7TDMI::block_data_transfer(const Instruction<dword>& instruction)
    {
		const auto rnIndex = get_bits(instruction, 16, 4);
		const auto registerList = get_bits(instruction, 0, 16);
		const auto pFlag = get_bit(instruction, 24);
		const auto uFlag = get_bit(instruction, 23);
		const auto sFlag = get_bit(instruction, 22);
		const auto wFlag = get_bit(instruction, 21);
		const auto lFlag = get_bit(instruction, 20);

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
                if (list & (1 << i))                                           
                {
                    registers.emplace_back(m_registers[i]);
                }
            }
        }
        else //TODO: the whole transfer thingy => nba github
        {
            __debugbreak();

            list |= 1 << 15;                                                   //Only r15 will be stored, but the base will be incremented as if every register is stored (+64);
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
        if (sFlag) m_registers.map(OperatingMode::USR);
        


        if (lFlag) i_ldm(registers, address);
        else       i_stm(registers, address);

        

        if (!pFlag && wFlag) rn += (offset * registerCount);
    }

    void ARM7TDMI::i_ldm(std::vector<std::reference_wrapper<Register<dword>>> registers, dword address)
    {
        dword offset{};
        
        for (auto& reg : registers)
        {
            reg.get() = bus->read<dword>(address + offset);

            offset += 4;
        }
    }
    void ARM7TDMI::i_stm(std::vector<std::reference_wrapper<Register<dword>>> registers, dword address)
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
    void ARM7TDMI::branch(const Instruction<dword>& instruction)
    {
		const auto offset = get_bits(instruction,  0, 24);
		const auto lFlag  = get_bit (instruction, 24);

        dword offsetR = offset;
        
        offsetR <<= 2;
        offsetR = sign_extend<int, 25>(offsetR);



#if _DEBUG_OUTPUT
        if (lFlag) std::cout << "(BL) ";
        else       std::cout << "(B) ";
        
        std::cout << "[0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << "address" << "] => [0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << m_registers.pc + static_cast<int>(offsetR) << "]\n\n";
#endif

                                                    

        if (lFlag) i_bl(offset);           
        else       i_b(offset);            

        m_pipeline.flush();                                                    
    }

    void ARM7TDMI::i_b(dword offset)
    {
        m_registers.pc += offset;
    }
    void ARM7TDMI::i_bl(dword offset)
    {
        m_registers.lr  = m_registers.pc - 4;
        m_registers.pc += offset;
    }
#pragma endregion
#pragma region Coprocessor Data Transfer
    void ARM7TDMI::coprocessor_data_transfer(const Instruction<dword>& instruction)
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
    void ARM7TDMI::coprocessor_data_operation(const Instruction<dword>& instruction)
    {
        __debugbreak();
    }

    void ARM7TDMI::i_cdp()
    {
        
    }
#pragma endregion
#pragma region Coprocessor Register Transfer
    void ARM7TDMI::coprocessor_register_transfer(const Instruction<dword>& instruction)
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
    void ARM7TDMI::software_interrupt(const Instruction<dword>& instruction)
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
    void ARM7TDMI::move_shifted_register(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  0, 3);
        const auto rsIndex = get_bits(instruction,  3, 3);
        const auto offset5 = get_bits(instruction,  6, 5);
        const auto opcode  = get_bits(instruction, 11, 2);

        auto& rd = m_registers[rdIndex];
        dword rs = m_registers[rsIndex];

        auto& cpsr = m_registers.get_cpsr();
        auto carry = cpsr.flag(PSR::Flag::Carry);



        const auto shiftType = static_cast<ShiftType>(opcode);
        std::tie(rd, carry) = shift_carry(rs, shiftType, offset5, carry);



		cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, rd[31]);
        cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     rd == 0);
        cpsr = set_bit<dword>(cpsr, PSR::Flag::Carry,    carry);
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



        if (opcode) i_add(rd, rs, op2);
        else        i_sub(rd, rs, op2);
    }



    void ARM7TDMI::move_compare_add_subtract_immediate(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction,  8, 3);
        const auto offset8 = get_bits(instruction,  0, 8);
        const auto opcode  = get_bits(instruction, 11, 2);

        auto& rd   = m_registers[rdIndex];
        auto& cpsr = m_registers.get_cpsr();



        using mcasiFunc = void(ARM7TDMI::*)(Register<dword>&, dword, dword);
        const std::unordered_map<unsigned int, mcasiFunc> opcodeToFunction
        {
            { 0b00, &ARM7TDMI::i_mov }, 
            { 0b01, &ARM7TDMI::i_cmp },
            { 0b10, &ARM7TDMI::i_add },
            { 0b11, &ARM7TDMI::i_sub },
        };

        std::invoke(opcodeToFunction.at(opcode), this, rd, 0, offset8);



		cpsr = set_bit<dword>(cpsr, PSR::Flag::Negative, 0);
		cpsr = set_bit<dword>(cpsr, PSR::Flag::Zero,     rd == 0);
    }



    void ARM7TDMI::alu_operations(const Instruction<word>& instruction)
    {
        const auto rdIndex = get_bits(instruction, 0, 3);
        const auto rsIndex = get_bits(instruction, 3, 3);
        const auto opcode  = get_bits(instruction, 6, 4);

        auto& rd = m_registers[rdIndex];
        dword rs = m_registers[rsIndex];



        using aluFunc = void(ARM7TDMI::*)(Register<dword>&, dword);
        const std::unordered_map<unsigned int, aluFunc> opcodeToFunction
        {
            { 0b0000, &ARM7TDMI::it_and }, 
            { 0b0001, &ARM7TDMI::it_eor }, 
            { 0b0010, &ARM7TDMI::it_lsl }, 
            { 0b0011, &ARM7TDMI::it_lsr }, 
            { 0b0100, &ARM7TDMI::it_asr }, 
            { 0b0101, &ARM7TDMI::it_adc }, 
            { 0b0110, &ARM7TDMI::it_sbc }, 
            { 0b0111, &ARM7TDMI::it_ror }, 
            { 0b1000, &ARM7TDMI::it_tst }, 
            { 0b1001, &ARM7TDMI::it_neg }, 
            { 0b1010, &ARM7TDMI::it_cmp }, 
            { 0b1011, &ARM7TDMI::it_cmn }, 
            { 0b1100, &ARM7TDMI::it_orr }, 
            { 0b1101, &ARM7TDMI::it_mul }, 
            { 0b1110, &ARM7TDMI::it_bic }, 
            { 0b1111, &ARM7TDMI::it_mvn }, 
        };


		
		const auto func = opcodeToFunction.at(opcode);
		Register<dword> result{};                                              //Store the result in a placeholder value => not all operations may write back to the destination register

		const auto isTest = (opcode > 0b0111 && opcode < 0b1100);

		if (isTest)
		{
            std::invoke(func, this, result, rs);
		}
		else
		{
			std::invoke(func, this, rd, rs);
			result = rd;
		}
    }

    void ARM7TDMI::it_and(Register<dword>& rd, dword rs)
    {
		rd = rd & rs;
    }
    void ARM7TDMI::it_eor(Register<dword>& rd, dword rs)
    {
		rd = rd ^ rs;
    }
    void ARM7TDMI::it_lsl(Register<dword>& rd, dword rs)
    {
		rd = rd << rs;
    }
    void ARM7TDMI::it_lsr(Register<dword>& rd, dword rs)
    {
        rd = rd >> rs;
    }
    void ARM7TDMI::it_asr(Register<dword>& rd, dword rs)
    {
        rs = shift(rd, CPU::ShiftType::ASR, rs);
    }
    void ARM7TDMI::it_adc(Register<dword>& rd, dword rs)
    {
        const auto& cpsr = m_registers.get_cpsr();
        const auto carry = cpsr.flag(PSR::Flag::Carry);

		rd = rd + rs + carry;
    }
    void ARM7TDMI::it_sbc(Register<dword>& rd, dword rs)
    {
		const auto& cpsr = m_registers.get_cpsr();
		const auto carry = cpsr.flag(PSR::Flag::Carry);

		rd = rd - rs - !carry;
    }
    void ARM7TDMI::it_ror(Register<dword>& rd, dword rs)
    {
		rs = shift(rd, CPU::ShiftType::ROR, rs);
    }
    void ARM7TDMI::it_tst(Register<dword>& rd, dword rs)
    {
        rd = rd & rs;
    }
    void ARM7TDMI::it_neg(Register<dword>& rd, dword rs)
    {
        rs = -rs;
    }
    void ARM7TDMI::it_cmp(Register<dword>& rd, dword rs)
    {
        rd = rd - rs;
    }
    void ARM7TDMI::it_cmn(Register<dword>& rd, dword rs)
    {
        rd = rd + rs;
    }
    void ARM7TDMI::it_orr(Register<dword>& rd, dword rs)
    {
		rd = rd | rs;
    }
    void ARM7TDMI::it_mul(Register<dword>& rd, dword rs)
    {
        rd = rd * rs;
    }
    void ARM7TDMI::it_bic(Register<dword>& rd, dword rs)
    {
		rd = rd & ~rs;
    }
    void ARM7TDMI::it_mvn(Register<dword>& rd, dword rs)
    {
        rd = ~rs;
    }
}
