#include "filelist.h"


FileListMaker::FileListMaker(const fs::path& parent_path, Loger& log)
	: log_(log) {
	MakeFileList(parent_path);
}

void FileListMaker::MakeFileList(const fs::path& path) {
	if (!exists(path)) {
		return;
	}
	else if (!is_directory(path)) {
		filelist_.push_back(path);
		return;
	}
	try {
		for (const auto& dir : fs::directory_iterator(path)) {
			MakeFileList(dir.path());
		}
	}
	catch(...) {
		log_.SaveEventToLog(path.string() + " NOT OPENED", LOG_STATUS::ER);
	}
}

const std::vector<fs::path>& FileListMaker::GetFileList() const {
	return filelist_;
}

const bool FileListMaker::IsEmpty() const {
	return filelist_.empty();
}