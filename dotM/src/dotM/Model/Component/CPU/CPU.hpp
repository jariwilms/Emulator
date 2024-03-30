#pragma once

namespace dot
{
	class CPU
	{
	public:
		~CPU() = default;
		
	protected:
		CPU() = default;

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
				if (this != &other) m_value = other.m_value;

				return *this;
			}
			
			bool operator[](unsigned int index) const
			{
				return get_bit(m_value, index);
			}

		protected:
			T m_value{};
		};
		template<typename T, typename U, size_t STAGES>
		struct Pipeline
		{
		public:
			Pipeline() = default;
			~Pipeline() = default;

			void push(const T& value, const U& address)
			{
                std::shift_right(m_elements.begin(),  m_elements.end(),  1);
                std::shift_right(m_addresses.begin(), m_addresses.end(), 1);

                m_elements.front()  = value;
                m_addresses.front() = address;

                if (m_load < STAGES) ++m_load;
			}
			void flush()
			{
				static_assert(std::is_default_constructible<T>::value, "Type must be default constructible!");

				std::fill(m_elements.begin(),  m_elements.end(),  T{});
				std::fill(m_addresses.begin(), m_addresses.end(), U{});

				m_load = 0;
			}

			inline unsigned int load() const
			{
				return m_load;
			}
			inline size_t stages() const
			{
				return STAGES;
			}

			std::tuple<T, U> operator[](unsigned int index)
			{
				if (index + 1 > m_load) throw std::out_of_range("Index out of range!");

				return std::make_tuple(m_elements[index], m_addresses[index]);
			}

		private:
			std::array<T, STAGES> m_elements{};
			std::array<U, STAGES> m_addresses{};

			unsigned int m_load{};
		};
	};
}
