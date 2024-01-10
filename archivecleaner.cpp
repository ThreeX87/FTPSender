#include "archivecleaner.h"

ArchiveCleaner::ArchiveCleaner(const fs::path& parent_path, int keep_archive)
	: path_(parent_path)
	, keep_archive_(keep_archive) {
}

void ArchiveCleaner::ClearArchive() const {
	if (!fs::is_directory(path_)) {
		return;
	}
	for (const auto& dir : fs::directory_iterator(path_)) {
		auto folder_time = fs::last_write_time(dir);
		if (CheckDate(folder_time)) {
			int i = 0;
			fs::remove_all(dir);
			std::cout << "---" << dir.path().string() << " - deleted" << '\n';
		}
	}
}

const bool ArchiveCleaner::CheckDate(const fs::file_time_type& file_time) const {

	auto today = std::chrono::system_clock::now();
	auto today_t = to_time_t(today);
	
	auto file_t = to_time_t(file_time);
	
	auto delta = (today_t - file_t)  / 60 / 60 / 24;
	
	return delta > keep_archive_;
}
