#pragma once

template<typename T>
constexpr bit get_bit(const T& value, unsigned int index)
{
	return (value >> index) & 0x1;
}
template<typename T, typename U>
constexpr U get_bits(const T& value, U index, U count)
{
	return (value >> index) & ((1ul << count) - 1);
}

template<typename T>
constexpr T set_bit(const T& value, unsigned int index, bool state)
{
	return value ^ (-static_cast<int>(state) ^ value) & (1ul << index);
}
template<typename T, typename U>
constexpr T set_bits(const T& value, unsigned int index, unsigned int count, const U& bits)
{
	return (value & ~(((1ul << count) - 1) << index)) | (bits << index);
}
