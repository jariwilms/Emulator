#pragma once

#include <stdafx.hpp>

#include "dotM/GameboyAdvance/Component/BUS/BUS.hpp"

namespace dot::gba
{
	template<size_t WIDTH>
	class IConnectable
	{
	public:
		explicit IConnectable(std::shared_ptr<BUS<WIDTH>> bus = nullptr)
			: bus{ bus } {}
		virtual ~IConnectable() = default;

		void connect(std::shared_ptr<BUS<WIDTH>> bus)
		{
			this->bus = bus;
		}

	protected:
		std::shared_ptr<BUS<WIDTH>> bus{};
	};
}