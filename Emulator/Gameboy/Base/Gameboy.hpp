#pragma once

#include <memory>

#include "Gameboy/Component/CPU/Base/CPU.hpp"

class Gameboy
{
public:
	~Gameboy() = default;


	
	virtual void on() = 0;
	virtual void off() = 0;

protected:
	Gameboy() = default;

	std::unique_ptr<CPU> m_cpu;
};

