#pragma once

#include <ctime>
#include <chrono>
#include <string>

enum LOG_STATUS {
	ER,
	OK
};

template <typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
              + system_clock::now());
    return system_clock::to_time_t(sctp);
}

struct ServerSettings {
	std::string server;
	std::string user;
	std::string password;
	int timeout = 0;
};
