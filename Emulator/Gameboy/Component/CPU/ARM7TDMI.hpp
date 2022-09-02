#pragma once

#include "Gameboy/Component/CPU/Base/CPU.hpp"

class ARM7TDMI : public CPU
{
public:
	ARM7TDMI();
	~ARM7TDMI() override;
	


	void cycle() override;
	void reset() override;

protected:
	void fetch() override;
	void decode() override;
	void execute() override;

private:
	enum class State
	{
		ARM,
		THUMB,
	};
	enum class Mode
	{
		USR, //User
		FIQ, //Fast Interrupt
		IRQ, //Interrupt
		SVC, //Supervisor
		ABT, //Abort
		SYS, //System
		UND, //Undefined
	};
	enum class Instruction
	{
		ADC,   //Add with Carry
		ADD,   //Add
		AND,   //Bitwise AND
		B,     //Branch
		BL,    //Branch and Link
		BIC,   //Bitwise Clear
		BX,    //Branch and Exchange
		CMN,   //Compare Negative
		CMP,   //Compare
		EOR,   //Bitwise Exclusive OR
		LDM,   //Load Multiple
		LDR,   //Load Register
		LDRB,  //Load Register Byte
		LDRH,  //Load Register Halfword
		LDRSB, //Load Register Signed Byte
		LDRSH, //Load Register Signed Halfword
		MLA,   //Multiply Accumulate
		MOV,   //Move
		MRS,   //Move PSR to Register
		MSR,   //Move Register to PSR
		MUL,   //Multiply
		MVN,   //Move Negative
		ORR,   //Bitwise Inclusive OR
		RSB,   //Reverse Subtract
		RSC,   //Reverse Subtract with Carry
		SBC,   //Subtract with Carry
		SMLAL, //Signed Multiply-Accumulate Long
		SMULL, //Signed Multiply Long
		STM,   //Store Multiple
		STR,   //Store Register
		STRB,  //Store Register Byte
		STRH,  //Store Register Halfword
		SUB,   //Subtract
		SWI,   //Software Interrupt
		SWP,   //Swap
		SWPB,  //Swap Byte
		TEQ,   //Test Equivalence
		TST,   //Test
		UMLAL, //Unsigned Multiply-Accumulate Long
		UMULL, //Unsigned Multiply Long
	};

	Instruction m_instruction;
};
