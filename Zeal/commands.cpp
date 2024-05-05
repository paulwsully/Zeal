#include "commands.h"
#include "EqStructures.h"
#include "EqAddresses.h"
#include "EqFunctions.h"
#include "EqPackets.h"
#include "Zeal.h"
#include <algorithm>
#include <cctype>
#include "string_util.h"


void ChatCommands::print_commands()
{
	std::stringstream ss;
	ss << "List of commands" << std::endl;
	ss << "-----------------------------------------------------" << std::endl;
	for (auto& [name, c] : CommandFunctions)
	{
		ss << name;
		if (c.aliases.size() > 0)
			ss << " [";
		for (auto it = c.aliases.begin(); it != c.aliases.end(); ++it) {
			auto& a = *it;
			ss << a;
			if (std::next(it) != c.aliases.end()) {
				ss << ", ";
			}
		}
		if (c.aliases.size() > 0)
			ss << "]";

		ss << " " << c.description;
		ss << std::endl;
	}
	Zeal::EqGame::print_chat(ss.str());
}

void __fastcall InterpretCommand(int c, int unused, int player, char* cmd)
{
	ZealService* zeal = ZealService::get_instance();
	std::string str_cmd = cmd;

	if (str_cmd.length()>0 && str_cmd.at(0) != '/')
		str_cmd = "/" + str_cmd;
	std::vector<std::string> args = Zeal::String::split(str_cmd," ");

	if (args.size() > 0)
	{
		bool cmd_handled = false;
		if (zeal->commands_hook->CommandFunctions.count(args[0]) > 0)
		{
			cmd_handled = zeal->commands_hook->CommandFunctions[args[0]].callback(args);
		}
		else
		{
			for (auto& m : zeal->commands_hook->CommandFunctions)
			{
				for (auto& a : m.second.aliases)
				{
					if (a == args[0])
					{
						cmd_handled = m.second.callback(args);
						break;
					}
				}
			}
		}
		if (cmd_handled) {
			return;
		}
	}
	zeal->hooks->hook_map["commands"]->original(InterpretCommand)(c, unused, player, cmd);
}

void ChatCommands::add(std::string cmd, std::vector<std::string>aliases, std::string description, std::function<bool(std::vector<std::string>&args)> callback)
{
	CommandFunctions[cmd] = ZealCommand(aliases, description, callback);
}

ChatCommands::~ChatCommands()
{

}
ChatCommands::ChatCommands(ZealService* zeal)
{
	add("o", { "" }, "Removes the o command that is switching ui from new to old.",
		[](std::vector<std::string>& args) {
			if (Zeal::String::compare_insensitive(args[0], "o"))
				return true;
			return false;
		});
	add("/target", { "/cleartarget" }, "Adds clear target functionality to the /target command if you give it no arguments.",
		[](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				Zeal::EqGame::set_target(0);
				return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
			}
			return false;
		});
	add("/corpsedrag", { "/drag"}, "Attempts to corpse drag your current target.",
		[](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				if (Zeal::EqGame::get_target())
				{
					Zeal::Packets::CorpseDrag_Struct tmp;
					memset(&tmp, 0, sizeof(tmp));
					strcpy_s(tmp.CorpseName, 30, Zeal::EqGame::get_target()->Name);
					strcpy_s(tmp.DraggerName, 30, Zeal::EqGame::get_self()->Name);
					Zeal::EqGame::send_message(Zeal::Packets::opcodes::CorpseDrag, (int*)&tmp, sizeof(tmp), 0);
				}
				return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
			}
			return false;
		});
	add("/corpsedrop", { "/drop"}, "Attempts to drop a corpse (your current target). To drop all use /corpsedrop all",
		[](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				if (Zeal::EqGame::get_target())
				{
					Zeal::Packets::CorpseDrag_Struct tmp;
					memset(&tmp, 0, sizeof(tmp));
					strcpy_s(tmp.CorpseName, 30, Zeal::EqGame::get_target()->Name);
					strcpy_s(tmp.DraggerName, 30, Zeal::EqGame::get_self()->Name);
					Zeal::EqGame::send_message(Zeal::Packets::opcodes::CorpseDrop, (int*)&tmp, sizeof(tmp), 0);
				}
				return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
			}
			else if (Zeal::String::compare_insensitive(args[1], "all"))
			{
				Zeal::EqGame::send_message(Zeal::Packets::opcodes::CorpseDrop, 0, 0, 0);
			}
			return false;
		});
	add("/trade", { "/opentrade", "/ot" }, "Opens a trade window with your current target.",
		[](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				if (Zeal::EqGame::get_target())
				{
					Zeal::Packets::TradeRequest_Struct tmp;
					memset(&tmp, 0, sizeof(tmp));
					tmp.from_id = Zeal::EqGame::get_self()->SpawnId;
					tmp.to_id = Zeal::EqGame::get_target()->SpawnId;
					Zeal::EqGame::send_message(Zeal::Packets::opcodes::RequestTrade, (int*)&tmp, sizeof(tmp), 0);
				}
				return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
			}
			return false;
		});
	add("/autoinventory", { "/autoinv", "/ai" }, "Puts whatever is on your cursor into your inventory.",
		[](std::vector<std::string>& args) {
			Zeal::EqGame::EqGameInternal::auto_inventory(Zeal::EqGame::get_char_info(), &Zeal::EqGame::get_char_info()->CursorItem, 0);
			return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
		});
	add("/autobank", { "/autoba", "/ab" }, "Changes your money into its highest denomination in bank and inventory (requires bank to be open).",
		[](std::vector<std::string>& args) {
			ZealService::get_instance()->ui->bank->change();
			return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
		});
	add("/aspectratio", { "/ar"}, "Change your aspect ratio.",
		[](std::vector<std::string>& args) {
			Zeal::EqStructures::CameraInfo* ci = Zeal::EqGame::get_camera();
			if (ci)
			{
				float ar = 0;
				if (args.size() > 1 && Zeal::String::tryParse(args[1], &ar))
				{
					ci->AspectRatio = ar;
				}
				else
				{
					Zeal::EqGame::print_chat("Current Aspect Ratio [%f]", ci->AspectRatio);
				}

			}

			return true; //return true to stop the game from processing any further on this command, false if you want to just add features to an existing cmd
		});
	add("/clearchat", {}, "Adds clearchat functionality to oldui.",
		[](std::vector<std::string>& args) {
			// each line can hold up to 256 characters but typically doesnt reach that far.
			// each unused line is set with '@\0', so lets clear to that
			if (!Zeal::EqGame::is_new_ui()) {
				int start = 0x799D8D;
				int end = 0x7B908D;
				while (start != end) {
					mem::write<WORD>(start, 0x4000);
					start += 256;
				}
				// max scrollbar height doesn't reset properly here. Need to figure out why still.
				return true;
			}

			return false;
		});
	add("/sit", {}, "Adds arguments on/down to force you to sit down instead of just toggling.",
		[](std::vector<std::string>& args) {
			if (args.size() > 1 && args.size() < 3) {
				if (Zeal::String::compare_insensitive(args[1], "on") || Zeal::String::compare_insensitive(args[1], "down")) {
					Zeal::EqGame::get_self()->ChangeStance(Stance::Sit);
					return true;
				}
			}
			return false;
		});
	add("/camp", {}, "Adds auto sit when attempting to camp.",
		[](std::vector<std::string>& args) {
			Zeal::EqGame::get_self()->ChangeStance(Stance::Sit);
			return false;
		});
	add("/showhelm", { "/helm" }, "Sends server command #showhelm with arugments on/off.",
		[](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				Zeal::EqGame::print_chat("usage: /showhelm [on | off]");
				return true;
			}
			if (args.size() > 1 && args.size() < 3) {
				if (Zeal::String::compare_insensitive(args[1], "on")) {
					Zeal::EqGame::do_say(true, "#showhelm on");
					return true;
				}
				else if (Zeal::String::compare_insensitive(args[1], "off"))
				{
					Zeal::EqGame::do_say(true, "#showhelm off");
					return true;
				}
			}
			return false;
		});
	add("/showlootlockouts", { "/sll", "/showlootlockout", "/showlockouts" }, "Sends #showlootlockouts to server.",
		[](std::vector<std::string>& args) {

			Zeal::EqGame::do_say(true, "#showlootlockouts");
			return true;
		});
	add("/zeal", { "/zea" }, "Help and version information.",
		[this](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				Zeal::EqGame::print_chat("Available args: version, help"); //leave room for more args on this command for later
				return true;
			}
			if (args.size() > 1 && Zeal::String::compare_insensitive(args[1], "version"))
			{
				std::stringstream ss;
				ss << "Zeal version: " << ZEAL_VERSION << std::endl;
				Zeal::EqGame::print_chat(ss.str());
				return true;
			}
			if (args.size() > 1 && Zeal::String::compare_insensitive(args[1], "help"))
			{
				print_commands();
				return true;
			}
			return false;
		});
	
	add("/clientmanatick", { "/cmt" }, "Toggle client mana tick (disabled by default in this client).",
		[this](std::vector<std::string>& args) {
			BYTE orig1[7] = { 0x66, 0x01, 0xBE, 0x9A, 0x0, 0x0, 0x0 }; //0x4C3F93
			BYTE orig2[7] = { 0x66, 0x01, 0x87, 0x9A, 0x0, 0x0, 0x0 }; //0x4c7642
			if (*(BYTE*)0x4C3F93 == 0x90)
			{
				mem::copy(0x4C3F93, (int)orig1, sizeof(orig1));
				mem::copy(0x4C7642, (int)orig2, sizeof(orig2));
				Zeal::EqGame::print_chat("Client sided mana tick re-enabled");
			}
			else
			{
				mem::set(0x4C3F93, 0x90, sizeof(orig1));
				mem::set(0x4C7642, 0x90, sizeof(orig2));
				Zeal::EqGame::print_chat("Client sided mana tick disabled");
			}
			return true;
		});

	add("/reloadskin", { }, "Reload your current ui with ini.",
		[this](std::vector<std::string>& args) {
			mem::write<BYTE>(0x8092d9, 1); //reload skin
			mem::write<BYTE>(0x8092da, 1);  //reload with ui
			return true;
		});
	add("/alarm", {}, "Open the alarm ui and gives alarm functionality on old ui.",
		[this, zeal](std::vector<std::string>& args) {
			if (Zeal::EqGame::is_new_ui()) {
				if (Zeal::EqGame::Windows->Alarm)
					Zeal::EqGame::Windows->Alarm->IsVisible = true;
				else
					Zeal::EqGame::print_chat("Alarm window not found");

				return true;
			}
			else {
				if (args.size() == 1) {
					std::ostringstream oss;
					oss << "-- ALARM COMMANDS --" << std::endl << "/alarm set #m#s" << std::endl << "/alarm halt" << std::endl;
					Zeal::EqGame::print_chat(oss.str());
					return true;
				}
				if (args.size() > 1 && args.size() < 4) {
					if (Zeal::String::compare_insensitive(args[1], "set")) {
						int minutes = 0;
						int seconds = 0;
						size_t delim[2] = { args[2].find("m"), args[2].find("s") };
						if (Zeal::String::tryParse(args[2].substr(0, delim[0]), &minutes) &&
								Zeal::String::tryParse(args[2].substr(delim[0]+1, delim[1]), &seconds))
						{
							zeal->alarm->Set(minutes, seconds);
							return true;
						}
						else
						{
							Zeal::EqGame::print_chat("[Alarm] Failed to parse the specified duration.");
							return true;
						}
					}
					else if (Zeal::String::compare_insensitive(args[1], "halt")) {
						zeal->alarm->Halt();
						return true;
					}
				}
			}
			return false;
		});
		add("/inspect", {}, "Inspect your current target.",
		[this, zeal](std::vector<std::string>& args) {
				if (args.size() > 1 && args[1] == "target")
				{
					Zeal::EqStructures::Entity* t = Zeal::EqGame::get_target();
					if (!t || t->Type!=0)
					{
						Zeal::EqGame::print_chat("That is not a valid target to inspect!");
						return true;
					}
					else
					{
						Zeal::EqGame::do_inspect(t);
						return true;
					}
				}
			return false;
		});
	add("/help", { "/hel" }, "Adds zeal to the help list.",
		[this](std::vector<std::string>& args) {
			if (args.size() == 1)
			{
				std::stringstream ss;
				ss << "Format: /help <class> Where class is one of normal, emote, guild, ect.." << std::endl;
				ss << "Normal will display a list of all commands." << std::endl;
				ss << "Emote will display a list of all player emotes." << std::endl;
				ss << "Guild will display a list of guild commands." << std::endl;
				ss << "Voice will display a list of voice control commands." << std::endl;
				ss << "Chat will display a list of chat channel commands." << std::endl;
				ss << "Zeal will display a list of custom commands." << std::endl;
				Zeal::EqGame::print_chat(ss.str());
				return true;
			}
			if (args.size() > 1 && Zeal::String::compare_insensitive(args[1],"zeal"))
			{
				print_commands();
				return true;
			}
			return false;
		});
	zeal->hooks->Add("commands", Zeal::EqGame::EqGameInternal::fn_interpretcmd, InterpretCommand, hook_type_detour);
}

