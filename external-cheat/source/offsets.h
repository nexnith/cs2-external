#pragma once
#include <cstddef>

namespace offsets
{
	// buttons.hpp
	constexpr std::ptrdiff_t jump = 0x222B550;

	// client_dll.hpp
	constexpr std::ptrdiff_t m_fFlags = 0x3F4;

	// offsets.hpp
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x255B598;
}