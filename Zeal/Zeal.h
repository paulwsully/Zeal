#pragma once
#include "framework.h"
#define ZEAL_VERSION "0.1.41"

class ZealService
{
public:
	HMODULE	this_module = nullptr;
	//hooks
	std::shared_ptr<IO_ini> ini = nullptr;
	std::shared_ptr<HookWrapper> hooks = nullptr;
	std::shared_ptr<looting> looting_hook = nullptr;
	std::shared_ptr<labels> labels_hook = nullptr;
	std::shared_ptr<Binds> binds_hook = nullptr;
	std::shared_ptr<ChatCommands> commands_hook = nullptr;
	std::shared_ptr<MainLoop> main_loop_hook = nullptr;
	std::shared_ptr<CameraMods> camera_mods = nullptr;
	std::shared_ptr<raid> raid_hook = nullptr;
	std::shared_ptr<eqstr> eqstr_hook = nullptr;
	std::shared_ptr<chat> chat_hook = nullptr;
	std::shared_ptr<SpellSets> spell_sets = nullptr;
	std::shared_ptr<ItemDisplay> item_displays = nullptr;


	//other features
	std::shared_ptr<OutputFile> outputfile = nullptr;
	std::shared_ptr<Experience> experience = nullptr;
	std::shared_ptr<CycleTarget> cycle_target = nullptr;
	std::shared_ptr<BuffTimers> buff_timers = nullptr;
	std::shared_ptr<PlayerMovement> movement = nullptr;
	std::shared_ptr<Alarm> alarm = nullptr;
	std::shared_ptr<Netstat> netstat = nullptr;
	std::shared_ptr<UIOptions> ui = nullptr;
	std::shared_ptr<Melody> melody = nullptr;
	
	

	bool exit = false;
	void apply_patches();
	ZealService();
	~ZealService();

	//static data/functions to get a base ptr since some hook callbacks don't have the information required
	static ZealService* ptr_service;
	static ZealService* get_instance();
};

