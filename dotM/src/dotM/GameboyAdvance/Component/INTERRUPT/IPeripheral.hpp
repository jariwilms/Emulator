#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/CPU/ARM7TDMI.hpp"

namespace dot::gba
{
	class IPeripheral
	{
	public:
		IPeripheral() = default;
		~IPeripheral() = default;

		void link(std::shared_ptr<ARM7TDMI> processor)
		{
			this->processor = processor;
		}

	protected:
		void raise_interrupt(ARM7TDMI::InterruptType type = ARM7TDMI::InterruptType::IRQ)
		{
			processor->signal_irq(type);
		}

	private:
		std::shared_ptr<ARM7TDMI> processor{};
	};
}