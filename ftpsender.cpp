#include "ftpsender.h"
#include <sys/stat.h>
#include <chrono>
#include <sstream>
#include <iostream>

using namespace std;

string FtpSender::ChangeCharInString(const string& text, char original, char changed) {
	if (text.find(original) != string::npos) {
		string result;
		for (const auto& c : text) {
			result.push_back(c == original? changed : c);
		}
		return result;
	}
	return text;
}

string AddEndCharToString(const string& text, char c) {
	if (text.back() == c) {
		return text;
	}
	string temp(text);
	temp.push_back(c);
	return temp;
}

string DelEndCharOnString(const string& text, char c) {
	if (text.back() != c) {
		return text;
	}
	string temp(text);
	temp.pop_back();
	return temp;
}

void MoveFile(const fs::path& from, const fs::path& to) {
	fs::create_directories(to.parent_path());
	fs::copy(from, to);
	fs::remove(from);
}

string FtpSender::InfoProgram() {
	string result = "--------------------Settings--------------------\n";
	result += "Server:\t\t\t" + AddEndCharToString(config_.GetValue("FTP", "server"), '\\') + "\n";
	result += "Path to sync:\t\t" + AddEndCharToString(config_.GetValue("local", "sync_path"), '\\') + "\n";
	result += "Archive path:\t\t" + AddEndCharToString(config_.GetValue("local", "archive_path"), '\\') + "\n";
	if(config_.GetValueBool("local", "check_ext")) {
		result += "Extensions to send:\n";
		const auto& extensions = config_.GetNode("extensions");
		if (extensions.IsChapter()) {
			for (const auto& ext : extensions.AsChapter().second) {
				if (ext.IsSection()) {
					result += "\t\t\t*." + ext.AsSection().second + "\n";
				}
			}
		}
	}
	result += "------------------------------------------------\n";
	return result;
}

FtpSender::FtpSender(Ini::Ini& config)
	: config_(config) {
	cout << InfoProgram();
	curl_global_init(CURL_GLOBAL_ALL);
	curlup = curl_easy_init();
	
	if(curlup == NULL) {
		cout << "!!! ERROR !!! ERROR !!! ERROR !!!\n";
	}
	else {
		string user = config_.GetValue("FTP", "user");
		string password = config_.GetValue("FTP", "password");
		string ftp_url = AddEndCharToString(ChangeCharInString(config_.GetValue("FTP", "server"), '\\', '/'), '/');
		int timeout = config_.GetValueInt("FTP", "server_timeout");
		
		curl_easy_setopt(curlup, CURLOPT_URL, ftp_url.c_str());
		curl_easy_setopt(curlup, CURLOPT_USERNAME, user.c_str());
		curl_easy_setopt(curlup, CURLOPT_PASSWORD, password.c_str());
		if (timeout) {
			curl_easy_setopt(curlup, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);
		}
	}
}

FtpSender::~FtpSender() {
	curl_easy_cleanup(curlup);
	curl_global_cleanup();
}

static size_t ReadFunc(void *ptr, size_t size, size_t nmemb, void *stream) {
	size_t n;
	FILE *f = (FILE*)stream;
	if (ferror(f)) {
		return CURL_READFUNC_ABORT;
	}		
	n = fread(ptr, size, nmemb, f) * size;
	return n;
}

bool FtpSender::CheckExtension(const fs::path& file) {
	if (!config_.GetValueBool("local", "check_ext")) {
		return true;
	}
	string file_extension = file.extension().empty() ? file.stem().string() : file.extension().string();
	const auto& extensions = config_.GetNode("extensions");
	if (extensions.IsChapter()) {
		for (const auto& ext : extensions.AsChapter().second) {
			if (ext.IsSection() && file_extension == ("." + ext.AsSection().second)) {
				return true;
			}
		}
	}
	return false;
}

int FtpSender::SendToFtp() { //const string& task) {
	if(curlup == NULL) {
		return -1;
	}
	
	fs::path archive_file = AddEndCharToString(ChangeCharInString(config_.GetValue("local", "archive_path"), '/', '\\'), '\\');
	string ftp_url = AddEndCharToString(ChangeCharInString(config_.GetValue("FTP", "server"), '\\', '/'), '/');
	string dest_path = AddEndCharToString(ChangeCharInString(config_.GetValue("local", "dest_path"), '\\', '/'), '/');
	fs::path rep_file = ChangeCharInString(config_.GetValue("local", "rep_file"), '/', '\\');
	string spr_file = AddEndCharToString(ChangeCharInString(config_.GetValue("local", "spr_file"), '\\', '/'), '/');
	string file_mask = config_.GetValue("local", "file_mask");
	int max_files = config_.GetValueInt("local", "max_files");
	string sync_path = DelEndCharOnString(ChangeCharInString(config_.GetValue("local", "sync_path"), '/', '\\'), '\\');
	
	FILE *fd;

	struct stat file_info;
	curl_off_t fsize;
	int file_count = 0;
	
	
	/*if (fs::exists()rep_file ) {
		fd = fopen(localpath_file.string().c_str(), "rb");
	}*/
	if (sync_path != "")
	{
		FileListMaker filelist(sync_path);

		for (const auto& localpath_file : filelist.GetFileList())
		{
			if (!CheckExtension(localpath_file)) {
				continue;
			}
			if(stat(localpath_file.string().c_str(), &file_info)) {
				int i = 1;
				while(i > 0) {
					fs::path temp_file = localpath_file.parent_path() / ("temp" + to_string(i) + localpath_file.extension().string());
					if (!fs::exists(temp_file)) {
						fs::rename(localpath_file, temp_file);
						break;
					}
					++i;
				}
				continue;
			}
			fsize = (curl_off_t)file_info.st_size;
			
			fd = fopen(localpath_file.string().c_str(), "rb");
			if (fd == NULL) {
				int i = 1;
				while(i > 0) {
					fs::path temp_file = localpath_file.parent_path() / ("temp" + to_string(i) + localpath_file.extension().string());
					if (!fs::exists(temp_file)) {
						fs::rename(localpath_file, temp_file);
						break;
					}
					++i;
				}
				continue;
			}
			
			auto file_time = fs::last_write_time(localpath_file);
			time_t tt = to_time_t(file_time);
		    tm *gmt = gmtime(&tt);
		    
		    stringstream subdir;
		    
		    subdir << gmt->tm_year + 1900 << "_" 
				<< (gmt->tm_mon + 1 < 10? "0" : "") << gmt->tm_mon + 1 << "_"
				<< (gmt->tm_mday < 10? "0" : "") << gmt->tm_mday << "/" ;
		    string destin_file;
		    fs::path new_location;
		    
		    for (int i = 1; i < max_files; ++i) {
		    	new_location = subdir.str();
		    	if (localpath_file.extension().empty()) {
					new_location = new_location / (file_mask + to_string(i) + localpath_file.stem().string());
				}
				else {
					new_location = new_location / (file_mask + to_string(i) + localpath_file.extension().string());
				}
		    	destin_file = ftp_url + dest_path + new_location.string();
		    	cout << destin_file;
				curl_easy_setopt(curlup, CURLOPT_URL, destin_file.c_str());
				curl_easy_setopt(curlup, CURLOPT_NOBODY, 1L);
		    	CURLcode r = curl_easy_perform(curlup);
		    	
		    	if (r != CURLE_OK) {
					break;
		    	}
			}
	
		    curl_easy_setopt(curlup, CURLOPT_FTP_CREATE_MISSING_DIRS, (long)CURLFTP_CREATE_DIR_RETRY);
			curl_easy_setopt(curlup, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curlup, CURLOPT_READFUNCTION, ReadFunc);
			curl_easy_setopt(curlup, CURLOPT_READDATA, fd);
			curl_easy_setopt(curlup, CURLOPT_INFILESIZE_LARGE,fsize);
			
			CURLcode r = curl_easy_perform(curlup);
			fclose(fd);
			
			if (r != CURLE_OK) {
				continue;
			}
		    ++file_count;
			MoveFile(localpath_file, archive_file / new_location);
		}                         
	}

	return file_count;
}