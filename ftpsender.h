#pragma once

#include "ini.h"
#include "filelist.h"
//#include "loger.h"
#include "common.h"

#include <iostream>
#include <curl/curl.h>

class FtpSender {
public:
	explicit FtpSender(Ini::Ini& config);
	~FtpSender();
	
	int SendToFtp(); //const std::string& task);
private:
	Ini::Ini& config_;
	CURL *curlup = NULL;
	
	bool CheckExtension(const fs::path& file);
	std::string InfoProgram();
	std::string ChangeCharInString(const std::string& text, char original, char changed);
};

