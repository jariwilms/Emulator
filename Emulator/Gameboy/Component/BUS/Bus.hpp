#pragma once

#include <vector>
#include <bitset>

#include "Gameboy/Component/BUS/Connector.hpp"

template<size_t W>
class Bus
{
public:
	Bus() = default;
	~Bus() = default;

	std::bitset<W> read(size_t address)
	{
		return m_memory;
	}
	void write(std::bitset<W> memory, size_t address)
	{
		//for device in devices => device.write(memory, address);
	}

	void attach(const Connector& connector)
	{
		m_connections.push_back(connector);
	}

private:
	std::bitset<W> m_memory;
	std::vector<std::reference_wrapper<const Connector>> m_connections;
};

