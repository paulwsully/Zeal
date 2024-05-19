#pragma once
#include "hook_wrapper.h"
#include "memory.h"

class labels
{
public:
	std::string debug_info;
	void print_debug_info(std::string);
	void print_debug_info(const char* format, ...);
	bool GetLabel(int EqType, std::string& str);
	int GetGauge(int EqType, std::string& str);
	labels(class ZealService* zeal);
	~labels();
	void callback_main();
private:
};
