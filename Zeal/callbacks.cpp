#include "callbacks.h"
#include "EqStructures.h"
#include "EqAddresses.h"
#include "EqFunctions.h"
#include "Zeal.h"
CallbackManager::~CallbackManager()
{

}

void __fastcall main_loop_hk(int t, int unused)
{
	ZealService* zeal = ZealService::get_instance();
	zeal->callbacks->invoke_generic(callback_type::MainLoop);
	zeal->hooks->hook_map["main_loop"]->original(main_loop_hk)(t, unused);
}

void __fastcall render_hk(int t, int unused)
{
	ZealService* zeal = ZealService::get_instance();
	zeal->callbacks->invoke_generic(callback_type::Render);
	zeal->hooks->hook_map["Render"]->original(render_hk)(t, unused);
}

void _fastcall charselect_hk(int t, int u)
{
	ZealService* zeal = ZealService::get_instance();
	zeal->callbacks->invoke_generic(callback_type::CharacterSelect);
	zeal->hooks->hook_map["DoCharacterSelection"]->original(charselect_hk)(t, u);
}

void CallbackManager::eml()
{
	invoke_generic(callback_type::EndMainLoop);
}

void CallbackManager::invoke_generic(callback_type fn)
{
	for (auto& f : generic_functions[fn])
			f();
}

void CallbackManager::add_generic(std::function<void()> callback_function, callback_type fn)
{
	generic_functions[fn].push_back(callback_function);
}
void CallbackManager::add_packet(std::function<bool(UINT, char*, UINT)> callback_function, callback_type type)
{
	packet_functions[type].push_back(callback_function);
}
void CallbackManager::add_command(std::function<bool(UINT, BOOL)> callback_function, callback_type type)
{
	cmd_functions[type].push_back(callback_function);
}

void __fastcall enterzone_hk(int t, int unused, int hwnd)
{
	ZealService* zeal = ZealService::get_instance();
	zeal->callbacks->invoke_generic(callback_type::Zone);
	zeal->hooks->hook_map["EnterZone"]->original(enterzone_hk)(t, unused, hwnd);
}
void __fastcall initgameui_hk(int t, int u)
{
	ZealService* zeal = ZealService::get_instance();
	zeal->hooks->hook_map["InitGameUI"]->original(initgameui_hk)(t, u);
	zeal->callbacks->invoke_generic(callback_type::InitUI);
}
void __stdcall clean_up_ui()
{
	ZealService* zeal = ZealService::get_instance();
	zeal->callbacks->invoke_generic(callback_type::CleanUI);
	zeal->hooks->hook_map["CleanUpUI"]->original(clean_up_ui)();
}

bool CallbackManager::invoke_packet(callback_type fn, UINT opcode, char* buffer, UINT len)
{
	for (auto& fn : packet_functions[fn])
	{
		if (fn(opcode, buffer, len))
			return true;
	}
	return false;
}

bool CallbackManager::invoke_command(callback_type fn, UINT opcode, bool state)
{
	for (auto& fn : cmd_functions[fn])
	{
		if (fn(opcode, state))
			return true;
	}
	return false;
}

char __fastcall handleworldmessage_hk(int* connection, int unused, UINT unk, UINT opcode, char* buffer, UINT len)
{
	ZealService* zeal = ZealService::get_instance();

	//static std::vector<UINT> ignored_opcodes = { 16543, 16629, 16530, 16425, 16562, 16526, 16694, 8253, 16735, 16727, 16735, 16458 };
	//if (!std::count(ignored_opcodes.begin(), ignored_opcodes.end(), opcode))
	//	Zeal::EqGame::print_chat("opcode: 0x%x len: %i", opcode, len);

	if (!Zeal::EqGame::get_self() && opcode == 0x4107) //a fix for a crash reported by Ecliptor at 0x004E2803
		return 1;

	if (zeal->callbacks->invoke_packet(callback_type::WorldMessage,opcode, buffer, len))
		return 1;

	return zeal->hooks->hook_map["HandleWorldMessage"]->original(handleworldmessage_hk)(connection, unused, unk, opcode, buffer, len);
}
void send_message_hk(int* connection, UINT opcode, char* buffer, UINT len, int unknown)
{
	ZealService* zeal = ZealService::get_instance();
	//Zeal::EqGame::print_chat("Opcode %i   len: %i", opcode, len);
	if (zeal->callbacks->invoke_packet(callback_type::SendMessage_, opcode, buffer, len))
		return;
	zeal->hooks->hook_map["SendMessage"]->original(send_message_hk)(connection, opcode, buffer, len, unknown);
}

void executecmd_hk(UINT cmd, bool isdown, int unk2)
{
	ZealService* zeal = ZealService::get_instance();
	//Zeal::EqGame::print_chat(USERCOLOR_SHOUT, "Cmd: %i", cmd);
	if (cmd == 0xd2)
		zeal->callbacks->invoke_generic(callback_type::EndMainLoop);
	if (zeal->callbacks->invoke_command(callback_type::ExecuteCmd, cmd, isdown))
		return;

	zeal->hooks->hook_map["executecmd"]->original(executecmd_hk)(cmd, isdown, unk2);
}

CallbackManager::CallbackManager(ZealService* zeal)
{
	zeal->hooks->Add("executecmd", 0x54050c, executecmd_hk, hook_type_detour);
	zeal->hooks->Add("main_loop", 0x5473c3, main_loop_hk, hook_type_detour);
	zeal->hooks->Add("Render", 0x4AA8BC, render_hk, hook_type_detour);
	zeal->hooks->Add("EnterZone", 0x53D2C4, enterzone_hk, hook_type_detour);
	zeal->hooks->Add("CleanUpUI", 0x4A6EBC, clean_up_ui, hook_type_detour);
	zeal->hooks->Add("DoCharacterSelection", 0x53b9cf, charselect_hk, hook_type_detour);
	zeal->hooks->Add("InitGameUI", 0x4a60b5, initgameui_hk, hook_type_detour);
	zeal->hooks->Add("HandleWorldMessage", 0x4e829f, handleworldmessage_hk, hook_type_detour);
	zeal->hooks->Add("SendMessage", 0x54e51a, send_message_hk, hook_type_detour);
}
