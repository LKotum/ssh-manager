#pragma once

#ifdef _WIN32
	#define WINDOWS true
#else
	#define WINDOWS false
#endif

#include <map>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>

#if _WIN32
	#include <stdlib.h>
#else
	#include <cstdlib>
#endif

typedef std::map<int, std::string> Lists;

class HostsParser
{
	private:
		Lists _listHosts;
		Lists _listUsers;
		Lists _listPorts;
		Lists _listNames;
		std::fstream _HostsFile;
		#if WINDOWS
			std::string _hosts_path = "\\.ssh\\hosts.ini";
			size_t size;
			char *path;
			_dupenv_s(&path, &size, "USERPROFILE")
			std::string _Path = path + _hosts_path;
		#else
			std::string _hosts_path = "/.ssh/hosts.ini";
			std::string _Path = getenv("HOME") + _hosts_path;
		#endif
		bool _fileExist(std::string);
		bool _readHosts();
		void _updateHosts();
		void _saveHosts();

	public:
		HostsParser();

		void addHost(std::string user, std::string host, std::string port);
		bool removeHost(int idndexHost);
		std::map<int, std::string> getListHosts();
		std::map<std::string, std::string> getHost(int index);
		bool hostExicst(int index);
};
