#include "ini.h"
#include <cassert>

std::string ClearLine(const std::string& line, const char char_begin, const char char_end) {
	if (line.size() == 0) {
		return "";
	}
	size_t start = line.find_first_not_of(char_begin);
	size_t finish = line.find_last_not_of(char_end);
	
	if (start == std::string::npos) {
		return "";
	}
	if (finish == std::string::npos) {
		return line.substr(start);
	}
	return line.substr(start, finish - start + 1);
}

std::string ClearLine(const std::string& line, const char c) {
	return ClearLine(line, c, c);
}

std::pair<std::string, std::string> ParseParameter(const std::string& line) {
	size_t pos = line.find("=");
	std::string parameter;
	std::string value;
	if (pos == std::string::npos) {
		return {"", ""};
	}
	parameter = ClearLine(line.substr(0, pos), ' ');
	value = ClearLine(line.substr(pos + 1), ' ');
	return {parameter, value};
}

INI::INI(const std::string& filename)
	: filename_(filename) {
	ReadFile();
}

INI::~INI() {
	SaveFile();
}

void INI::ReadFile() {
	std::ifstream file(filename_);
	if (!file.is_open()) {
		std::ofstream create_file(filename_);
		return;
	}
	std::string line;
	std::string group;

	while(!file.eof()) {
		getline(file, line);
		if (line[0] == ';') {
			continue;
		}
		else if (line[0] == '[') {
			group = ClearLine(ClearLine(line, '[', ']'), ' ');
		}
		else {
			const auto r = ParseParameter(line);
			if (r.first != "" && r.second != "") {
				data_[group].insert(r);
			}
		}
	}
}

const std::map<std::string, std::map<std::string, std::string>>& INI::GetAllData() const {
	return data_;
} 

void INI::SaveFile() {
	if (!IsEdit_) {
		return;
	}
	std::ofstream file(filename_);
	for (const auto [group, par] : data_) {
		file << "[" << group << "]" << '\n';
		for (const auto [param, value] : par) {
			if (param != "")
				file << param << " = " << value << '\n';
		}
		file << '\n';
	}
}

const std::string& INI::GetValue(const std::string& chapter, const std::string& key) {
	if (data_.count(chapter) == 0) {
		std::map<std::string, std::string> temp;
		data_[chapter] = temp;
		IsEdit_ = true;
	}
	if (data_.at(chapter).count(key) == 0) {
		data_[chapter][key] = "";
		IsEdit_ = true;
	}
	return data_.at(chapter).at(key);
}

bool INI::GetValueBool(const std::string& chapter, const std::string& key) {
	return GetValue(chapter, key) == "true";
}

int INI::GetValueInt(const std::string& chapter, const std::string& key) {
	std::string result_str = GetValue(chapter, key);
	int result_int = 0;
	try {
		result_int = stoi(result_str);
	}
	catch(...) {
		result_int = 0;
		SetValue(chapter, key, "0");
		IsEdit_ = true;
	}
	return result_int;
}

void INI::SetValue(const std::string& chapter, const std::string& key, const std::string& value) {
	data_[chapter][ClearLine(key, ' ')] = ClearLine(value, ' ');
	IsEdit_ = true;
	SaveFile();
}

const std::map<std::string, std::string>& INI::GetValueMap(const std::string& chapter) {
	if (data_.count(chapter) == 0) {
		data_[chapter];
		IsEdit_ = true;
	}
	return data_.at(chapter);
}


//std::string filename_;
//std::map<std::string, std::map<std::string, std::string>> data_;
