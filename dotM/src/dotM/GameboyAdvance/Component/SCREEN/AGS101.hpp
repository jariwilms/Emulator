#pragma once

#include <stdafx.hpp>

namespace dot::gba
{
	class AGS101
	{
	public:
		AGS101() = default;
		~AGS101() = default;

	private:
		std::shared_ptr<MEMORY> m_VRAM{};

		std::pair<unsigned int, unsigned int> m_dimensions{ 240, 160 };
	};
}
