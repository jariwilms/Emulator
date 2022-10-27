#pragma once

template<typename T>
inline bit get_bit(const T& value, unsigned int position)
{
	return (value & (1ul << position)) >> position;
}
template<typename T>
inline T set_bit(const T& value, unsigned int position, bool state)
{
	return (value & ~(1ul << position)) | (state << position);
}
