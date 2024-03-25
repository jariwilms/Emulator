#pragma once

namespace dot
{
	class CPU
	{
	public:
		~CPU() = default;
		
	protected:
		CPU() = default;
		
		enum class ShiftType
		{
			LSL = 0b00,                                                        //Logical Shift Left
			LSR = 0b01,                                                        //Logical Shift Right
			ASR = 0b10,                                                        //Arithmetic Shift Right
			ROR = 0b11,                                                        //Rotate Right
		};
		template<typename T, typename U>
		static constexpr T shift(T value, ShiftType type, U bits)
		{
			static_assert(std::is_integral<T>::value, "T must be an integral type");
			static_assert(std::is_integral<U>::value, "U must be an integral type");


			
			const auto size = sizeof(T) * 8;

			switch (type)
			{
				case ShiftType::LSL: return value << bits;
				case ShiftType::LSR: return value >> bits;
				case ShiftType::ASR: return get_bit(value, size - 1) ? ~(~value >> bits) : value >> bits;
				case ShiftType::ROR: return (value >> bits) | (value << (size - bits));

				default:             throw std::runtime_error("Invalid ShiftType");
			}
		}
		template <typename T, size_t MSB>
		static constexpr T sign_extend(const T& value)
		{
			struct extend{ T v : MSB; };                                       //We let the compiler do our job
			
			return extend{ value }.v;
		}
		template<typename T>
		static constexpr T reverse(const T& value)
		{
			static_assert(std::is_integral<T>::value, "T must be an integral type");

			T result{};
			const int size = sizeof(T) * 8;
			
			for (auto i = 0; i < size; ++i)
			{
				result |= get_bit(value, i) << (size - i - 1);
			}
			
			return result;
		}

		template<typename T>
		struct Register
		{
		public:
			Register() = default;
			explicit Register(const T& value)
			{
				m_value = value;
			}
			virtual ~Register() = default;

			operator T() const
			{
				return m_value;
			}
			operator T& ()
			{
				return m_value;
			}
			
			Register<T>& operator=(const T& other)
			{
				m_value = other;

				return *this;
			}
			Register<T>& operator=(const Register<T>& other)
			{
				m_value = other.m_value;

				return *this;
			}
			
			bool operator[](unsigned int index) const
			{
				return get_bit(m_value, index);
			}

		protected:
			T m_value{};
		};
		template<typename T, size_t STAGES>
		struct Pipeline
		{
		public:
			Pipeline() = default;
			~Pipeline() = default;

			void push(const T& value, const T& address)
			{
				m_elements.front() = value;
				m_addresses.front() = address;

				if (m_load == 0) ++m_load;
			}
			void shift()
			{
				std::shift_right(m_elements.begin(), m_elements.end(), 1);
				std::shift_right(m_addresses.begin(), m_addresses.end(), 1);

				m_elements.front() = 0;
				m_addresses.front() = 0;

				if (m_load < STAGES) ++m_load;
			}
			void flush()
			{
				static_assert(std::is_default_constructible<T>::value, "Type must be default constructible");

				std::fill(m_elements.begin(), m_elements.end(), T{});
				std::fill(m_addresses.begin(), m_addresses.end(), T{});

				m_load = 0;                                                    //Reset load
			}

			inline unsigned int load() const
			{
				return m_load;
			}
			inline size_t size() const
			{
				return STAGES;
			}

			std::pair<T, T> operator[](unsigned int index)
			{
				if (index + 1 > m_load) throw std::out_of_range("Index out of range!");

				return std::make_pair(m_elements[index], m_addresses[index]);
			}

		private:
			std::array<T, STAGES> m_elements{};
			std::array<T, STAGES> m_addresses{};

			unsigned int m_load{};                                             //The range of m_load is [0, STAGES]
		};
	};
}
