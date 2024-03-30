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

	return value ^ (-static_cast<int>(state) ^ value) & ((U{} + 1) << index);
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
    struct extend { T v : MSB; };                                       //Let the compiler handle it

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

enum class ShiftType
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