#include <winsock2.h>
#include <windows.h>


#include "ftpsender.h"
#include "archivecleaner.h"
 
using namespace std;

int main(int argc, char** argv)
{
	fs::path setting_file = (fs::path(argv[0])).parent_path() / "settings.ini";
	INI settings(setting_file.string());
	Loger log;
	
	int log_level = settings.GetValueInt("log", "log_level");
	log.SetLogParameter(settings.GetValue("log", "log_file"), log_level);
	
	int keep_archive = settings.GetValueInt("local", "keep_archive");
	if (keep_archive == 0) {
		keep_archive = 30;
	}
	
	ArchiveCleaner cleaner_archive(settings.GetValue("local", "archive_path"), keep_archive);
	FtpSender sender(settings, log);
	int repeat = settings.GetValueInt("local", "repeat") * 60000;
	if (repeat == 0) {
		repeat = 5 * 60000;
	}
	
	map<string, string> tasks = settings.GetValueMap("tasks");
	
	bool exit = false;
	while(!exit) {
		cout << ">> Start program\n";
		
		int sended = 0;
		for (const auto& task : tasks) {
			sended += sender.SendToFtp(task.second);
		}
		
		cleaner_archive.ClearArchive();
		
		cout << ">> " << sended << " files sended!\n<< Stop program\n";
		Sleep(repeat);
	}
	
	return 0;
}