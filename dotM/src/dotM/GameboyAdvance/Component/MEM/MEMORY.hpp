#pragma once

#include <stdafx.hpp>

namespace dot::gba
{
	class MEMORY //TODO: template access specification? read/write throws exception if not allowed?
	{
	public:
        enum class Access
        {
            Read,
            Write,
            Read_Write,
        };

        MEMORY(Access access, size_t addr0, size_t addrf)
            : m_memory{ std::make_unique<byte[]>((addrf - addr0) + 1) }, m_range{ addr0, addrf } {}
		virtual ~MEMORY() = default;


		
		template<typename T>
		T read(size_t address) const
		{
			T value{};
			
			const auto offset = map_physical(address);
			std::memcpy(&value, m_memory.get() + offset, sizeof(T));

			return value;
		}
		template<typename T>
		void write(size_t address, T data) const
		{
			const auto offset = map_physical(address);
			std::memcpy(m_memory.get() + offset, &data, sizeof(T));
		}

		const byte* get(size_t address)
		{
			const auto offset = map_physical(address);
			
			return m_memory.get() + offset;
		}
		template<typename T>
		void set(size_t address, const T* data, size_t size) const
		{
			if (size > this->size()) throw std::runtime_error("Size is too large");

			address = map_physical(address);
			std::memcpy(m_memory.get() + address, data, size);
		}

		size_t size() const                                             //Returns the size of the memory in bytes
		{
			const auto& [addr0, addrF] = m_range;
			
			return (addrF - addr0) + 1;
		}
		std::pair<size_t, size_t> range() const
		{
			return m_range;
		}

		bool in_bounds(size_t address) const
		{
			const auto& [addr0, addrF] = m_range;
			
			return address >= addr0 && address <= addrF;
		}

	protected:
		size_t map_physical(size_t address) const
		{
			if (!in_bounds(address)) throw std::invalid_argument("Address out of range!");
			
			return address - m_range.first;
		}
		
	private:
		std::unique_ptr<byte[]> m_memory{};
		std::pair<size_t, size_t> m_range{};
	};
}
