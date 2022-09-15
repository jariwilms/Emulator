#pragma once

#include <stdafx.hpp>

namespace dot::gba
{
	class MEMORY
	{
	public:
		enum class Type
		{
			Read,
			Write,
			Read_Write,
		};

		MEMORY(Type type, size_t addr0, size_t addrF) : m_type{ type }, m_memory{ std::make_unique<byte[]>((addrF - addr0) / 8) }, m_range{ std::make_tuple(addr0, addrF) } {}
		virtual ~MEMORY() = default;



		template<typename T>
		T read(size_t address) const
		{
			if (!isInRange(address)) return {};

			auto index = getIndex(address);

			T value{};
			std::memcpy(&value, m_memory.get() + index, sizeof(T));

			return value;
		}

		template<typename T>
		void write(size_t address, T data)
		{
			if (m_type == Type::Read) return;
			if (!isInRange(address)) return;

			auto index = getIndex(address);

			std::memcpy(m_memory.get() + index, &data, sizeof(T));
		}

		template<typename T>
		void set(size_t address, size_t length, T* data)
		{
			if (length > size()) return; //throw error

			std::memcpy(m_memory.get(), data, length);
		}

		template<typename T = byte>
		size_t size() const
		{
			const auto& [addr0, addrF] = m_range;
			return (addrF - addr0) / sizeof(T) + 1;
		}

		std::tuple<size_t, size_t> range() const
		{
			return m_range;
		}

	protected:
		std::unique_ptr<byte[]> m_memory{};
		std::tuple<size_t, size_t> m_range{};

	private:
		inline bool isInRange(size_t address) const
		{
			const auto& [addr0, addrF] = m_range;
			return address >= addr0 && address <= addrF;
		}
		inline size_t getIndex(size_t address) const
		{
			const auto& [addr0, addrF] = m_range;
			return address - addr0;
		}



		Type m_type{};
	};
}
