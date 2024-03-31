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
		struct Range
		{
		public:
			Range() = default;
			explicit Range(size_t min, size_t max)
				: min{ min }, max{ max } {}

			size_t min{};
			size_t max{};
		};

        MEMORY(Access access, size_t addr0, size_t addrF)
			: m_access{ access }, m_memory{ std::make_unique<byte[]>((addrF - addr0) + 1) }, m_range{ addr0, addrF } {}
		virtual ~MEMORY() = default;

		template<typename T>
		T read(size_t address) const
		{
			//if (m_access == Access::Write) throw std::runtime_error("Cannot read from write-only memory.");
			return *data<T>(address);
		}
		template<typename T>
		void write(size_t address, const T& value) const
		{
			//if (m_access == Access::Read) throw std::runtime_error("Cannot write to read only memory!");
			*data<T>(address) = value;
		}

		template<typename T = byte>
		T* data(size_t address) const
		{
			const auto offset = translate(address);
			return std::launder<T>(reinterpret_cast<T*>(m_memory.get() + offset));
		}

		Access access() const
		{
			return m_access;
		}
		size_t size() const                                                    //Returns the size of the memory in bytes
		{
			return (m_range.max - m_range.min) + 1;
		}
		Range range() const
		{
			return m_range;
		}
		bool contains(size_t address) const                                    //TODO: rename/remove
		{
			return address >= m_range.min  && address <= m_range.max;
		}

	//protected:
		size_t translate(size_t address) const
		{
			if (!contains(address)) throw std::invalid_argument("Address out of range!");
			
			return address - m_range.min;
		}
		
	//private:
		const Access m_access{};

		std::unique_ptr<byte[]> m_memory{};
		const Range m_range{};
	};
}
