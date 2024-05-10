#ifdef _WIN32
	#define WINDOWS true
#elif _WIN64
	#define WINDOWS true
#else
	#define WINDOWS false
#endif

#include "include/hostspsr.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <map>

#if WINDOWS
	#include <stdlib.h>
#else
	#include <cstdlib>
#endif

typedef std::map< int, std::string> Lists;

HostsParser::HostsParser()
{
	#if WINDOWS
		size_t size;
		char* path;
		_dupenv_s(&path, &size, "USERPROFILE");
		_Path = path + _hosts_path;
	#else
		_Path = getenv("HOME") + _hosts_path;
	#endif
	if (_fileExist(_Path))
	{
		_HostsFile.open(_Path);
	}
	else
	{
		_HostsFile.open(_Path, std::ofstream::out | std::ofstream::trunc);
		_HostsFile.close();
		_HostsFile.open(_Path);
	}
	bool result = _readHosts();
	if (!result) {
		_HostsFile.close();
		this->~HostsParser();
	}
	_HostsFile.close();
}

void HostsParser::addHost(std::string user, std::string host, std::string port)
{
	int index = _listNames.size();
	_listNames.insert({index, host+"("+user+")"});
	_listHosts.insert({index, host});
	_listUsers.insert({index, user});
	_listPorts.insert({index, port});
	_saveHosts();
}

bool HostsParser::hostExicst(int index)
{
	Lists::const_iterator it = _listNames.find(index);
	return it!=_listNames.end();
}

bool HostsParser::removeHost(int indexHost)
{
	int check = _listNames.erase(indexHost);
	if (check == 0)
	{
		return false;
	}
	_listHosts.erase(indexHost);
	_listUsers.erase(indexHost);
	_listPorts.erase(indexHost);

	Lists temp_listNames = _listNames;
	Lists temp_listHosts = _listHosts;
	Lists temp_listUsers = _listUsers;
	Lists temp_listPorts = _listPorts;

	_listNames.clear();
	_listHosts.clear();
	_listUsers.clear();
	_listPorts.clear();

	for (int i=0; i < indexHost; i++)
    {
		_listNames[i] = temp_listNames[i];
		_listHosts[i] = temp_listHosts[i];
		_listUsers[i] = temp_listUsers[i];
		_listPorts[i] = temp_listPorts[i];
    }

	for (int i=indexHost; i < temp_listNames.size(); i++)
    {
		_listNames[i] = temp_listNames[i+1];
		_listHosts[i] = temp_listHosts[i+1];
		_listUsers[i] = temp_listUsers[i+1];
		_listPorts[i] = temp_listPorts[i+1];
    }
	_saveHosts();
	return true;
}

bool HostsParser::_fileExist(std::string path)
{
	return bool(std::ifstream(path));
}

std::map<int, std::string> HostsParser::getListHosts()
{
	return _listNames;
}

std::map<std::string, std::string> HostsParser::getHost(int index)
{
	std::map<std::string, std::string> H;
	H.insert({"host", _listHosts[index]});
	H.insert({"user", _listUsers[index]});
	H.insert({"port", _listPorts[index]});
	return H;
}

void HostsParser::_saveHosts()
{
	_HostsFile.open(_Path, std::ofstream::out | std::ofstream::trunc);
	_HostsFile.close();
	_HostsFile.open(_Path, std::ofstream::out);
	for (int i=0; i < _listNames.size();i++)
    {
		_HostsFile << "[" << _listNames[i] << "]" << std::endl;
		_HostsFile << "host = " << _listHosts[i] << std::endl;
		_HostsFile << "user = " << _listUsers[i] << std::endl;
		_HostsFile << "port = " << _listPorts[i] << std::endl;
		_HostsFile << std::endl;
    }
	_HostsFile.close();
}

bool HostsParser::_readHosts()
{
	std::string line;
	std::string sectionName;
	std::string optionName;
	std::string rawHostContent;

	size_t lineNbr = 1;

	int index = -1;

	while(std::getline(_HostsFile, line))
	{
		line.erase(std::remove_if( line.begin(), line.end(),
		[l = std::locale {}](auto ch) { return std::isspace(ch, l); }), line.end());

		if (line.empty())
		{
			lineNbr++;
			continue;
		}

		if (line.at(0) == ';')
		{
			lineNbr++;
			continue;
		}

		if(line.at(0) == '[' && line.at(line.length()-1) == ']')
		{
			sectionName = line.substr(1, line.length()-2);
			index++;
			_listNames.insert({index, sectionName});
			continue;
		}

		size_t equalSignPos = line.find('=');
		if(equalSignPos != std::string::npos)
		{
			optionName = line.substr(0, equalSignPos);
			rawHostContent = line.substr(equalSignPos+1, line.length()-1);
			if (optionName == "host"){
				_listHosts.insert({index, rawHostContent});
			}
			else if (optionName == "user"){
				_listUsers.insert({index, rawHostContent});
			}
			else if (optionName == "port"){
				_listPorts.insert({index, rawHostContent});
			}
		}
		else
		{
			std::string errorMessage = "[ERROR]" + _Path + ":" + std::to_string(lineNbr) + ": parsing error \n" + line;
			return false;
		}
		lineNbr++;
	}
	return true;
}