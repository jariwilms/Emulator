#pragma once

#include <bitset>

using byte = unsigned char;
using word = unsigned short;
using dword = unsigned int;
using qword = unsigned long long;

using Instruction = std::bitset<32>;

class CPU
{
public:
	virtual ~CPU() = default;

	virtual void cycle() = 0;
	virtual void reset() = 0;

protected:
	CPU() = default;
	
	virtual void fetch() = 0;
	virtual void decode() = 0;
	virtual void execute() = 0;
};
