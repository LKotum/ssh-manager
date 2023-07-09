#pragma once

#ifdef _WIN64
	#define FLAG true
#else
	#define FLAG false
#endif

#include <map>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>

typedef std::map<int, std::string> Lists;

class HostsParser
{
	private:
		Lists _listHosts;
		Lists _listUsers;
		Lists _listPorts;
		Lists _listNames;
		std::fstream _HostsFile;
		std::string _Path = FLAG ? std::filesystem::current_path().string() + "\\hosts.ini" : "./hosts.ini";
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
