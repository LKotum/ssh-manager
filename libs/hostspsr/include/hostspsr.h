#pragma once

#include <map>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

typedef std::map< int, std::string> Lists;

class HostsParser
{
	private:
		Lists _listHosts;
		Lists _listUsers;
		Lists _listPorts;
		Lists _listNames;
		std::fstream _HostsFile;
		std::string _defaultPath = "./hosts.ini";
		std::string _path = _defaultPath;
		bool _fileExist(std::string);
		bool _pathExist(std::string);
		bool _readHosts();
		void _updateHosts();
		void _saveHosts();

	public:
		HostsParser();
		HostsParser(std::string path);

		void addHost(std::string user, std::string host, std::string port);
		bool removeHost(int idndexHost);
		std::map<int, std::string> getListHosts();
		std::map<std::string, std::string> getHost(int index);
		bool hostExicst(int index);
};
