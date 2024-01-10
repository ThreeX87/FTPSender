#pragma once

#include <fstream>
#include <string>
#include <filesystem>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iostream>

#include "common.h"

namespace fs = std::filesystem;

class Loger {
public:
	Loger() = default;
	void SetLogParameter(const fs::path& log_file, int log_level);
	
	void SaveEventToLog(const std::string& event_text, LOG_STATUS status);
private:
	fs::path log_file_;
	int log_level_ = 0;
	
	void SaveEventToFile(const std::string& event_text);
};