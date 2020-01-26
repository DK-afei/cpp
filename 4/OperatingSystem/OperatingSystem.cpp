// OperatingSystem.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<fstream>
#include<iostream>
extern vector<user*> users;
extern inode* inod[20];
extern block* blk[100];
extern user* cyan;
extern string crtpath;
extern user* crtuser;
fstream fuser,fdisk,fblk;
using namespace std;

void init(){

	fuser.open("users.txt", ios::in | ios::out);	//读取用户信息

	char bufi[144];
	char bufb[520];

	for (int i = 0; i < 20; i++){
		inod[i] = (inode*)malloc(144); //分配144内存给每个inode块		
		new (inod[i]) inode;
	}
	for (int i = 0; i < 100; i++){
		blk[i] = (block*)malloc(520);	//分配520内存给每个inode块
		blk[i]->bnum = i;
		new (blk[i]) block;
	}

	fdisk.open("inode.txt", ios::in | ios::out);
	//int i = 0;
	int k = 0;

	
	//初始化inode块
	while (!fdisk.eof()){
		fdisk.getline(bufi, 144);
		string buf = bufi;
		string t;
		vector<string>s;
		for (int j = 0; j < buf.length(); j++){
			if (buf[j] == ' '){
				s.push_back(t);
				t = "";
			}
			else t += buf[j];
		}
		s.push_back(t);
		if (s.size() == 10){
			if (s[0] == "true") inod[k]->status = true;
			else inod[k]->status = false;

			inod[k]->owner = s[1];
			inod[k]->group = s[2];
			inod[k]->linknum = atoi(s[3].c_str());
			inod[k]->size = atoi(s[4].c_str());
			if (s[5] != "NULL") inod[k]->bp[0] = blk[atoi(s[5].c_str())];
			if (s[6] != "NULL") inod[k]->bp[1] = blk[atoi(s[6].c_str())];
			if (s[7] != "NULL") inod[k]->bp[2] = blk[atoi(s[7].c_str())];
			if (s[8] != "NULL") inod[k]->bp[3] = blk[atoi(s[8].c_str())];
			inod[k]->permission = s[9];
		}
		k++;
	}
	
	//初始化block块
	for (int i = 0; i < 100; i++){
		string fname = "./block/" + to_string(i);
		fblk.open(fname, ios::in | ios::out);
		char buf[512];
		bool sf = true;
		while (!fblk.eof()){
			fblk.getline(buf, 512);
			string txt = buf;
			if (sf){			
				if (txt == "true") blk[i]->status = true;
				else blk[i]->status = false;
				blk[i]->bnum = i;
				sf = false;
			}
			else blk[i]->text += txt;
		}
		fblk.close();
	}
	
	
	while (!fuser.eof()){
		fuser.getline(bufi,144);
		string buf = bufi;
		vector<string> s;
		string t;
		for (int i = 0; i < buf.length(); i++){
			if (buf[i] == ' '){
				s.push_back(t);
				t = "";			
			}
			else t += buf[i];	
		}
		s.push_back(t);
		if (s.size() == 3){
			user* u = new user();
			u->setUsrname(s[0]);
			u->setPwd(s[1]);
			u->setGroup(s[2]);
			u->home = "/" + s[0] + "/";
			users.push_back(u);
		}
	}

	
	fuser.close();
	fdisk.close();

}

void beforeexit(){
	fuser.open("users.txt", ios::in | ios::out);
	fdisk.open("inode.txt", ios::in | ios::out);
	for (int i = 0; i < users.size(); i++){
		string txt;
		txt += users[i]->getUsrname() + " ";
		txt += users[i]->getPwd() + " ";
		txt += users[i]->getGroup();
		txt += "\n";
		fuser.write(txt.c_str(), txt.length());
	}
	for (int i = 0; i < 20; i++){
		string txt;
		if (inod[i]->status) txt += "true ";
		else txt += "false ";
		txt += inod[i]->owner + " ";
		txt += inod[i]->group + " ";
		txt += to_string(inod[i]->linknum) + " ";
		txt += to_string(inod[i]->size) + " ";
		for (int j = 0; j < 4; j++){
			if (inod[i]->bp[j] == NULL){
				txt += "NULL ";
			}
			else txt += to_string(inod[i]->bp[j]->bnum)+" ";
		}
		
		txt += inod[i]->permission;
		txt += "\n";
		fdisk.write(txt.c_str(), txt.length());
	}
	for (int i = 0; i < 100; i++){
		string fname = "./block/" + to_string(i);
		fblk.open(fname, ios::in | ios::out);
		string txt;
		if (blk[i]->status)
			txt += "true\n";
		else txt += "false\n";
		txt += blk[i]->text;
		fblk.write(txt.c_str(), txt.length());
		fblk.close();
	}
	fdisk.close();
	fuser.close();
}

void panel(){
	cout << "--------------------------------------------------------------------------------\n";
	cout << "                                   登录系统\n";
	cout << "--------------------------------------------------------------------------------\n";
	string user, pwd;
	cout << "输入用户名：";
	cin >> user;
	cout << "输入密码：";
	cin >> pwd;
	if (cyan->login(user, pwd)){
		cout << "------------------------------------------------------------------------------\n";
		cout << "                             Welcome " + user + "\n";
		cout << "------------------------------------------------------------------------------\n";
		string command = "";
		int bg = 1;

		char jdf;
		if (user == "root") jdf = '#';
		else jdf = '@';
		while (getline(cin, command)){
			if (command == "exit") return;
			vector<string>args;
			string arg = "";
			for (int i = 0; i < command.length(); i++){
				if (command[i] != ' '){
					arg += command[i];
					continue;
				}
				else{
					if (arg != " ") args.push_back(arg);
					arg = "";
				}
			}

			if (arg != "") args.push_back(arg);

			if (args.size() == 1){
				if (args[0] == "ls") cyan->ls();
				else if (args[0] == "pwd") cout << crtpath << endl;
				else if (args[0] == "help"){
					cout << "ls [option]... 显示当前文件夹下所有文件的文件名或详细信息\n";
					cout << "cd [path]... 进入文件夹\n";
					cout << "pwd 显示当前所在路径\n";
					cout << "mkdir [filename]... 在当前路径中创建文件夹\n";
					cout << "rmdir [path]... 删除空文件夹\n";
					cout << "touch [filename]... 在当前路径中创建文件\n";
					cout << "rm [path]... 删除文件\n";
					cout << "cat [path]... 显示文件内容\n";
					cout << "vi [path]... 向文件中添加内容\n";
					cout << "mv [filename1]...[filename2]... 改变文件名\n";
					cout << "cp [path1]...[path2]... 拷贝文件\n";
					cout << "chmod [mod]...[path]... 改变文件或文件夹权限\n";
					cout << "chgrp [group]... 改变文件或文件夹所属组\n";
					cout << "chown [owner]... 改变文件或文件夹所属用户\n";

					cout << "chpwd [newpassword]...[user]... 改变用户密码\n";
					cout << "deluser [username]... 删除用户\n";
					cout << "adduser [username]...[password]...[group]... 新增用户\n";
					cout << "su [username]...[password]... 切换登录用户\n";
				}
				else if (args[0] == "cd") cout << "cd [path]... 进入文件夹\n";
				else if (args[0] == "mkdir") cout << "mkdir [filename]... 在当前路径中创建文件夹\n";
				else if (args[0] == "rmdir") cout << "rmdir [path]... 删除空文件夹\n";
				else if (args[0] == "cat") cout << "cat [path]... 显示文件内容\n";
				else if (args[0] == "touch") cout << "touch [filename]... 在当前路径中创建文件\n";
				else if (args[0] == "rm") cout << "rm [path]... 删除文件\n";
				else if (args[0] == "vi") cout << "vi [path]... 向文件中添加内容\n";
				else if (args[0] == "chmod") cout << "chmod [mod]...[path]... 改变文件或文件夹权限\n";
				else if (args[0] == "chgrp") cout << "chgrp [group]... 改变文件或文件夹所属组\n";
				else if (args[0] == "chown") cout << "chown [owner]... 改变文件或文件夹所属用户\n";
				else if (args[0] == "mv") cout << "mv [filename1]...[filename2]... 改变文件名\n";
				else if (args[0] == "cp") cout << "cp [path1]...[path2]... 拷贝文件\n";
				else if (args[0] == "deluser") cout << "deluser [username]... 删除用户\n";
				else if (args[0] == "chpwd") cout << "chpwd [newpassword]...[user]... 改变用户密码\n";
				else if (args[0] == "su") cout << "su [username]...[password]... 切换登录用户\n";
				else if (args[0] == "adduser") cout << "adduser [username]...[password]...[group]... 新增用户\n";
				else cout << "找不到指令！" << endl;
			}
			else if (args.size() == 2){
				if (args[0] == "ls") cyan->ls(args[1]);
				else if (args[0] == "cd") cyan->cd(args[1]);
				else if (args[0] == "mkdir") cyan->mkdir(args[1]);
				else if (args[0] == "rmdir") cyan->rmdir(args[1]);
				else if (args[0] == "cat") cyan->cat(args[1]);
				else if (args[0] == "touch") cyan->touch(args[1]);
				else if (args[0] == "rm") cyan->rm(args[1]);
				else if (args[0] == "vi"){
					string txt;
					cin >> txt;
					cyan->vi(args[1], txt);
					continue;
				}
				else if (args[0] == "deluser"){
					if (user != "root") cout << "deluser:没有删除用户的权限!" << endl;
					else cyan->deluser(args[1]);
				}
				else if (args[0] == "chpwd"){
					cyan->chgpwd(args[1], user);
					cout << "修改密码成功！" << endl;
				}
				else if (args[0] == "chmod") cout << "chmod [mod]...[path]... 改变文件或文件夹权限\n";
				else if (args[0] == "chgrp") cout << "chgrp [group]... 改变文件或文件夹所属组\n";
				else if (args[0] == "chown") cout << "chown [owner]... 改变文件或文件夹所属用户\n";
				else if (args[0] == "mv") cout << "mv [filename1]...[filename2]... 改变文件名\n";
				else if (args[0] == "cp") cout << "cp [path1]...[path2]... 拷贝文件\n";
				else if (args[0] == "su") cout << "su [username]...[password]... 切换登录用户\n";
				else if (args[0] == "adduser") cout << "adduser [username]...[password]...[group]... 新增用户\n";
				else cout << "找不到指令！" << endl;
			}
			else if (args.size() == 3){

				if (args[0] == "chmod"){
					if (args[1].length() != 3)
						cout << "chmod:参数有误！" << endl;

					else cyan->chmod(const_cast<char*>(args[1].c_str()), args[2]);
				}
				else if (args[0] == "chgrp") cyan->chgrp(args[1], args[2]);
				else if (args[0] == "chown") cyan->chown(args[1], args[2]);
				else if (args[0] == "mv") cyan->mv(args[1], args[2]);
				else if (args[0] == "cp") cyan->cp(args[1], args[2]);		
				else if (args[0] == "su"){
					if (cyan->login(args[1], args[2])){
						if (args[1] == "root") jdf = '#';
						else jdf = '@';
						user = args[1];
					}
					else cout << "su:用户名或密码错误!" << endl;
				}
				else if (args[0] == "adduser") cout << "adduser [username]...[password]...[group]... 新增用户\n";
				else cout << "找不到指令！" << endl;
			}
			else if (args.size() == 4){
				if (args[0] == "adduser"){
					if (user != "root") cout << "adduser:没有添加用户的权限!" << endl;
					else cyan->adduser(args[1], args[2], args[3]);
				}				
			}
		
			cout << crtpath << jdf << " ";
		}

	}
	else{
		cout << "登陆失败！\n";
		panel();
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	init();
	
	panel();
	/*cyan->touch("123");
	string txt="";
	for (int i = 0; i < 5; i++){
		for (int i = 0; i < 512; i++)
			txt += "s";
		cyan->vi("123", txt);
		txt = "";
	}*/
	beforeexit();

	return 0;
}

