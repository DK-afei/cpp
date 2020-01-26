#pragma once
#include <vector>
#include <string>
#include <ctime>

using std::vector;
using std::string;

typedef unsigned int u32;
typedef unsigned int pblock;
typedef unsigned short u16;
typedef bool bit;

struct Block
{
	string name;                /* �ļ��� */
	string data;                /* �ļ����� */
};

struct SuperBlock
{
	u32 inodes_count;           /* inodes ���� */
	u32 blocks_count;           /* blocks ���� */
	u32 free_blocks_count;      /* ���е� blocks ���� */
	u32 free_inodes_count;      /* ���е� inodes ���� */
	u16 block_size;             /* block �Ĵ�С ��λbyte */
	u16 blocks_per_group;       /* ÿ block group �� block ���� */
	u16 inodes_per_group;       /* ÿ block group �� inode ���� */
	u16 group_count;            /* group ���� */
};

struct Inode 
{
	u32 inode;             /* inode �����ڵ� */
	u16 type;              /* �ļ����� Ŀ¼1 �ļ�0 */
	string access;         /* ����Ȩ�� ֻ��ӵ���ߺ�������(rwxr--) */
	u16 uid;               /* ӵ����ID */ 
	u32 size;              /* �ļ���С ��λbyte */  
	time_t mtime;          /* ����޸�ʱ�� */
	vector<pblock> blocks; /* ʵ�ʴ�����ݵ�����blockλ�� */
};

struct Group
{
	vector<bit> block_bit_map;  /* ÿ��bit��¼һ��block�Ƿ�ռ�� 1ռ��0���� */
	vector<bit> inode_bit_map;  /* ÿ��bit��¼һ��inode���Ƿ�ռ�� */
	vector<Inode*> inode_table; /* inode�ű� */
	vector<Block*> data;	    /* ���� ����Ŀ¼���ļ� Ŀ¼������������ļ���inode�� */ 
	u16 free_blocks_count;      /* ���е� blocks ���� */
    u16 free_inodes_count;      /* ���е� inodes ���� */
};

struct Record
{
	u32 inode;             /* inode �� */
	string name;           /* �ļ��� */
	u32 p_inode;           /* ��inode �� */
	string p_name;         /* ���ļ��� */
	u16 type;              /* �ļ����� Ŀ¼1 �ļ�0 */
};

struct User
{
	u16 id;                /* �û���� */
	string name;           /* �û��� */
	string password;       /* ���� */
};

/* �������� */ 
void split(const string& str, vector<string>& elems, char delim = ' ');
