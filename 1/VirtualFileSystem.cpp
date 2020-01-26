#include "Types.hpp"
#include "VirtualFileSystem.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using std::cout; using std::cin; using std::endl;
using std::string; using std::vector; using std::stack; 

VirtualFileSystem::VirtualFileSystem()
{
    is_created = false;

    // 创建默认用户组 
    User *usr0 = new User;
    usr0->id = 0;
    usr0->name = "root";
    usr0->password = "root";

    User *usr1 = new User;
    usr1->id = 1;
    usr1->name = "visitor";
    usr1->password = "";

    users.push_back(usr0);
    users.push_back(usr1);

    // 设置文件系统状态 
    cusr = 1;  // 默认访客登录 
    cwd = 0;   // 默认当前目录为根目录 
    pwd = "/"; // 根目录记为"/" 
}

VirtualFileSystem::~VirtualFileSystem()
{
    if (is_created)
    {
		if (!cache.empty())
		{
			for (int k = 0; k < cache.size(); ++k) delete cache[k];
		}
		if (sb)
		{
			delete sb;
		}
		if (!blocks.empty())
		{
			for (int i = 0; i < blocks.size(); ++i)
			{
				Group *g = blocks[i];
				if (!g->inode_table.empty())
					for (int j = 0; j < g->inode_table.size(); ++j)
						delete g->inode_table[j];
				if (!g->data.empty())
					for (int j = 0; j < g->data.size(); ++j)
						delete g->data[j];
			}

		}
    }
	if (!users.empty()) for (auto u : users) delete u;
}

string VirtualFileSystem::get_cusr()
{
	return users[cusr]->name;
}

string VirtualFileSystem::get_cpath()
{
	string path = pwd;
	string cur = pwd;
	while (cur != "/")
	{
		for (auto c : cache)
			if (c->name == cur) { cur = c->p_name; break; }
		if (cur == "/") path = cur + path;
		else path = cur + "/" + path;
	}
	return path;
}

void VirtualFileSystem::login()
{
    cout << "user name:" << endl;
    string name; cin >> name;

    int i;
    for (i = 0; i < users.size(); ++i)
        if (users[i]->name == name)
            break;
    if (i < users.size())
    {
        cout << "password:" << endl;
        string password; 
        for (int j = 0; j < 3; ++j)
        {
            cin >> password;
            if (password == users[i]->password)
            {
                cusr = users[i]->id;
                return;
            }
            cout << "wrong password, try again." << endl;
        }
    }
    else 
    {
        User *new_usr = new User;
        new_usr->name = name;
        new_usr->id = users.size();
        cout << "set your password:" << endl;
        cin >> new_usr->password;
        users.push_back(new_usr);
        cusr = new_usr->id;
    }
}

void VirtualFileSystem::logout()
{
    cusr = 1;
}

void VirtualFileSystem::mkdir(string name)
{
	// 有重名return
    for (int i = 0; i < blocks.size(); ++i)
    {
        Group *g = blocks[i];

        // 找到一个可用的inode
        int j = 0;
        while (g->inode_bit_map[j] && j < g->inode_bit_map.size()) ++j;
        if (j >= g->inode_bit_map.size()) continue;

        // 找到一个可用的block
        int k = 0;
        while (g->block_bit_map[k] && k < g->block_bit_map.size()) ++k;
        if (k >= g->block_bit_map.size()) continue;

        // 创建目录inode
        Inode *newinode = new Inode;
        newinode->inode = sb->inodes_count - sb->free_inodes_count;
        newinode->type = 1;
        newinode->access = "rwxr--";
        newinode->uid = cusr;
        newinode->size = sb->block_size;
        newinode->mtime = time(nullptr);
        newinode->blocks.push_back(k);
		if (j < g->inode_table.size()) g->inode_table[j] = newinode;
        else g->inode_table.push_back(newinode);

        // 创建目录实际内容
        Block *dirblock = new Block;
        dirblock->name = name;
        dirblock->data = std::to_string(newinode->inode) + " " + std::to_string(cwd); // 第一个是.的inode 第二个是..的inode
		if (k < g->data.size()) g->data[k] = dirblock;
		else g->data.push_back(dirblock);

        // 改变计数
        --sb->free_inodes_count;
        --sb->free_blocks_count;

        g->block_bit_map[k] = 1;
        g->inode_bit_map[j] = 1;
        --g->free_blocks_count;
        --g->free_inodes_count;

		// 改变父目录内容
		Group *pg = nullptr;
		Inode* ino = find_inode(&pg, cwd);
		pg->data[ino->blocks[0]]->data = 
			pg->data[ino->blocks[0]]->data + " " + std::to_string(newinode->inode);

        // 写入缓存
        Record *record = new Record;
        record->inode = newinode->inode;
        record->name = dirblock->name;
        record->p_inode = cwd;
        record->p_name = pwd;
        record->type = 1;
        cache.push_back(record);

       return; 
    }
}

void VirtualFileSystem::mkfs(unsigned total, unsigned blocksize)
{
	if (is_created) { cout << "file system already exist."; return; }

    // 设置super block 
    u16 group_count = blocksize / 144; // 一个group decriptor的大小是144B 
    u16 block_per_inode = 2;           // 设每个inode至少占用2个block 

    sb = new SuperBlock;
    sb->blocks_count = total;
    sb->blocks_per_group = sb->blocks_count / group_count;
    sb->inodes_per_group = sb->blocks_per_group / block_per_inode;
    sb->inodes_count = sb->inodes_per_group * group_count;
    sb->free_inodes_count = sb->inodes_count; 
    sb->free_blocks_count = sb->blocks_count; 
	sb->block_size = blocksize;
	sb->group_count = group_count;

    // 创建磁盘空间 
	blocks.resize(group_count, nullptr);
	for (int i = 0; i < group_count; ++i)
	{
		Group *g = new Group;
		g->block_bit_map.resize(sb->blocks_per_group, 0);
		g->inode_bit_map.resize(sb->inodes_per_group, 0);
		g->free_blocks_count = sb->blocks_per_group;
		g->free_inodes_count = sb->inodes_per_group;
		blocks[i] = g;
	}

    // 创建根目录
	// 创建根目录inode
	Inode *rootinode = new Inode;
	rootinode->inode = 0;
	rootinode->type = 1;
	rootinode->access = "rwxr--";
	rootinode->uid = 0;
	rootinode->size = sb->block_size;
	rootinode->mtime = time(nullptr);
	rootinode->blocks.push_back(0);
	blocks[0]->inode_table.push_back(rootinode);

	// 创建根目录实际内容
	Block *rootblock = new Block;
	rootblock->name = "/";
	rootblock->data = "0 -1"; // 第一个是. 第二个是..
	blocks[0]->data.push_back(rootblock);

	// 改变计数
	--sb->free_inodes_count;
	--sb->free_blocks_count;

	blocks[0]->block_bit_map[0] = 1;
	blocks[0]->inode_bit_map[0] = 1;
	--blocks[0]->free_blocks_count;
	--blocks[0]->free_inodes_count;

	// 写入缓存
	Record *record = new Record;
	record->inode = rootinode->inode;
	record->name = rootblock->name;
	record->p_inode = sb->inodes_count; // 最大的inode(不存在)
	record->p_name = "";
	record->type = 1;
	cache.push_back(record);

    cwd = 0;   /* 当前工作目录的inode号 */
    pwd = "/"; /* 可打印的当前工作目录 */

    is_created = true;
}

void VirtualFileSystem::touch(string name, string data)
{
	// 有重名return
    for (int i = 0; i < blocks.size(); ++i)
    {
        Group *g = blocks[i];

        // 找到一个可用的inode
        int j = 0;
        while (g->inode_bit_map[j] && j < g->inode_bit_map.size()) ++j;
        if (j >= g->inode_bit_map.size()) continue;

        // 找到一个可用的block
        int k = 0;
        while (g->block_bit_map[k] && k < g->block_bit_map.size()) ++k;
        if (k >= g->block_bit_map.size()) continue;

        // 创建文件inode
        Inode *newinode = new Inode;
        newinode->inode = sb->inodes_count - sb->free_inodes_count;
        newinode->type = 0;
        newinode->access = "rwxr--";
        newinode->uid = cusr;
        newinode->size = data.size();
        newinode->mtime = time(nullptr);
        newinode->blocks.push_back(k);
		if (j < g->inode_table.size()) g->inode_table[j] = newinode;
        else g->inode_table.push_back(newinode);

        // 创建文件实际内容
        Block *fileblock = new Block;
        fileblock->name = name;
		fileblock->data = data;
		if (k < g->data.size()) g->data[k] = fileblock;
		else g->data.push_back(fileblock);

        // 改变计数
        --sb->free_inodes_count;
        --sb->free_blocks_count;

        g->block_bit_map[k] = 1;
        g->inode_bit_map[j] = 1;
        --g->free_blocks_count;
        --g->free_inodes_count;

		// 改变父目录内容
		Group *pg = nullptr;
		Inode* ino = find_inode(&pg, cwd);
		pg->data[ino->blocks[0]]->data = 
			pg->data[ino->blocks[0]]->data + " " + std::to_string(newinode->inode);

        // 写入缓存
        Record *record = new Record;
        record->inode = newinode->inode;
        record->name = fileblock->name;
        record->p_inode = cwd;
        record->p_name = pwd;
        record->type = 0;
        cache.push_back(record);

       return; 
    }
}

void VirtualFileSystem::ls(string option)
{
	if (option == "") // 从cache里面找
	{
		for (auto c : cache)
			if (c->p_inode == cwd)
				cout << c->name + " ";
	}
	else if (option == "-l")
	{
		// 找到当前目录下所有子文件/目录的inode
		for (int i = 0; i < cache.size(); ++i)
		{
			if (cache[i]->p_inode == cwd)
			{
				Inode *ino = find_inode(cache[i]->inode);
				if (ino) print_inode(ino);// 打印出子文件/目录的inode信息
			}
		}
	}
}

void VirtualFileSystem::cd(string path)
{
	if (path == ".")
	{
		return;
	}
	else if (path == "..")
	{
		if (pwd == "/") return;
		for (auto c : cache) 
			if (c->inode == cwd)
			{
				pwd = c->p_name;
				cwd = c->p_inode;
				return;
			}
	}
	else
	{
		// 如果参数是子目录, 往下走
		for (auto c : cache) 
			if (c->type == 1 && c->name == path && c->p_name == pwd)
			{
				pwd = c->name;
				cwd = c->inode;
				return;
			}
	}
}

void VirtualFileSystem::cat(string name)
{
	// 从cache里面找
	for (auto c : cache)
		if (c->name == name && c->p_inode == cwd)
		{
			Group *g = nullptr;
			Inode *i = find_inode(&g, c->inode);
			for (auto p : i->blocks)
				cout << g->data[p]->data;
		}
}

void VirtualFileSystem::chmod(string option, string name)
{
	Inode *i = nullptr;
	for (auto c : cache)
		if (c->name == name && c->p_inode == cwd)
		{
			i = find_inode(c->inode);
			break;
		}
	if (i == nullptr)
	{
		cout << name << " doesn't exist." << endl;
		return;
	}
	if (0 == cusr || i->uid == cusr)
	{
		if (option == "a+x") i->access[5] = 'x';
		else if (option == "a-x") i->access[5] = '-';
		else if (option == "a+w") i->access[4] = 'w';
		else if (option == "a-w") i->access[4] = '-';
	}
	else cout << "permission denied.";
}

void VirtualFileSystem::rm(string name, string option)
{
	if (option == "") // 删除文件
	{
		delete_file(name, cwd);
	}
	else if (option == "-r") // 删除目录及其子文件
	{
		stack<std::pair<string, u32> > undeal_dirs; // 当前目录及其父目录inode
		undeal_dirs.push(std::make_pair(pwd, cwd));
		while (!undeal_dirs.empty())
		{
			auto cur = undeal_dirs.top(); undeal_dirs.pop();
			vector<string> files;
			u32 cur_pino;
			for (auto c : cache) // 找到所有子目录和文件, 暂存目录
			{
				if (c->p_name == cur.first)
				{
					if (c->type == 0) { files.push_back(c->name); cur_pino = c->p_inode; }
					else undeal_dirs.push(std::make_pair(c->name, c->p_inode));
				}
			}
			for (auto f : files) // 删除文件
				delete_file(f, cur_pino);
			if (cur.first == "/") continue; // 根目录不删
			delete_file(cur.first, cur.second); // 删除目录
		}
	}
}

void VirtualFileSystem::df()
{
	cout << "Filesystem  " << endl;
	cout << "1k-blocks   " << (sb->blocks_count * sb->block_size) / 1024 << endl;
	cout << "Used        " << ((sb->blocks_count - sb->free_blocks_count) * sb->block_size) / 1024 << endl;
	cout << "Available   " << (sb->free_blocks_count * sb->block_size) / 1024 << endl;
	cout << "Used        " << ((sb->blocks_count - sb->free_blocks_count) / sb->blocks_count) * 100 << "%" << endl;
	cout << "Inode       " << endl;
	cout << "Total       " << (sb->inodes_count) << endl;
	cout << "Used        " << (sb->inodes_count - sb->free_inodes_count) << endl;
	cout << "Available   " << sb->free_inodes_count << endl;
	cout << "Used        " << (double)((sb->inodes_count - sb->free_inodes_count) / sb->inodes_count ) * 100 << "%" << endl;
	cout << "Mounted on  /"<< endl;
}

void VirtualFileSystem::print_inode(Inode* i)
{
	cout << (i->type ? "d" : "-");
	cout << i->access << " " << i->inode << " " << users[i->uid]->name << " " << i->size << " ";
	string time = string(ctime(&(i->mtime))); time.pop_back();
	cout << time << " ";
	for (auto c : cache) if (c->inode == i->inode) { cout << c->name << endl; return; }
	/* 没找到再搜索 */
}

Inode* VirtualFileSystem::find_inode(Group** g, u32 inode)
{
	unsigned group_id = inode / sb->inodes_per_group;
	*g = blocks[group_id];
	for (Inode *i : (*g)->inode_table)
		if (i && i->inode == inode)
			return i;
	return nullptr;
}

Inode* VirtualFileSystem::find_inode(u32 inode)
{
	unsigned group_id = inode / sb->inodes_per_group;
	Group *g = blocks[group_id];
	for (Inode *i : g->inode_table)
		if (i->inode == inode)
			return i;
	return nullptr;
}

void VirtualFileSystem::delete_file(string name, u32 p_inode)
{
	Inode *i = nullptr;
	Group *g = nullptr;
	int j;
	for (j = 0; j < cache.size(); ++j)
	{
		if (cache[j] && cache[j]->name == name && cache[j]->p_inode == p_inode)
		{
			i = find_inode(&g, cache[j]->inode);
			break;
		}
	}
	if (i && i->access[5] == 'x' || (i->access[2] == 'x' && i->uid == cusr) || cusr == 0)
	{
		// 改变计数
		sb->free_blocks_count += i->blocks.size();
		sb->free_inodes_count += 1;
		g->free_blocks_count += i->blocks.size();
		g->free_inodes_count += 1;

		// 从包含它的目录中删除 
		Group *p_g = nullptr;
		Inode *p_i = find_inode(&p_g, p_inode);
		if (p_i)
		{
			string text = p_g->data[p_i->blocks[0]]->data;
			string text_ino = std::to_string(i->inode);
			auto pos = text.find(text_ino);
			if (pos != std::string::npos)
			{
				text.replace(pos, text_ino.size(), "");
				text.pop_back(); // 删除空格
				p_g->data[p_i->blocks[0]]->data = text;
			}
		}

		// 从block bit map中删除
		for (auto b : i->blocks)
			g->block_bit_map[b] = 0;
		// 从inode bit map中删除
		g->inode_bit_map[i->inode % sb->inodes_per_group] = 0;

		// 从inode表中删除
		for (int k = 0; k < g->inode_table.size(); ++k)
		{
			if (g->inode_table[k] && g->inode_table[k]->inode == i->inode)
			{
				delete g->inode_table[k];
				g->inode_table[k] = nullptr;
				break;
			}
		}

		// 从缓存中删除
		delete cache[j];
		cache[j] = nullptr;
		cache.erase(cache.begin() + j);
	}
	else cout << "permission denied.";
}
