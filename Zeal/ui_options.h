#pragma once
#include "hook_wrapper.h"
#include "memory.h"
#include "EqUI.h"

class ui_options
{
public:
	void InitUI();
	std::unordered_map<Zeal::EqUI::BasicWnd*, std::function<void(Zeal::EqUI::BasicWnd*)>> checkbox_callbacks;
	std::unordered_map<std::string, Zeal::EqUI::BasicWnd*> checkbox_names;
	std::unordered_map<Zeal::EqUI::SliderWnd*, std::function<void(Zeal::EqUI::SliderWnd*, int)>> slider_callbacks;
	std::unordered_map<std::string, Zeal::EqUI::SliderWnd*> slider_names;
	std::unordered_map<Zeal::EqUI::BasicWnd*, std::function<void(Zeal::EqUI::BasicWnd*, int)>> combo_callbacks;
	std::unordered_map<std::string, Zeal::EqUI::BasicWnd*> combo_names;
	std::unordered_map<std::string, Zeal::EqUI::BasicWnd*> label_names;
	void SetLabelValue(std::string name, const char* format, ...);
	void SetSliderValue(std::string name, int value);
	void SetSliderValue(std::string name, float value);
	void SetComboValue(std::string name, int value);
	void AddCheckboxCallback(std::string name, std::function<void(Zeal::EqUI::BasicWnd*)> callback);
	void AddSliderCallback(std::string name, std::function<void(Zeal::EqUI::SliderWnd*, int)> callback);
	void AddComboCallback(std::string name, std::function<void(Zeal::EqUI::BasicWnd*, int)> callback);
	void AddLabel(std::string name);
	void SetChecked(std::string name, bool checked);
	void UpdateOptions();
	void CleanUI();
	ui_options(class ZealService* pHookWrapper, class IO_ini* ini);
	~ui_options();
private:
	void LoadSettings(class IO_ini* ini);
};
