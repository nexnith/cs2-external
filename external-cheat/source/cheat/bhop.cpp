#include "memory.h"
#include "..\offsets.h"
#include <Windows.h>
#include "bhop.h"

void bhop( const memory& mem, uintptr_t client )
{ 
	uintptr_t localPlayer = 0;
	uint32_t flags = 0;

	if ( !mem.read<uintptr_t>( client + offsets::dwLocalPlayerPawn, localPlayer ) || localPlayer == 0 )
	{
		return;
	}

	if ( !mem.read<uint32_t>( localPlayer + offsets::m_fFlags, flags ) )
	{
		return;
	}

	if ( GetAsyncKeyState( VK_SPACE ) && 0x8000 )
	{
		if ( flags & ( 1 << 0 ) )
		{
			mem.write<std::uint32_t>( client + offsets::jump, 65537 );
		}
		else {
			mem.write < std::uint32_t>( client + offsets::jump, 256 );
		}
	}
}
