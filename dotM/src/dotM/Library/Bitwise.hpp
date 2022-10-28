#pragma once

template<typename T>
constexpr bit get_bit(const T& value, unsigned int position)
{
	return (value >> position) & 0x1;
}
template<typename T>
constexpr unsigned long long get_bits(const T& value, unsigned int position, unsigned int count)
{
	return (value >> position) & ((1ul << count) - 1);
}

template<typename T>
constexpr T set_bit(const T& value, unsigned int position, bool state)
{
	return value ^ (!state ^ value) & (1ul << position);
}
template<typename T, typename U>
constexpr T set_bits(const T& value, unsigned int position, unsigned int count, const U& bits) //not tested
{
	return (value & ~(((1ul << count) - 1) << position)) | (bits << position);
}
