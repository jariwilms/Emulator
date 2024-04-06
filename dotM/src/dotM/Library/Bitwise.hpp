#pragma once

template<typename T, typename U>
constexpr inline bool get_bit(const T& value, U index)
{
	static_assert(std::is_integral<T>::value, "Only integral types are allowed!");
	static_assert(std::is_integral<U>::value, "Only integral types are allowed!");

	static_assert(sizeof(T) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");
	static_assert(sizeof(U) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");

	return (value >> index) & 0x1;
}
template<typename T, typename U>
constexpr inline T get_bits(const T& value, U index, U count)
{
	static_assert(std::is_integral<T>::value, "Only integral types are allowed!");
	static_assert(std::is_integral<U>::value, "Only integral types are allowed!");

	static_assert(sizeof(T) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");
	static_assert(sizeof(U) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");

	return (value >> index) & (((U{} + 1) << count) - 1);
}

template<typename T, typename U>
constexpr inline T set_bit(const T& value, U index, bool state)
{
	static_assert(std::is_integral<T>::value, "Only integral types are allowed!");
	static_assert(std::is_integral<U>::value, "Only integral types are allowed!");

	static_assert(sizeof(T) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");
	static_assert(sizeof(U) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");

	return value ^ (-static_cast<int>(state) ^ value) & ((U{} + 1) << index);  //TODO: cast<int> might give problem with larger types
}
template<typename T, typename U, typename V>
constexpr inline T set_bits(const T& value, U index, U count, const V& src)
{
	static_assert(std::is_integral<T>::value, "Only integral types are allowed!");
	static_assert(std::is_integral<U>::value, "Only integral types are allowed!");
	static_assert(std::is_integral<V>::value, "Only integral types are allowed!");

	static_assert(sizeof(T) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");
	static_assert(sizeof(U) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");
	static_assert(sizeof(V) <= sizeof(unsigned long long), "Size may not exceed unsigned long long size!");

	return (value & ~((((U{} + 1) << count) - 1) << index)) | (src << index);
}

template <typename T, size_t MSB>
static constexpr T sign_extend(const T& value)
{
    struct extend { T v : MSB; };                                              //Let the compiler handle it

    return extend{ value }.v;
}

template<typename T>
static constexpr T bit_reverse(const T& value)
{
    static_assert(std::is_integral<T>::value, "T must be an integral type!");

    T result{};
    constexpr auto size = sizeof(T) * 8;

    for (auto i = 0; i < size; ++i)
    {
        result |= get_bit(value, i) << (size - i - 1);
    }

    return result;
}

enum class ShiftType                                                           //TODO: make more generic. Values are bound to ARM7
{
    LSL = 0b00,                                                                //Logical Shift Left
    LSR = 0b01,                                                                //Logical Shift Right
    ASR = 0b10,                                                                //Arithmetic Shift Right
    ROR = 0b11,                                                                //Rotate Right
};
template<typename T, typename U>
static constexpr T shift(T value, ShiftType type, U bits)
{
    static_assert(std::is_integral<T>::value, "T must be an integral type!");
    static_assert(std::is_integral<U>::value, "U must be an integral type!");



    constexpr auto size = sizeof(T) * 8;

    switch (type)
    {
        case ShiftType::LSL: return value << bits;
        case ShiftType::LSR: return value >> bits;
        case ShiftType::ASR: return get_bit(value, size - 1) ? ~(~value >> bits) : value >> bits;
        case ShiftType::ROR: return (value >> bits) | (value << (size - bits));

        default:             throw std::runtime_error("Invalid ShiftType!");
    }
}

template<typename T, typename U>
static constexpr T shifted_composite(const U& msb, const U& lsb)
{
    static_assert(sizeof(T) == 2 * sizeof(U),                     "Size of types does not match!");
    static_assert(std::is_unsigned_v<T> && std::is_unsigned_v<U>, "Operands must be unsigned!");

    return T{ static_cast<T>((msb << (8 * sizeof(U))) | lsb) }; //msb and lsb are promoted to at least an int
}

template<typename T>
bool carry_occurred_add(const T& a, const T& b, std::uint32_t index) //Index = bit to which was carried
{
    const auto& lowerMask = ~(~0u << index);
    const auto& upperMask = lowerMask + 1;

    return ((a & lowerMask) + (b & lowerMask) & upperMask) == upperMask;
}
template<typename T>
bool carry_occurred_sub(const T& a, const T& b, std::uint32_t index) //Index = bit to which was carried
{
    const auto& bitMask = ~(~0u << index);

    return (a & bitMask) < (b & bitMask);
}
