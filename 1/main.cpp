
#include "Types.hpp"
#include "VirtualFileSystem.hpp"

#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
using std::cout; using std::cin; using std::endl;
using std::string; using std::vector;





const string promptor = "$ ";
const string illeagal_cmd = "illeagal command.";
unsigned file_name_count = 1;

void prompt(string name, string path)
{
	cout << name << ":" << path << promptor;
}

void split(const string& str, vector<string>& elems, char delim) 
{
	std::istringstream iss(str);
	for (string item; getline(iss, item, delim);)
	elems.push_back(item);
}

int main()
{
	VirtualFileSystem *fs = new VirtualFileSystem;

	bool isContinue = true;
	while (isContinue)
	{
		// 打印提示符
		prompt(fs->get_cusr(), fs->get_cpath());
		
		// 读入命令
		string line;
		while(getline(cin, line) && line == "");
		vector<string> args; 
		split(line, args, ' ');
		auto args_count = args.size();
		if (args_count == 0) continue;

        // 处理命令
		if (args[0] == "clear")
		{
			system("cls");
		}
		else if (fs->is_created && args[0] == "ls")
		{
			if(args_count > 2) cout << "too many arg(s).";
			else if(args_count > 1) fs->ls(args[1]);
			else fs->ls();
		}
		else if (fs->is_created && args[0] == "cd")
		{
			if(args_count > 2) cout << "too many arg(s).";
			else if(args_count > 1) fs->cd(args[1]);
			else cout << "lack arg(s).";
		}
		else if (fs->is_created && args[0] == "mkdir")
		{
			if(args_count > 1) fs->mkdir(args[1]);
			else cout << "lack arg(s).";
		}
		else if (fs->is_created && args[0] == "touch")
		{
			if (args.size() > 2) fs->touch(args[1], args[2]);
			else if (args.size() > 1) fs->touch(args[1]);
			else fs->touch("file" + std::to_string(file_name_count++));
		}
		else if (fs->is_created && args[0] == "cat")
		{
			if (args_count < 2) cout << "lack arg(s).";
			else fs->cat(args[1]);
		}
		else if (fs->is_created && args[0] == "chmod")
		{
			if (args_count < 3) cout << "lack arg(s).";
			else fs->chmod(args[1], args[2]);
		}
		else if (fs->is_created && args[0] == "rm")
		{
			if (args_count < 2) cout << "lack arg(s).";
			else if (args_count > 2) fs->rm(args[2], args[1]);
			else fs->rm(args[1]);
		}
		else if (!fs->is_created && args[0] == "mkfs") // 参数给出磁盘块数和块大小
		{
			if (args_count < 2) fs->mkfs();
			else if (args_count < 3) fs->mkfs(stoi(args[1]));
			else fs->mkfs(stoi(args[1]), stoi(args[2]));
		}
		else if (fs->is_created && args[0] == "df")
		{
			fs->df();
		}
		else if (fs->is_created && args[0] == "format")
		{
			delete fs;
			fs = new VirtualFileSystem;
		}
		else if (args[0] == "login")
		{
			fs->login();
		}
		else if (args[0] == "logout")
		{
			fs->logout();
		}
		else if (args[0] == "quit")
		{
			isContinue = false;
		}
		else
		{
			cout << illeagal_cmd;
		}

		// 打印空行
		cout << endl;
	}

	delete fs;

	return 0;
}

