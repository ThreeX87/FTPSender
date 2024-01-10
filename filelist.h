#pragma once

#include <filesystem>
#include <vector>

#include "loger.h"
#include "common.h"

namespace fs = std::filesystem;

class FileListMaker {
public:
	explicit FileListMaker(const fs::path& parent_path, Loger& log);
	
	//рекурсивный метод для получения списка файлов во всех вложенных папках
	void MakeFileList(const fs::path& parent_path);
	
	//получение списка файлов
	const std::vector<fs::path>& GetFileList() const;
	
	const bool IsEmpty() const;
	
private:
	std::vector<fs::path> filelist_;
	Loger& log_;
};