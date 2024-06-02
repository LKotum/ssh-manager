#ifdef _WIN32
	#define WINDOWS true
#elif _WIN64
	#define WINDOWS true
#else
	#define WINDOWS false
#endif

#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <optional>
#include <unordered_map>
#include <typeinfo>
#include <map>
#include "libs/hostspsr/include/hostspsr.h"

const int REPEAT = 5;

const char *logo =
	"\033[33m  ______  \033[32m  ______  \033[34m ____  ____ \033[0m\n"
	"\033[33m.' ____ \\ \033[32m.' ____ \\ \033[34m|_   ||   _|\033[0m\n"
	"\033[33m| (___ \\_|\033[32m| (___ \\_|\033[34m  | |__| |  \033[0m\n"
	"\033[33m _.____`. \033[32m _.____`. \033[34m  |  __  |  \033[0m\n"
	"\033[33m| \\____) |\033[32m| \\____) |\033[34m _| |  | |_ \033[0m\n"
	"\033[33m \\______.'\033[32m \\______.'\033[34m|____||____|\033[0m\n"
	"\n"
	"SSH manager (\033[31mv1.2.0-BETA2\033[0m)\n"
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
	bool reconnect{false};
	bool main{true};
	std::optional<int> host;
};

typedef std::function<void(sshmngr &)> NoArgHandle;
typedef std::function<void(sshmngr &, const std::string &)> OneArgHandle;

const std::unordered_map<std::string, NoArgHandle> NoArgs
{
	{"--help", [](sshmngr &s){ s.help = true; s.main = false; }},
	{"-h", [](sshmngr &s){ s.help = true; s.main = false; }},
	{"--version", [](sshmngr &s){ s.version = true; s.main = false; }},
	{"-v", [](sshmngr &s){ s.version = true; s.main = false; }},
	{"--list", [](sshmngr &s){ s.list = true; s.main = false; }},
	{"-l", [](sshmngr &s){ s.list = true; s.main = false; }},
	{"--new", [](sshmngr &s){ s.newconnect = true; s.main = false; }},
	{"-n", [](sshmngr &s){ s.newconnect = true; s.main = false; }},
	{"--new-connect", [](sshmngr &s){ s.newconnect = true; s.connect = true; s.main = false;}},
	{"-N", [](sshmngr &s){ s.newconnect = true; s.connect = true; s.main = false;}},
};

bool check(sshmngr &s, const std::string &arg)
{
	try
	{
        s.host = std::stoi(arg);
    }
	catch (...)
	{
		std::cout << "\033[31m[ERROR]\033[0m Invalid type value after argument" << std::endl;
        return false;
    }
	return true;
};

bool check(const std::string arg, int &value)
{
	try
	{
        value = std::stoi(arg);
    }
	catch (...)
	{
		std::cout << "\033[31m[ERROR]\033[0m Invalid type value after argument" << std::endl;
		return false;
    }
	return true;
};

const std::unordered_map<std::string, OneArgHandle> OneArgs
{
	{"-c", [](sshmngr &s, const std::string &arg){ s.connect = check(s, arg); s.main = false; }},
	{"--connect", [](sshmngr &s, const std::string &arg){ s.connect = check(s, arg); s.main = false; }},
	{"-r", [](sshmngr &s, const std::string &arg){ s.remove = check(s, arg); s.main = false; }},
	{"--remove", [](sshmngr &s, const std::string &arg){ s.remove = check(s, arg); s.main = false; }},
	{"-L", [](sshmngr &s, const std::string &arg){ s.connect = check(s, arg); s.main = false; s.reconnect = true;}},
	{"--loop", [](sshmngr &s, const std::string &arg){ s.connect = check(s, arg); s.main = false; s.reconnect = true;}}
};

std::string sshClient()
{
	#if WINDOWS
		return "ssh ";
	#else
		const char* term = getenv("TERM");
		return (term && term == std::string("xterm-kitty")) ? "kitten ssh " : "ssh ";
	#endif
};

std::string command(std::string user, std::string host, std::string port)
{
	return sshClient() + user + "@" + host + " -p " + port;
};

void printList(HostsParser &hosts)
{
	std::cout << "\033[33mYour hosts:\033[0m\n";
	std::map<int, std::string> listhosts = hosts.getListHosts();

	for (auto iter2{listhosts.begin()}; iter2 != listhosts.end(); iter2++)
	{
		std::cout << "[" << iter2->first << "]" << " - \t" << "\033[32m" << iter2->second << "\033[0m" << std::endl;
	}
};

void printLogo()
{
	std::cout << logo << std::endl;
};

void printVer()
{
	printLogo();
};

std::string request(){
	std::string answer;
	std::cout << "\033[34m->\033[0m ";
	std::cin >> answer;
	return answer;
};

void printFlagsHelp()
{
	const char *Help=
	"\033[33m-h\033[0m, \033[33m--help\033[0m                              Get this help\n"
	"\033[33m-v\033[0m, \033[33m--version\033[0m                           Get information about program\n"
	"\033[33m-l\033[0m, \033[33m--list\033[0m                              Get list of hosts\n"
	"\033[33m-n\033[0m, \033[33m--new\033[0m                               Used to save new connections\n"
	"\033[33m-N\033[0m, \033[33m--new-connect\033[0m                       To save and connect quickly\n"
	"\033[33m-c\033[0m, \033[33m--connect\033[0m + \033[35m(argument (digital))\033[0m    Used for connect to host\n"
	"\033[33m-r\033[0m, \033[33m--remove\033[0m + \033[35m(argument (digital))\033[0m     Used to remove host preset\n"
	"\033[33m-L\033[0m, \033[33m--loop\033[0m + \033[35m(argument (digital))\033[0m       Used for cyclic connection to the host\n";
	std::cout << Help << std::endl;
};

void printHelp(){
	const char *Help=
	"\033[33mh\033[0m,  \033[33mhelp\033[0m          Get this help\n"
	"\033[33ml\033[0m,  \033[33mlist\033[0m          Get saved hosts templates\n"
	"\033[33mn\033[0m,  \033[33mnew\033[0m           Used to create new connections\n"
	"\033[33mc\033[0m,  \033[33mconnect\033[0m       Used for connect to host\n"
	"\033[33mr\033[0m,  \033[33mremove\033[0m        Used to remove host preset\n"
	"\033[33mq\033[0m,  \033[33mquit\033[0m, \033[33mexit\033[0m    Used to terminate the program\n"
	"\033[33mrc\033[0m, \033[33mreconnect\033[0m     Used for cyclic connection to the host\n";
	std::cout << Help << std::endl;
};

void addHost(HostsParser &hosts, bool connect = false)
{
	std::string host, port, user;
	std::cout << "\033[32m[INFO]\033[0m Enter your host: ";
	std::cin >> host;
	std::cout << "\033[32m[INFO]\033[0m Enter your port: ";
	std::cin >> port;
	std::cout << "\033[32m[INFO]\033[0m Enter your username: ";
	std::cin >> user;
	hosts.addHost(user, host, port);
	if(connect){
		system(command(user, host, port).c_str());
		return;
	}
	std::cout << "\033[32m[INFO]\033[0m Try connect to " + host + " \033[33m(\033[4my\033[0m\033[33m/n)\033[0m: ";
	std::string answer;
	std::cin >> answer;
	if (answer == "y"){
		system(command(user, host, port).c_str());

	}
	else if (answer == "n"){
		return;
	}
	else {
		system(command(user, host, port).c_str());
	}
};

void removeHost(HostsParser &hosts, int index)
{
	if (!hosts.hostExicst(index)){
		std::cout << "\033[33m[WARN]\033[0m Could not find this host!\n";
		return;
	}
	std::cout << "\033[32m[INFO]\033[0m Are you sure? \033[33m(yes/no)\033[0m: ";
	std::string answer;
	std::cin >> answer;

	while((answer != "yes") && (answer != "no")){
		std::cout << "\033[31m[WARN]\033[0m Please, write \033[33m[yes]\033[0m or \033[33m[no]\033[0m: ";
		std::cin >> answer;
	}

	if(answer == "yes"){
		bool result = hosts.removeHost(index);
		if (result){
			std::cout << "\033[32m[INFO]\033[0m Successfully!\n";
		}
		else{
			std::cout << "\033[31m[WARN]\033[0m Failed!\n";
		}
	}
	else{
		std::cout << "\033[32m[INFO]\033[0m Cancelled!\n";
		return;
	}
};

void connectHost(std::map<std::string, std::string> &host, bool reconnect = false)
{
	system(command(host["user"], host["host"], host["port"]).c_str());
	if (!reconnect) return;
	int counter = 1;
	do{
		system(command(host["user"], host["host"], host["port"]).c_str());
		counter++;
		if (counter == REPEAT){
			std::cout << "\033[32m[INFO]\033[0m Continue reconnecting? \033[33m(\033[4my\033[0m\033[33m/n)\033[0m: ";
			std::string answer;
			std::cin >> answer;
			if (answer == "n") reconnect = false;
			counter = 0;
		}
	} while (reconnect);
};

int commander(std::string comm){
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
	else if ((comm == "rconnect") || (comm == "rc")){
		return 9;
	}

	return 1;
};

int main(int argc, char* argv[])
{
	sshmngr args;
	for (int i{1}; i < argc; i++)
	{
		std::string opt{argv[i]};
		if (auto j{NoArgs.find(opt)}; j != NoArgs.end()) j->second(args);
		else if (auto k{OneArgs.find(opt)}; k != OneArgs.end())
			if (++i < argc)
				k->second(args, {argv[i]});
			else
			{
				std::cerr << "\033[31m[ERROR]\033[0m Missing param after: " << opt << std::endl;
				return 0;
			}
		else
		{
			std::cerr << "\033[31m[ERROR]\033[0m Unrecognized command-line option: " << opt << std::endl;
			return 0;
		}
	}

	HostsParser hosts = HostsParser();

	if (args.help){
		printFlagsHelp();
	}

	if((args.connect) && (!args.newconnect)){
		if (hosts.hostExicst(args.host.value())){
			std::map<std::string, std::string> host = hosts.getHost(args.host.value());
			connectHost(host, args.reconnect);
		}
		else{
			std::cout << "\033[31m[WARN]\033[0m Could not find this host!\n";
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
	std::cout << "\033[32mWelcome!\033[0m\n";
	printList(hosts);
	std::cout <<"\nType \033[33m[h]\033[0m or \033[33m[help]\033[0m for help...\n";
	while (args.main){
		std::string answer;
		answer = request();
		switch (commander(answer)){
			case 1:
				std::cout << "\033[31m[WARN]\033[0m Wrong command! Type \033[33m[help]\033[0m to see all commands\n";
				break;
			case 2:
				{
					std::cout << "\033[32m[INFO]\033[0m Enter the number of host: ";
					std::cin >> answer;
					int q;
					if(check(answer, q)){
						if (hosts.hostExicst(q)){
							std::map<std::string, std::string> host = hosts.getHost(q);
							connectHost(host);
						}
						else{
							std::cout << "\033[31m[WARN]\033[0m Could not find this host!\n";
						}
					}
				}
				break;
			case 3:
				addHost(hosts);
				break;
			case 4:
				{
					std::cout << "\033[32m[INFO]\033[0m Enter the number of host: ";
					std::cin >> answer;
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
				std::cout << "" << std::endl;
				break;
			case 8:
				return 0;
				break;
			case 9:
			{
				std::cout << "\033[32m[INFO]\033[0m Enter the number of host: ";
				std::cin >> answer;
				int q;
				if (check(answer, q)) {
					if (hosts.hostExicst(q)) {
						std::map<std::string, std::string> host = hosts.getHost(q);
						connectHost(host, true);
					}
					else {
						std::cout << "\033[31m[WARN]\033[0m Could not find this host!\n";
					}
				}
				break;
			}
		}
	}
	return 0;
}