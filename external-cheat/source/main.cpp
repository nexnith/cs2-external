#include <iostream>
#include <thread>
#include <chrono>
#include "memory/memory.h"
#include "offsets.h"
#include "cheat/bhop.h"

int main( )
{
    memory mem{ "cs2.exe" };

    while ( !mem.isValid( ) )
    {
        std::cout << "waiting for process..." << std::endl;

        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        mem = memory{ "cs2.exe" };
    }

    uintptr_t client = mem.getModuleAdress( "client.dll" );

    while ( client == 0 )
    {
        std::cout << "waiting for client.dll..." << std::endl;

        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        client = mem.getModuleAdress( "client.dll" );
    }

    std::cout << "client.dll -> " << "0x" << std::hex << client << std::dec << std::endl;

    while ( true && (!GetAsyncKeyState(VK_DELETE )))
    {
        bhop( mem, client );

        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    }

    return 0;
}