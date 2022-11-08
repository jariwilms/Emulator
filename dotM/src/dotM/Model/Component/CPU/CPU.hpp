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
			struct extend{ T v : MSB; };                                       //The compiler extends the value for us
			
			return extend{ value }.v;
		}


		
		template<typename T>
		struct Register
		{
		public:
			Register() = default;
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
		template<typename T>
		struct RegisterReference
		{
		public:
			RegisterReference(Register<T>& reg)
				: m_register{ &reg } {}
			~RegisterReference() = default;

			inline Register<T>& get() const
			{
				return *m_register;
			}

			operator Register<T>() const
			{
				return *m_register;
			}
			operator Register<T>& ()
			{
				return *m_register;
			}

			void operator=(Register<T>& other)
			{
				m_register = &other;
			}

			template<typename U>
			void operator=(const U& other)
			{
				*m_register = other;
			}

			template<typename U>
			Register<T> operator+(const U& other)
			{
				auto result = *m_register;
				result += other;

				return result;
			}
			template<typename U>
			Register<T> operator-(const U& other)
			{
				auto result = *m_register;
				result -= other;

				return result;
			}
			template<typename U>
			Register<T> operator*(const U& other)
			{
				auto result = *m_register;
				result *= other;

				return result;
			}
			template<typename U>
			Register<T> operator/(const U& other)
			{
				auto result = *m_register;
				result /= other;

				return result;
			}
			template<typename U>
			Register<T> operator%(const U& other)
			{
				auto result = *m_register;
				result %= other;

				return result;
			}

			template<typename U>
			Register<T> operator&(const U& other)
			{
				auto result = *m_register;
				result &= other;

				return result;
			}
			template<typename U>
			Register<T> operator|(const U& other)
			{
				auto result = *m_register;
				result |= other;

				return result;
			}

			template<typename U>
			void operator+=(const U& other)
			{
				*m_register += other;
			}
			template<typename U>
			void operator-=(const U& other)
			{
				*m_register -= other;
			}

			template<typename U>
			void operator&=(const U& other)
			{
				*m_register &= other;
			}

		private:
			Register<T>* m_register;
		};
		
		template<typename T, size_t STAGES>
		struct Pipeline
		{
		public:
			Pipeline() = default;
			~Pipeline() = default;

			void push(const T& value)
			{
				m_elements.front() = value;

				if (m_load == 0) ++m_load;
			}
			void shift()
			{
				std::shift_right(m_elements.begin(), m_elements.end(), 1);
				m_elements.begin()->clear();

				if (m_load < STAGES) ++m_load;
			}
			void flush()
			{
				static_assert(std::is_default_constructible<T>::value, "Type must be default constructible");

				std::fill(m_elements.begin(), m_elements.end(), T{});

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

			T& operator[](unsigned int index)
			{
				if (index + 1 > m_load) throw std::out_of_range("Index out of range!");

				return m_elements[index];
			}

		private:
			std::array<T, STAGES> m_elements{};

			unsigned int m_load{};                                             //The range of m_load is [0, STAGES]
		};
	};
}
