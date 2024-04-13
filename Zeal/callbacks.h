#pragma once
#include "hook_wrapper.h"
#include "memory.h"
#include <functional>
#include <unordered_map>
enum class callback_type
{
	MainLoop,
	Zone,
	CleanUI,
	Render,
	CharacterSelect,
	InitUI,
	EndMainLoop,
	WorldMessage,
	SendMessage_,
	ExecuteCmd
};
class CallbackManager
{
public:
	void add_generic(std::function<void()> callback_function, callback_type fn = callback_type::MainLoop);
	void add_packet(std::function<bool(UINT, char*, UINT)> callback_function, callback_type fn = callback_type::WorldMessage);
	void add_command(std::function<bool(UINT, BOOL)> callback_function, callback_type fn = callback_type::ExecuteCmd);
	void invoke_generic(callback_type fn);
	bool invoke_packet(callback_type fn, UINT opcode, char* buffer, UINT len);
	bool invoke_command(callback_type fn, UINT opcode, bool state);
	CallbackManager(class ZealService* zeal);
	~CallbackManager();
	void eml();
private:
	std::unordered_map<callback_type, std::vector<std::function<void()>>> generic_functions;
	std::unordered_map<callback_type, std::vector<std::function<bool(UINT, char*, UINT)>>> packet_functions;
	std::unordered_map<callback_type, std::vector<std::function<bool(UINT, BOOL)>>> cmd_functions;
};

