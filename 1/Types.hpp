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
	string name;                /* 文件名 */
	string data;                /* 文件数据 */
};

struct SuperBlock
{
	u32 inodes_count;           /* inodes 计数 */
	u32 blocks_count;           /* blocks 计数 */
	u32 free_blocks_count;      /* 空闲的 blocks 计数 */
	u32 free_inodes_count;      /* 空闲的 inodes 计数 */
	u16 block_size;             /* block 的大小 单位byte */
	u16 blocks_per_group;       /* 每 block group 的 block 数量 */
	u16 inodes_per_group;       /* 每 block group 的 inode 数量 */
	u16 group_count;            /* group 数量 */
};

struct Inode 
{
	u32 inode;             /* inode 索引节点 */
	u16 type;              /* 文件类型 目录1 文件0 */
	string access;         /* 访问权限 只有拥有者和其他人(rwxr--) */
	u16 uid;               /* 拥有者ID */ 
	u32 size;              /* 文件大小 单位byte */  
	time_t mtime;          /* 最后修改时间 */
	vector<pblock> blocks; /* 实际存放数据的组内block位置 */
};

struct Group
{
	vector<bit> block_bit_map;  /* 每个bit记录一个block是否被占用 1占用0空闲 */
	vector<bit> inode_bit_map;  /* 每个bit记录一个inode号是否被占用 */
	vector<Inode*> inode_table; /* inode号表 */
	vector<Block*> data;	    /* 数据 包含目录和文件 目录里的内容是子文件的inode号 */ 
	u16 free_blocks_count;      /* 空闲的 blocks 计数 */
    u16 free_inodes_count;      /* 空闲的 inodes 计数 */
};

struct Record
{
	u32 inode;             /* inode 号 */
	string name;           /* 文件名 */
	u32 p_inode;           /* 父inode 号 */
	string p_name;         /* 父文件名 */
	u16 type;              /* 文件类型 目录1 文件0 */
};

struct User
{
	u16 id;                /* 用户编号 */
	string name;           /* 用户名 */
	string password;       /* 密码 */
};

/* 帮助函数 */ 
void split(const string& str, vector<string>& elems, char delim = ' ');
