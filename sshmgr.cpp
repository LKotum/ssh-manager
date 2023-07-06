#ifndef SSHMGR_CPP
#define SSHMGR_CPP

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <optional>
#include <unordered_map>
#include <typeinfo>
#include <map>
#include "libs/hostspsr/include/hostspsr.h"

using namespace std;

const char *logo =
	"\033[33m  ______  \033[32m  ______  \033[34m ____  ____ \033[0m\n"
	"\033[33m.' ____ \\ \033[32m.' ____ \\ \033[34m|_   ||   _|\033[0m\n"
	"\033[33m| (___ \\_|\033[32m| (___ \\_|\033[34m  | |__| |  \033[0m\n"
	"\033[33m _.____`. \033[32m _.____`. \033[34m  |  __  |  \033[0m\n"
	"\033[33m| \\____) |\033[32m| \\____) |\033[34m _| |  | |_ \033[0m\n"
	"\033[33m \\______.'\033[32m \\______.'\033[34m|____||____|\033[0m\n"
	"\n"
	"SSH manager (\033[31mv1.0.0\033[0m)\n"
	"By \033[35mMaxLane\033[0m and \033[35mBadWolf\033[0m\n"
	"\n"
	"Feedback:\n"
	"	\033[32mhttps://wolfinfo.ru\033[0m\n"
	"	\033[32mhttps://lkotum.ru\033[0m  \n";

struct sshmngr
{
	bool help{false};
	bool version{false};
	bool newconnect{false};
	bool connect{false};
	bool remove{false};
	bool list{false};
	bool main{true};
	optional<int> host;
};


typedef function<void(sshmngr &)> NoArgHandle;
typedef function<void(sshmngr &, const string &)> OneArgHandle;

const unordered_map<string, NoArgHandle> NoArgs
{
	{"--help", [](sshmngr &s){ s.help = true; s.main = false; }},
	{"-h", [](sshmngr &s){ s.help = true; s.main = false; }},
	{"--version", [](sshmngr &s){ s.version = true; s.main = false; }},
	{"-v", [](sshmngr &s){ s.version = true; s.main = false; }},
	{"--list", [](sshmngr &s){ s.list = true; s.main = false; }},
	{"-l", [](sshmngr &s){ s.list = true; s.main = false; }},
	{"--new", [](sshmngr &s){ s.newconnect = true; s.main = false; }},
	{"-n", [](sshmngr &s){ s.newconnect = true; s.main = false; }},
	{"-nc", [](sshmngr &s){ s.newconnect = true; s.connect = true; s.main = false;}},
	{"--new-connect", [](sshmngr &s){ s.newconnect = true; s.connect = true; s.main = false;}},
};

bool check(sshmngr &s, const string &arg)
{
	try
	{
        stoi(arg);
    }
	catch (...)
	{
		cout << "\033[31m[ERROR]\033[0m Invalid type value after argument" << endl;
        return false;
    }
	s.host = stoi(arg);
	return true;
};

bool check(const string arg, int &value)
{
	try
	{
        value = stoi(arg);
    }
	catch (...)
	{
		cout << "\033[31m[ERROR]\033[0m Invalid type value after argument" << endl;
		return false;
    }
	return true;
};

const unordered_map<string, OneArgHandle> OneArgs
{
	{"-c", [](sshmngr &s, const string &arg){ s.connect = check(s, arg); s.main = false; }},
	{"--connect", [](sshmngr &s, const string &arg){ s.connect = check(s, arg); s.main = false; }},
	{"-r", [](sshmngr &s, const string &arg){ s.remove = check(s, arg); s.main = false; }},
	{"--remove", [](sshmngr &s, const string &arg){ s.remove = check(s, arg); s.main = false; }},
};

string command(string user, string host, string port)
{
	return "ssh " + user + "@" + host + " -p " + port;
};

void printList(HostsParser &hosts)
{
	map<int, string> listhosts = hosts.getListHosts();

	for (auto iter2{listhosts.begin()}; iter2 != listhosts.end(); iter2++)
	{
		cout << "[" << iter2->first << "]" << "\t" << iter2->second << std::endl;
	}
};

void printLogo()
{
	cout << logo << endl;
};

void printVer()
{
	printLogo();
};

string request(bool submenu = false){
	string answer;
	if (submenu)
		cout << "	";
	cout << "\033[34m->\033[0m ";
	cin >> answer;
	return answer;
};

void printFlagsHelp()
{
	const char *Help=
	"SSHManager FlagsHelpList v1.0\n"
	"\n"
	"\033[33m-h\033[0m,  \033[33m--help\033[0m                                Get this help\n"
	"\033[33m-v\033[0m,  \033[33m--version\033[0m                             Get information about program\n"
	"\033[33m-l\033[0m,  \033[33m--list\033[0m                                Get list of hosts\n"
	"\033[33m-n\033[0m,  \033[33m--new\033[0m                                 Used to save new connections\n"
	"\033[33m-nc\033[0m, \033[33m--new-connect\033[0m                         To save and connect quickly\n"
	"\033[33m-c\033[0m,  \033[33m--connect\033[0m + \033[35m(argument (digital))\033[0m      Used for connect to host\n"
	"\033[33m-r\033[0m,  \033[33m--remove\033[0m + \033[35m(argument (digital))\033[0m       Used to remove host preset\n";
	cout << Help << endl;
};

void printHelp(){
	const char *Help=
	"SSHManager Help v1.0\n"
	"\n"
	"h, help                              Get this help\n"
	"l, list                              Get saved hosts templates\n"
	"n, new    	                          Used to create new connections\n"
	"c, connect + (argument (digital))    Used for connect to host\n"
	"r, remove + (argument (digital))     Used to remove host preset\n"
	"q, quit, exit                        Used to terminate the program\n";
	cout << Help << endl;
};

void addHost(HostsParser &hosts, bool connect = false)
{
	string ip, port, login;
	cout << "\033[32m[INFO]\033[0m Write ip/host address\n";
	ip = request(true);
	cout << "\033[32m[INFO]\033[0m Write port\n";
	port = request(true);
	cout << "\033[32m[INFO]\033[0m Write our login\n";
	login = request(true);
	hosts.addHost(login, ip, port);
	if(connect){
		system(command(login, ip, port).c_str());
		return;
	}
	cout << "\033[32m[INFO]\033[0m Try connect? \033[33m(\033[4my\033[0m\033[33m/n)\033[0m\n";
	string answer;
	answer = request(true);
	if (answer == "y"){
		system(command(login, ip, port).c_str());

	}
	else if (answer == "n"){
		return;
	}
	else {
		command(login, ip, port);
	}
};

void removeHost(HostsParser &hosts, int index)
{
	if (!hosts.hostExicst(index)){
		cout << "\033[33m[WARN]\033[0m Could not find this host!\n";
		return;
	}
	cout << "\033[32m[INFO]\033[0mAre you sure? \033[33m(yes/no)\033[0m\n";
	string answer;
	answer = request(true);

	while((answer != "yes") && (answer != "no")){
		cout << "\033[31m[WARN]\033[0m Please, write [yes] or [no]\n";
		answer = request(true);
		cout << ":" << answer << endl;
	}

	if(answer == "yes"){
		bool result = hosts.removeHost(index);
		if (result){
			cout << "\033[32m[INFO]\033[0m Successfully!\n";
		}
		else{
			cout << "\033[31m[WARN]\033[0m Failed!\n";
		}
	}
	else{
		cout << "\033[32m[INFO]\033[0m Cancelled!\n";
		return;
	}
};

void connectHost(map<string, string> &host)
{
	system(command(host["user"], host["host"], host["port"]).c_str());
};

int commander(string comm){
	if ((comm == "connect") || (comm == "c")){
		return 2;
	}
	else if ((comm == "new") || (comm == "n")){
		return 3;
	}
	else if ((comm == "remove") || (comm == "r")){
		return 4;
	}
	else if ((comm == "help") || (comm == "h")){
		return 5;
	}
	else if ((comm == "list") || (comm == "l")){
		return 6;
	}
	// 7 is path
	else if ((comm == "quit") || (comm == "q") || (comm == "exit") || (comm == "quit()")){
		return 8;
	}

	return 1;
};

int main(int argc, char* argv[])
{
	sshmngr args;
	for (int i{1}; i < argc; i++)
	{
		string opt{argv[i]};
		if (auto j{NoArgs.find(opt)}; j != NoArgs.end()) j->second(args);
		else if (auto k{OneArgs.find(opt)}; k != OneArgs.end())
			if (++i < argc)
				k->second(args, {argv[i]});
			else
			{
				cerr << "\033[31m[ERROR]\033[0m Missing param after: " << opt << endl;
				return 0;
			}
		else
		{
			cerr << "\033[31m[ERROR]\033[0m Unrecognized command-line option: " << opt << endl;
			return 0;
		}
	}

	string path = "./hosts";
	HostsParser hosts = HostsParser(path);

	if (args.help){
		printFlagsHelp();
	}

	if((args.connect) && (!args.newconnect)){
		if (hosts.hostExicst(args.host.value())){
			map<string, string> host = hosts.getHost(args.host.value());
			connectHost(host);
		}
		else{
			cout << "\033[31m[WARN]\033[0m Could not find this host!\n";
		}
	}

	if (args.list){
		printList(hosts);
	}

	if(args.newconnect){
		addHost(hosts, args.connect);
	}

	if (args.remove){
		removeHost(hosts, args.host.value());
	}

	if (args.version){
		printVer();
	}

	if (!args.main){
		return 0;
	}

	printLogo();
	cout << "Welcome!\nYor hosts:\n";
	printList(hosts);
	cout <<"\nType \033[33m[h]\033[0m or \033[33m[help]\033[0m for help\n";
	while (args.main){
		string answer;
		answer = request();
		switch (commander(answer)){
			case 1:
				cout << "\033[31m[WARN]\033[0m Wrong argument! Type \033[33m[help]\033[0m to see all commands\n";
				break;
			case 2:
				{
					cout << "\033[32m[INFO]\033[0m Write number of connection\n";
					answer = request(true);
					int q;
					if(check(answer, q)){
						if (hosts.hostExicst(q)){
							map<string, string> host = hosts.getHost(q);
							connectHost(host);
						}
						else{
							cout << "\033[31m[WARN]\033[0m Could not find this host!\n";
						}
					}
				}
				break;
			case 3:
				addHost(hosts);
				break;
			case 4:
				{
					cout << "[INFO] Write number of connection\n";
					answer = request(true);
					int q;
					if(check(answer, q))
						removeHost(hosts, q);
				}
				break;
			case 5:
				printHelp();
				break;
			case 6:
				printList(hosts);
				break;
			case 7:
				cout << "" << endl;
				break;
			case 8:
				return 0;
				break;
			default:
				cout << "[WARN] Wrong argument! Type [help] to see all commands\n";
				break;
		}
	}

	return 0;
}

// TODO (For roadmap), realize autoreconnect with flags -n-a, -n-c-a, -c-a and in main program

#endif