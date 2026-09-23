#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <memory>

class memory
{
private:
	struct handle_deleter
	{
		void operator()( HANDLE h ) const noexcept
		{
			if ( h && h != INVALID_HANDLE_VALUE )
			{
				CloseHandle( h );
			}
		}
	};
	using unique_handle = std::unique_ptr<void, handle_deleter>;

	uintptr_t process_id = 0;
	unique_handle processHandle = nullptr;

public:
	explicit memory( const char* processName ) noexcept
	{
		PROCESSENTRY32 entry{ sizeof( PROCESSENTRY32 ) };
		unique_handle snapshot{ CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 ) };

		if ( snapshot.get( ) == INVALID_HANDLE_VALUE ) return;

		if ( Process32First( snapshot.get( ), &entry ) )
		{
			do {
				if ( strcmp( processName, entry.szExeFile ) == 0 ) {
					process_id = entry.th32ProcessID;
					processHandle.reset( OpenProcess(
						PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
						FALSE,
						static_cast< DWORD >( process_id )
					) );
					break;
				} 
			} while ( Process32Next( snapshot.get( ), &entry ) );
		}
	}

	~memory( ) = default;

	memory( const memory& ) = delete;
	memory& operator = ( const memory& ) = delete;
	memory( memory&& ) noexcept = default;
	memory& operator = ( memory&& ) noexcept = default;

	bool isValid( ) const noexcept {
		return processHandle != nullptr && processHandle.get( ) != INVALID_HANDLE_VALUE;
	}

	uintptr_t getModuleAdress( const char* moduleName ) const noexcept
	{
		if ( process_id == 0 )
			return 0;

		MODULEENTRY32 entry{ sizeof( MODULEENTRY32 ) };
		unique_handle snapshot{ CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, static_cast< DWORD >( process_id ) ) };

		if ( snapshot.get( ) == INVALID_HANDLE_VALUE ) return 0;

		if ( ::Module32First( snapshot.get( ), &entry ) ) {
			do {
				if ( strcmp( moduleName, entry.szModule ) == 0 ) {
					return reinterpret_cast< std::uintptr_t >( entry.modBaseAddr );
				}
			} while ( Module32Next( snapshot.get( ), &entry ) );
		}

		return 0;
	}

	template <typename T>
	bool read( const uintptr_t adress, T& outValue ) const noexcept
	{
		if ( !isValid( ) ) return false;
		SIZE_T bytes_read = 0;
		const bool success = ReadProcessMemory(
			processHandle.get( ),
			reinterpret_cast< void* >( adress ),
			&outValue,
			sizeof( T ),
			&bytes_read );

		return success && ( bytes_read == sizeof( T ) );
	}

	template <typename T>
    bool write(const std::uintptr_t address, const T& value) const noexcept
    {
        if (!isValid()) return false;

        SIZE_T bytes_written = 0;
        const bool success = ::WriteProcessMemory(
            processHandle.get(),
            reinterpret_cast<void*>(address),
            &value,
            sizeof(T),
            &bytes_written
        );

        return success && (bytes_written == sizeof(T));
    }
};