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
