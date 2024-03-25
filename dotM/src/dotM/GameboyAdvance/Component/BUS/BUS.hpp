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
			validate<T>();

			const auto memory = match_memory(address);
			return memory.expired() ? T{} : memory.lock()->read<T>(address);
		}
		template<typename T>
		void write(size_t address, T value)
		{
			validate<T>();
			
			const auto memory = match_memory(address);
			if (!memory.expired()) memory.lock()->write<T>(address, value);
		}

		template<typename T>
		void transfer(size_t srcAddress, size_t dstAddress, size_t size)
		{
			const auto src = match_memory(srcAddress);
			const auto dst = match_memory(dstAddress);

			if (src.expired() || dst.expired()) return;



			const auto srcMemory = src.lock();
			const auto dstMemory = dst.lock();
			
			std::memcpy(srcMemory->get<T>() + srcAddress, dstMemory->data<T>() + dstAddress, size);
		}

		void connect(std::shared_ptr<MEMORY> memory)
		{
			m_memory.emplace_back(memory);
		}

	protected:
		template<typename T>
		constexpr void validate()
		{
			constexpr auto size = sizeof(T) * 8;
			static_assert(size <= WIDTH, "Type size exceeds bus width!");
		}
		std::weak_ptr<MEMORY> match_memory(size_t address)
		{
			for (const auto& memory : m_memory)
			{
				if (memory->contains(address)) return memory;
			}
			
			return {};
		}
		

		
		std::vector<std::shared_ptr<MEMORY>> m_memory{};
		std::shared_ptr<MEMORY> m_selectedMemory{};
		size_t m_address{};

		const size_t m_width{ WIDTH };
	};
}
