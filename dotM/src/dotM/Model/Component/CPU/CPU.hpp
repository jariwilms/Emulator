#pragma once

namespace dot
{
	class CPU
	{
	public:
		~CPU() = default;
		
	protected:
		CPU() = default;
		
		enum class ShiftType : byte
		{
			LSL = 0b00, //Logical Shift Left
			LSR = 0b01, //Logical Shift Right
			ASR = 0b10, //Arithmetic Shift Right
			ROR = 0b11, //Rotation Right
		};
		template<typename T, typename U>
		T shift(T value, ShiftType type, U amount)
		{
			
		}
	};
}
