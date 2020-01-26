#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"filenode.h"
using namespace std;
class user
{
public:
	user();
	~user();
	//用户操作
	bool login(string account, string password);
	bool adduser(string account, string password, string grp);
	bool deluser(string account);
	bool chgpwd(string pwd, string account);

	
	//文件操作
	bool mkdir(string filename);//只当前路径
	bool chmod(char mod[3],string path);	//数字操作型 chmod +x(?)
	bool chown(string owner,string path);
	bool chgrp(string group, string path);
	bool cd(string path);
	bool rmdir(string path);//只删除空文件夹
	bool cat(string path);//只能查看非目录文件内容
	bool touch(string filename);//只当前路径
	bool rm(string path);
	bool mv(string newname,string old);//只改名 无移动功能
	//pwd=cout crtpath
	void ls(string arg);
	void ls();
	bool cp(string path1, string path2);	//检查path2目录下有无崇明文件//只能拷贝文件到另一文件或文件夹
	void vi(string path, string txt);


	
	//getter & setter
	void setUsrname(string str);
	void setPwd(string str);
	void setGroup(string str);
	string getUsrname();
	string getPwd();
	string getGroup();

	void printinfo();
	string home;

public:
	void updateSize(string path,string updatefile);
	void updateSize(string path, int changesize,bool pls);
	int allocInode();
	int allocBlock();
	int findPath(string path, int &prenode);//通过路径查找并返回最后一个结点的inode和上一个结点的inode
	//根据父节点和文件名查找inode/查找block 通过inode查block
	int findInode(const int inum, string filename);
	//追加存储内容 mkdir append后记得更新linknum
	void append2LastBlk(const int inum,string appendText);
	void freeblk(const int inum);
	string usrname;
	string pwd;
	string group;
	int posct=-1;	//使用时的初始化问题
	int poslk=-1;
	
};

