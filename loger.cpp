#include "loger.h"

namespace fs = std::filesystem;

void Loger::SetLogParameter(const fs::path& log_file, int log_level) {
	log_file_ = log_file;
	log_level_ = log_level;
}

void Loger::SaveEventToFile(const std::string& event_text) {
	std::ofstream log_file(log_file_, std::ios::app);
	log_file << event_text << '\n';
}

void Loger::SaveEventToLog(const std::string& event_text, LOG_STATUS status) {
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	
	std::stringstream output_log;
	std::string status_string = status == LOG_STATUS::OK ? "OK" : "ERROR";
	
	output_log << std::ctime(&end_time) << '\t'
		<< status_string
		<< " - " << event_text;
		
	bool save_to_file = !((log_level_ == 0) || (log_level_ == 1 && status == LOG_STATUS::OK));
	
	if (save_to_file) {
		SaveEventToFile(output_log.str());
	}
}
	
//fs::path log_file_;
