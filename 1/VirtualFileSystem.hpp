#pragma once
#include "Types.hpp"

struct VirtualFileSystem
{
	VirtualFileSystem();
	~VirtualFileSystem();

	/* �ļ�ϵͳ�Ƿ���� */
	bool is_created;

	/* �ļ�ϵͳ��ǰ״̬ */
	u16         cusr;                /* ��ǰ�û�id */
	u32         cwd;                 /* ��ǰ����Ŀ¼(inode��) */
	std::string pwd;                 /* ��ǰ����Ŀ¼��(�ɴ�ӡ) */

	/* �ļ�ϵͳ��������� */
	vector<User*>   users;           /* �û��� */
	vector<Group*>  blocks;          /* block groups */
	SuperBlock     *sb;              /* super block */
	vector<Record*> cache;           /* ���� */

	/* �����ṩ�Ľӿ� */
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

	/* �ڲ����� */
private:
	void print_inode(Inode* i);
	Inode* find_inode(Group** g, u32 inode);
	Inode* find_inode(u32 inode);
	void delete_file(string name, u32 p_inode); 
};
