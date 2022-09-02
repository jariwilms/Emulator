#include "ARM7TDMI.hpp"

ARM7TDMI::ARM7TDMI()
	: m_instruction{}
{
	
}
ARM7TDMI::~ARM7TDMI()
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
	
}

void ARM7TDMI::fetch()
{

}
void ARM7TDMI::decode()
{
	
}
void ARM7TDMI::execute()
{

}
