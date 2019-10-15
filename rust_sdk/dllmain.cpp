#include <windows.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include "utils.hpp"

std::vector<base_player*> entities;
std::atomic<base_camera*> camera;
std::atomic_bool should_exit( false );

void __stdcall entity_loop_thread( void* base_networkable )
{
	while ( !should_exit )
	{
		const auto unk1 = *reinterpret_cast< void** >( std::uintptr_t( base_networkable ) + 0xb8 );

		if ( !unk1 )
			continue;

		const auto client_entities = *reinterpret_cast< entity_realm** >( unk1 );

		if ( !client_entities )
			continue;

		const auto list = client_entities->list->values;

		if ( !list )
			continue;

		entity_mutex.lock( );

		if ( entities.size( ) >= 500 )
			entities.clear( );

		entity_mutex.unlock( );

		for ( auto i = 0u; i < list->size; i++ )
		{
			const auto element = *reinterpret_cast< void** >( std::uintptr_t( list->buffer ) + ( 0x20 + ( i * 8 ) ) );

			if ( !element || std::strstr( utils::mono::get_class_name( element ), "BasePlayer" ) == nullptr )
				continue;

			const auto base_mono_object = *reinterpret_cast< void** >( std::uintptr_t( element ) + 0x10 );

			if ( !base_mono_object )
				continue;

			const auto object = *reinterpret_cast< void** >( std::uintptr_t( base_mono_object ) + 0x30 );

			if ( !object )
				continue;

			const auto object_1 = *reinterpret_cast< game_object** >( std::uintptr_t( object ) + 0x30 );

			if ( !object_1 )
				continue;

			const auto player = object_1->unk->player;

			std::lock_guard guard( entity_mutex );

			if ( !player || player->health <= 0.8f || std::find( entities.begin( ), entities.end( ), player ) != entities.end( ) )
				continue;

			entities.push_back( player );
		}

		std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
	}
}

void __stdcall camera_loop_thread( void* game_object_manager )
{
	while ( !should_exit )
	{
		const auto last_object = *reinterpret_cast< unk1** >( game_object_manager );
		const auto first_object = *reinterpret_cast< unk1** >( std::uintptr_t( game_object_manager ) + 0x8 );

		for ( auto object = first_object; object != last_object; object = object->next )
		{
			if ( object->object->tag == 5 )
			{
				camera.store( reinterpret_cast< base_camera* >( object->object->object->unk ) );
				break;
			}
		}

		std::this_thread::sleep_for( std::chrono::seconds( 20 ) );
	}
}

void __stdcall main_thread( HMODULE module )
{
	AllocConsole( );
	freopen_s( reinterpret_cast< FILE** >( stdin ), "CONIN$", "r", stdin );
	freopen_s( reinterpret_cast< FILE** >( stdout ), "CONOUT$", "w", stdout );

	const auto base_networkable_address = utils::memory::find_signature( "GameAssembly.dll", "48 8b 05 ? ? ? ? 48 8b 88 ? ? ? ? 48 8b 09 48 85 c9 74 ? 45 33 c0 8b" );

	if ( !base_networkable_address )
		return;

	const auto base_networkable = reinterpret_cast<std::uintptr_t>( base_networkable_address + *reinterpret_cast< std::int32_t* >( base_networkable_address + 3 ) + 7 );

	if ( !base_networkable )
		return;

	std::printf( "BaseNetworkable: 0x%llx\n", ( base_networkable - std::uintptr_t( GetModuleHandleA( "GameAssembly.dll" ) ) ) );

	std::thread entity_iteration( &entity_loop_thread, *reinterpret_cast< void** >( base_networkable ) );

	const auto game_object_manager_address = utils::memory::find_signature( "UnityPlayer.dll", "48 89 05 ? ? ? ? 48 83 c4 38 c3 48 c7 05 ? ? ? ? ? ? ? ? 48 83 c4 38 c3 cc cc cc cc cc 48" );

	if ( !game_object_manager_address )
		return;

	const auto game_object_manager = reinterpret_cast< std::uintptr_t >( game_object_manager_address + *reinterpret_cast< std::int32_t* >( game_object_manager_address + 3 ) + 7 );

	if ( !game_object_manager )
		return;

	std::printf( "GameObjectManager: 0x%llx\n", ( game_object_manager - std::uintptr_t( GetModuleHandleA( "UnityPlayer.dll" ) ) ) );

	std::thread etc_iteration( &camera_loop_thread, *reinterpret_cast< void** >( game_object_manager ) );
	
	while ( !GetAsyncKeyState( VK_END ) )
	{
		static base_player* local_player = nullptr;

		std::lock_guard guard( entity_mutex );

		for ( const auto& entity : entities )
		{
			if ( !entity )
				continue;

			if ( entity->player_model->is_local_player )
			{
				local_player = entity;
				break;
			}
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
	}

	should_exit = true;
	entity_iteration.join( );
	etc_iteration.join( );

	fclose( reinterpret_cast< FILE* >( stdin ) );
	fclose( reinterpret_cast< FILE* >( stdout ) );
	FreeConsole( );
	PostMessage( GetConsoleWindow( ), WM_CLOSE, 0, 0 );
	FreeLibraryAndExitThread( module, EXIT_SUCCESS );
}

bool __stdcall DllMain( HMODULE module, std::uint32_t call_reason, void* )
{
	if ( call_reason != DLL_PROCESS_ATTACH )
		return false;

	if ( const auto handle = CreateThread( nullptr, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( main_thread ), module, 0, nullptr ); handle != NULL )
		CloseHandle( handle );
	
	return true;
}