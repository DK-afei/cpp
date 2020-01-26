#pragma once
#include "Types.hpp"

struct VirtualFileSystem
{
	VirtualFileSystem();
	~VirtualFileSystem();

	/* 文件系统是否可用 */
	bool is_created;

	/* 文件系统当前状态 */
	u16         cusr;                /* 当前用户id */
	u32         cwd;                 /* 当前工作目录(inode号) */
	std::string pwd;                 /* 当前工作目录名(可打印) */

	/* 文件系统保存的数据 */
	vector<User*>   users;           /* 用户组 */
	vector<Group*>  blocks;          /* block groups */
	SuperBlock     *sb;              /* super block */
	vector<Record*> cache;           /* 缓存 */

	/* 向外提供的接口 */
	string get_cusr();
	string get_cpath();

	void login();
	void logout();
	void mkdir(string name);
	void touch(string name, string data = "");
	void mkfs(unsigned total = 512, unsigned blocksize = 1024);
	void ls(string option = "");
	void cd(string path);
	void cat(string name);
	void chmod(string option, string name);
	void rm(string name, string option = "");
	void df();

	/* 内部函数 */
private:
	void print_inode(Inode* i);
	Inode* find_inode(Group** g, u32 inode);
	Inode* find_inode(u32 inode);
	void delete_file(string name, u32 p_inode); 
};
