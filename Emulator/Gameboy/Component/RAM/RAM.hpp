#pragma once

#include <bitset>

template<size_t B>
class RAM
{
public:
	RAM() = default;
	~RAM() = default;

private:
	std::bitset<B * 8> m_memory;
};

