#pragma once

template<typename T>
bit get_bit(T value, unsigned int index)
{
	return (value >> index) & 0x1;
}
template<typename T>
T get_bits(T value, unsigned int index, unsigned int amount)
{
	return (value >> index) & ((1 << amount) - 1);
}

template<typename T>
T set_bit(T value, unsigned int index, bit b)
{
	return b ? value | (1 << index) : value & ~(1 << index);
}
template<typename T, typename U>
T set_bits(T value, unsigned int index, unsigned int amount, U bits)
{
	return (value & ~(((1 << amount) - 1) << index)) | (bits << index);
}