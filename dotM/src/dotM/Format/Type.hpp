#pragma once

using bit = bool;
using byte = unsigned char;
using word = unsigned short;
using dword = unsigned int;
using qword = unsigned long long;

//class bit
//{
//public:
//	bit() = default;
//	bit(const bit& value)
//		: m_value{ value }
//	{
//
//	}
//	bit(bit&& value)
//		: m_value{ value }
//	{
//
//	}
//	bit(bool value)
//		: m_value{ value }
//	{
//
//	}
//	bit(int value)
//		: m_value{ value }
//	{
//
//	}
//	~bit() = default;
//
//	bit& operator=(bit value)
//	{
//		m_value = value;
//		return *this;
//	}
//	bit& operator=(bool value)
//	{
//		m_value = value;
//		return *this;
//	}
//	bit& operator=(int value)
//	{
//		m_value = value;
//		return *this;
//	}
//	operator bool() const
//	{
//		return m_value;
//	}
//	operator int() const
//	{
//		return m_value;
//	}
//
//private:
//	bool m_value{};
//};
