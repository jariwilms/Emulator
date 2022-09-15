#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/MEM/MEMORY.hpp"

namespace dot::gba
{
	class BUS
	{
	public:
		BUS(const size_t bits)
			: m_bits{ bits }
		{

		}
		virtual ~BUS() = default;

		template<typename T>
		T read()
		{
			check<T>();

			return m_target->read<T>(m_address);
		}

		template<typename T>
		void write(T data)
		{
			check<T>();

			m_target->write<T>(data);
		}

		inline void set(dword address)
		{
			m_address = address;
			broadcast();
		}

		inline void attach(std::shared_ptr<MEMORY> memory)
		{
			m_memory.emplace_back(memory);
		}

	private:
		template<typename T>
		inline void check()
		{
			assert(sizeof(T) <= m_bits);
		}

		void broadcast()
		{
			for (const auto& memory : m_memory)
			{
				const auto& [addr0, addrF] = memory->range();
				if (m_address >= addr0 && m_address <= addrF) m_target = memory;
			}
		}

		std::vector<std::shared_ptr<MEMORY>> m_memory{};
		std::shared_ptr<MEMORY> m_target{};
		dword m_address{};

		const size_t m_bits{};
	};
}
