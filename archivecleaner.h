#pragma once

#include <filesystem>
#include <chrono>
#include <ctime>

#include "loger.h"
#include "filelist.h"
#include "common.h"

namespace fs = std::filesystem;

class ArchiveCleaner {
public:
	explicit ArchiveCleaner(const fs::path& parent_path, int keep_archive);
	
	void ClearArchive() const;
private:
	const fs::path path_;
	int keep_archive_;
	
	const bool CheckDate(const fs::file_time_type& file_time) const;
};