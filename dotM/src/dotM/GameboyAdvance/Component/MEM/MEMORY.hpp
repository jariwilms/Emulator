#pragma once

#include <stdafx.hpp>

namespace dot::gba
{
	class MEMORY
	{
	public:
		enum class Access
		{
			Read,
			Write,
			Read_Write,
		};

		MEMORY(Access type, size_t addr0, size_t addrf)
			: m_access{ type }, m_range{ addr0, addrf }, m_memory{ std::make_unique<byte[]>((addrf - addr0) + 1) }
		{
			
		}
		virtual ~MEMORY() = default;



		template<typename T>
		T read(size_t address) const
		{
			if (m_access == Access::Write) return {};

			address = map_physical(address);

			T value{};
			std::memcpy(&value, m_memory.get() + address, sizeof(T));

			return value;
		}
		template<typename T>
		void write(size_t address, T data) const
		{
			if (m_access == Access::Read) return;

			address = map_physical(address);

			std::memcpy(m_memory.get() + address, &data, sizeof(T));
		}

		template<typename T>
		void set(size_t address, const T* data, size_t size) const
		{
			if (size > this->size()) throw std::runtime_error("Size is too large");

			address = map_physical(address);
			std::memcpy(m_memory.get() + address, data, size);
		}

		inline size_t size() const                                             //Returns the size of the memory in bytes
		{
			const auto& [addr0, addrF] = m_range;
			
			return (addrF - addr0) + 1;
		}
		inline std::pair<size_t, size_t> range() const
		{
			return m_range;
		}
		inline Access access() const
		{
			return m_access;
		}

		inline bool in_bounds(size_t address) const
		{
			const auto& [addr0, addrF] = m_range;
			
			return address >= addr0 && address <= addrF;
		}
		
	protected:
		inline size_t map_physical(size_t address) const
		{
			if (!in_bounds(address)) throw std::invalid_argument("Address out of range!");
			
			return address - m_range.first;
		}
		
	private:
		std::unique_ptr<byte[]> m_memory{};
		std::pair<size_t, size_t> m_range{};                                   //Holds the start and end range of addressable memory

		Access m_access{};                                                     //Specifies which operations are allowed
	};
}
