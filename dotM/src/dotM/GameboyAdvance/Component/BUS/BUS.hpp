#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/MEM/MEMORY.hpp"

namespace dot::gba
{
	template<size_t WIDTH>
	class BUS
	{
	public:
		BUS() = default;
		virtual ~BUS() = default;

		template<typename T>
		T read(size_t address)
		{
			if (!validate<T>(address)) return {};

			return m_selectedMemory->read<T>(address);
		}
		template<typename T>
		void write(size_t address, T data)
		{
			if (!validate<T>(address)) return;

			m_selectedMemory->write<T>(address, data);
		}

		inline void connect(std::shared_ptr<MEMORY> memory)
		{
			m_memory.emplace_back(memory);
		}

	protected:
		template<typename T>
		inline bool validate(size_t address)                                   //Check if the given type and address are valid
		{
			auto size = sizeof(T) * 8;
			auto memory = map_memory(address);

			if (memory.has_value() && (size <= m_width))
			{
				m_selectedMemory = memory.value();
				
				return true;
			}

			return false;
		}

		std::optional<std::shared_ptr<MEMORY>> map_memory(size_t address) const
		{
			for (const auto& memory : m_memory)
			{
				const auto& [addr0, addrF] = memory->range();
				
				if (memory->in_bounds(address)) return memory;
			}

			return {};
		}

		std::vector<std::shared_ptr<MEMORY>> m_memory{};
		std::shared_ptr<MEMORY> m_selectedMemory{};
		size_t m_address{};

		const size_t m_width{ WIDTH };
	};
}
