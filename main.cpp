#include <winsock2.h>
#include <windows.h>


#include "ftpsender.h"
#include "archivecleaner.h"
 
using namespace std;

int main(int argc, char** argv)
{

	fs::path setting_file = (fs::path(argv[0])).parent_path() / "settings.ini";
	ifstream ifs(setting_file);
	Ini::Ini settings(ifs);
	ifs.close();
	bool config_is_edit = false;

	int keep_archive = settings.GetValueInt("local", "keep_archive");
	if (keep_archive == 0) {
		keep_archive = 30;
		settings.SetValue("local", "keep_archive", "30");
		config_is_edit = true;
	}
	
	ArchiveCleaner cleaner_archive(settings.GetValue("local", "archive_path"), keep_archive);
	FtpSender sender(settings);
	int repeat = settings.GetValueInt("local", "repeat") * 60000;
	if (repeat == 0) {
		repeat = 5 * 60000;
		settings.SetValue("local", "repeat", "5");
		config_is_edit = true;
	}
	
	bool exit = false;
	while(!exit) {
		cout << ">> Start program\n";
		
		int sended = 0;
		//for (const auto& task : tasks) {
			sended += sender.SendToFtp();
		//}
		
		cleaner_archive.ClearArchive();
		
		if (settings.IsEdited()) {
			ofstream ofs(setting_file);
			settings.SaveIni(ofs);
		}
		
		cout << ">> " << sended << " files sended!\n<< Stop program\n";
		Sleep(repeat);
	}
	
	return 0;
}