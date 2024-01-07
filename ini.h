#pragma once

#include <fstream>
#include <string>
#include <map>

#include "common.h"

class INI {
public:
	explicit INI(const std::string& filename);
	~INI();
	//работа с файлом
	void ReadFile();
	void SaveFile();
	
	//обработка запросов
	const std::string& GetValue(const std::string& chapter, const std::string& key);
	int GetValueInt(const std::string& chapter, const std::string& key);
	bool GetValueBool(const std::string& chapter, const std::string& key);
	void SetValue(const std::string& chapter, const std::string& key, const std::string& value);
	
	const std::map<std::string, std::string>& GetValueMap(const std::string& chapter);
	
	const std::map<std::string, std::map<std::string, std::string>>& GetAllData() const;
	
private:
	std::string filename_;
	std::map<std::string, std::map<std::string, std::string>> data_;
	bool IsEdit_ = false;
};