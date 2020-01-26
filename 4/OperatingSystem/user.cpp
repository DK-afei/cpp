#include "stdafx.h"
#include "user.h"
using namespace std;
const int ISIZE=20;
const int BSIZE=100;
extern vector<user*> users;
extern string crtpath;
extern inode* inod[20];
extern block* blk[100];
extern filenode* root;
extern user* crtuser;


user::user()
{
}


user::~user()
{
}
/****************************************用户账号操作*************************************/
bool user::login(string account, string password){
	if (!users.empty()){
		for (int i = 0; i<users.size(); i++){
			if (users[i]->getUsrname()==account && users[i]->getPwd()==password){
				crtpath = users[i]->home;
				crtuser = users[i];
				return true;
			}
		}
	}
	return false;
}
bool user::adduser(string account, string password, string grp){
	if (!users.empty()){
		for (int i = 0; i<users.size(); i++){
			if (users[i]->getUsrname()==account){
				return false;
			}
		}
	}
	user* newuser=new user();
	newuser->setUsrname(account);
	newuser->setPwd(password);
	newuser->setGroup(grp);
	string crt = crtpath;
	crtpath = "/";
	mkdir(account);
	newuser->home = "/" + account+"/";
	users.push_back(newuser);
	crtpath = crt;
	return true;
}
bool user::deluser(string account){
	if (!users.empty()){
		for (int i = 0; i<users.size(); i++){
			if (users[i]->usrname == account){
				users.erase(users.begin() + i);
				return true;
			}
		}
	}
	return false;
}
bool user::chgpwd(string pwd,string account = crtuser->getUsrname()){
	//检查用户权限
	for (int i = 0; i < users.size(); i++){
		if (users[i]->getUsrname() == account){
			users[i]->setPwd(pwd);
			return true;
		}
	}
	return false;
}


/*****************************************文件操作****************************************/
int user::findInode(const int inum, string filename){
	inode* fi = inod[inum];
	int len = filename.length();

	//为了和block中保存的filename作比较
	if (len < 14){
		for (int i = 0; i < 14 - len; i++){
			filename += " ";
		}
	}

	for (int i = 0; i < 4; i++){
		if (fi->bp[i] != NULL){
			string str = fi->bp[i]->text;
			int j = 0;
			while (j <= str.length()){
				string fn = str.substr(j, 14);
				if (fn == filename){
					poslk = i;
					posct = j;
					string num = str.substr(j + 14, 2);
					int inum = atoi(num.c_str());
					return inum;
				}
				j += 16;
			}
		}
		else break;
	}

	for (int i = 0; i < 4; i++){
		if (fi->p1->bp[i] != NULL){
			string str = fi->p1->bp[i]->text;
			int j = 0;
			while (j <= str.length()){
				string fn = str.substr(j, 14);
				if (fn == filename){
					poslk = i + 4;
					posct = j;
					string num = str.substr(j + 14, 2);
					int inum = atoi(num.c_str());
					return inum;
				}
				j += 16;
			}
		}
		else break;
	}

	for (int i = 0; i < 4; i++){
		for (int k = 0; k < 4; k++){
			if (fi->p2->p1[i]->bp[k] != NULL){
				string str = fi->p2->p1[i]->bp[k]->text;
				int j = 0;
				while (j <= str.length()){
					string fn = str.substr(j, 14);
					if (fn == filename){
						poslk = k + 8 + i * 4;
						posct = j;
						string num = str.substr(j + 14, 2);
						int inum = atoi(num.c_str());
						return inum;
					}
					j += 16;
				}
			}
			else break;
		}
	}
	//目录下不存在此文件
	return -1;
}

//在存储末尾的block号追加内容 若已满需新分配
void user::append2LastBlk(const int inum, string appendText){
	inode* in = inod[inum];
	int plz = in->linknum;
	int blknum;

	//直接地址
	if (plz < 4){
		block* tem = in->bp[plz];
		if (tem->text.length() < 512){
			blknum = tem->bnum;
		}
		else{
			for (int i = 0; i < BSIZE; i++){
				//首次适配 分配空闲block
				if (!blk[i]->status){
					blk[i]->status = true;
					plz += 1;
					in->linknum = plz;
					if (plz <= 3)
						in->bp[plz] = blk[i];
					else
						in->p1->bp[0] = blk[i];
					blknum = i;
					break;					
				}
			}
		}
	}
	//一次间址
	else if (plz >= 4 && plz < 8){
		block* tem = in->p1->bp[plz - 4];
		if (tem->text.length() < 512){
			blknum = tem->bnum;
		}
		else{
			for (int i = 0; i < BSIZE; i++){
				//首次适配 分配空闲block
				if (!blk[i]->status){
					blk[i]->status = true;
					plz += 1;
					in->linknum = plz;
					if (plz <= 7)
						in->p1->bp[plz - 4] = blk[i];
					else
						in->p2->p1[0]->bp[0] = blk[i];
					blknum = i;
					break;
				}
			}
		}

	}
	//二次间址
	else if (plz >= 8 && plz < 24){
		int p1num = plz / 4 - 2;
		block* tem = in->p2->p1[p1num]->bp[plz % 4];
		if (tem->text.length() < 512){
			blknum = tem->bnum;
		}
		else{
			for (int i = 0; i < BSIZE; i++){
				//首次适配 分配空闲block
				if (!blk[i]->status){
					blk[i]->status = true;
					plz += 1;
					in->linknum = plz;
					if (plz == 24){
						/***************************inode存满**************************************/
					}
					//轮到新的p1
					if (plz % 4 == 0)
						in->p2->p1[p1num + 1]->bp[0] = blk[i];

					else
						in->p2->p1[p1num]->bp[plz % 4] = blk[i];
					blknum = i;
					break;
				}
			}
		}
	}
	//若是目录文件则16切
	if (in->permission[0]=='d'){
		block* apdblk = blk[blknum];
		apdblk->text.append(appendText);
		//cout << apdblk->text<<endl;
	}
	//非目录考虑放一半之后满了的情况 更新linknum
	else{
		block* apdblk = blk[blknum];
		int len = apdblk->text.length();
		if (appendText.length() <= 512 - len){
			apdblk->text.append(appendText);
		}
		else{
			//考虑好几个512长度(?)
			int h = 0, t = 512 - len;
			string sub = appendText.substr(h, t-h);
			apdblk->text.append(sub);
			h = t, t = appendText.length();
			for (int i = 0; i < BSIZE; i++){
				if (!blk[i]->status){
					blk[i]->status = true;
					blk[i]->text.append(appendText.substr(h, t-h));
					in->linknum++;
					if (in->linknum <= 3){
						in->bp[in->linknum] = blk[i];
					}
					else if (in->linknum > 3 && in->linknum <= 7){
						in->p1->bp[in->linknum - 4] = blk[i];
					}
					else if (in->linknum > 7 && in->linknum <= 23){
						int p1num = in->linknum / 4 - 2;
						in->p2->p1[p1num]->bp[in->linknum % 4] = blk[i];
					}
				}
			}
		}
	}
}

static int x = 0;
int user::findPath(string path=crtpath, int &prenode=x){
	//相对路径查找
	if (path == "") return 0;
	int preinode = 0;
	int inum = 0;
	if (path[0] != '/'){
		string node = "";
		for (int i = 1; i < crtpath.length(); i++){
			if (crtpath[i] == '/' || i == crtpath.length() - 1){
				if (i == crtpath.length() - 1 && crtpath[i] != '/') node += crtpath[i];
				preinode = inum;
				inum = findInode(preinode, node);
				node = "";
			}
			else node += crtpath[i];
		}

		for (int i = 0; i < path.length(); i++){
			if (path[i] == '/' || i == path.length() - 1){
				if (i == path.length() - 1 && path[i] != '/') node += path[i];
				preinode = inum;
				inum = findInode(preinode, node);
				if (inum == -1) return -1;
				node = "";

			}
			else node += path[i];
		}
	}

	//否则是绝对路径
	if(path[0]=='/'){
		string node = "";
		for (int i = 1; i < path.length(); i++){
			if (path[i] == '/' || i == path.length() - 1){
				if (i == path.length() - 1 && path[i] != '/') node += path[i];
				preinode = inum;
				inum = findInode(preinode, node);
				if (inum == -1) return -1;
				node = "";

			}
			else node += path[i];
		}
	}
	prenode = preinode;
	return inum;
}

void user::updateSize(string path, int changesize,bool pls){
	//需要倒着更新
	//先检查路径合法性
	//相对路径变绝对路径
	if (path[0] != '/') path = crtpath + path;
	int preinode = 0;
	int inum = findPath(path,preinode);
	if (inum == -1) return;

	int pos = 0;
	string p = path;
	string node = "";
	while (p.length() >= 1){
		node = "";
		pos = 0;
		for (int i = 1; i < p.length(); i++){
			if (p[i] == '/' || i == p.length() - 1){
				if (i == p.length() - 1 && p[i] != '/') node += p[i];
				if (i == p.length() - 1) break;
				node = "";
				pos = i;
			}
			else node += p[i];
		}
		p.erase(p.begin() + pos, p.end());
		inum = findPath(p);
		if (pls) inod[inum]->size += changesize;
		else inod[inum]->size -= changesize;

	}
}

void user::freeblk(const int inum){
	inode* m = inod[inum];
	int plz = m->linknum;
	if (plz < 4){
		for (int i = 0; i <= plz; i++){
			m->bp[i]->text = "";
			m->bp[i]->status = false;
			m->bp[i] = NULL;
		}
	}
	else if (plz >= 4 && plz < 8){
		for (int i = 0; i <= 3; i++){
			m->bp[i]->text = "";
			m->bp[i]->status = false;
			m->bp[i] = NULL;
		}
		for (int i = 0; i <= plz - 4; i++){
			m->p1->bp[i]->text = "";
			m->p1->bp[i]->status = false;
			m->p1->bp[i] = NULL;
		}
	}
	else if (plz >= 8){
		for (int i = 0; i <= 3; i++){
			m->bp[i]->text = "";
			m->bp[i]->status = false;
			m->bp[i] = NULL;
		}
		for (int i = 0; i <= 3; i++){
			m->p1->bp[i]->text = "";
			m->p1->bp[i]->status = false;
			m->p1->bp[i] = NULL;
		}
		for (int j = 0; j < plz / 4 - 2; j++){
			for (int i = 0; i <= plz % 4; i++){
				m->p2->p1[j]->bp[i]->text = "";
				m->p2->p1[j]->bp[i]->status = false;
				m->p2->p1[j]->bp[i] = NULL;
			}
		}
	}
	m->linknum = 0;
	m->owner = "";
	m->group = "";
	m->permission = "";
	m->size = 0;
	m->status = false;

}

int user::allocBlock(){
	int BSIZE = 100;
	for (int i = 0; i < BSIZE; i++){
		if (!blk[i]->status){
			blk[i]->status = true;
			return i;
		}
	}
}

int user::allocInode(){
	int ISIZE = 20;
	for (int i = 0; i < ISIZE; i++){
		if (!inod[i]->status){
			inod[i]->status = true;
			return i;
		}
	}
}

bool user::mkdir(string filename){
	int i;
	int len = filename.length();
	if (len < 14){
		for (int i = 0; i < 14 - len; i++)
			filename.append(" ");
	}
	if (len > 14){
		cout << "mkdir:文件名只能小于等于14位！" << endl;
		return false;
	}

	int inum = findPath(crtpath);

	int x = findInode(inum, filename);
	if (x != -1){
		cout << "mkdir:已存在的文件名！" << endl;
		return false;
	}

	x = allocInode();
	string n = to_string(x);
	if (n.length() < 2) n = " " + n;
	string appendText = filename + n;
	append2LastBlk(inum, appendText);

	inod[x]->bp[0] = blk[allocBlock()];
	inod[x]->linknum = 0;

	inod[x]->owner = crtuser->getUsrname();
	inod[x]->group = crtuser->getGroup();
	inod[x]->permission = "drwxr-xr-x";	//新建目录的默认权限
	inod[x]->size = 0;
	updateSize(filename, 16, true);
	return true;
}

bool user::cd(string path){
	//.和..

	//需要区分用户权限
	int inum = findPath(path);
	if (inum == -1){
		cout << "cd:不存在的路径！" << endl;
		return false;
	}
	if (inod[inum]->permission[0] == '-'){
		cout << "cd:必须是一个文件夹！" << endl;
		return false;
	}


	if (path[0] == '/'){
		if (path[path.length() - 1] == '/')
			crtpath = path;
		else crtpath = path + "/";
	}
	else{
		if (path[path.length() - 1] == '/')
			crtpath += path;
		else crtpath += (path + "/");
	}
	return true;
}

bool user::chown(string owner,string path){
	bool flag = false;
	for (int i = 0; i < users.size(); i++){
		if (users[i]->usrname == owner){
			flag = true;
			break;
		}
	}
	if (!flag){
		cout << "chown:不存在的用户名！" << endl;
		return false;
	}
	int inum = findPath(path);
	if (inum == -1) {
		cout << "chown:不存在的文件名！" << endl;
		return false;
	}
	inod[inum]->owner = owner;
	return true;
}

bool user::chgrp(string group,string path){
	bool flag = false;
	for (int i = 0; i < users.size(); i++){
		if (users[i]->group == group){
			flag = true;
			break;
		}
	}
	if (!flag){
		cout << "chgrp:不存在的用户组！" << endl;
		return false;
	}
	int inum = findPath(path);
	if (inum == -1) {
		cout << "chgrp:不存在的文件名！" << endl;
		return false;
	}
	inod[inum]->group = group;
	return true;
}

bool user::chmod(char mod[3], string path){
	string modstr = "";
	for (int i = 0; i < 3; i++){
		int s = mod[i] - 48;
		for (int i = 2; i >= 0; i--){
			if (s & (1 << i))
				modstr+= "1";
			else
				modstr+= "0";
		}
	}
	//cout << modstr << endl;
	string t="";
	int p = 0;
	for (int i = 0; i < modstr.length(); i++){
		char rwx='-';
		if (modstr[i] == '1'){
			if (p == 3) p = 0;
			if (p == 0) rwx = 'r';
			else if (p == 1) rwx = 'w';
			else if (p == 2) rwx = 'x';	
		}
		t += rwx;
		p++;
	}
	//cout << t << endl;

	int inum = findPath(path);
	if (inum == -1){
		cout << "chmod:不存在的文件名！" << endl;
		return false;
	}
	inod[inum]->permission = inod[inum]->permission[0] + t;
	return true;
}

void user::ls(){
	string node = "";
	int preinode = 0;
	int inum = findPath(crtpath, preinode);
	
	int plz = inod[inum]->linknum;
	if (plz <= 3){
		for (int i = 0; i <= plz; i++){
			string fn;
			string txt = inod[inum]->bp[i]->text;
			int j = 0;
			while (j <= txt.length()){
				fn = txt.substr(j, 14);
				cout << fn << "\t";
				j += 16;
			}
		}
	}
	else if (plz > 3 && plz <= 7){
		for (int i = 0; i <= 3; i++){
			string fn;
			string txt = inod[inum]->bp[i]->text;
			int j = 0;
			while (j <= txt.length()){
				fn = txt.substr(j, 14);
				cout << fn << "\t";
				j += 16;
			}
		}
		for (int i = 0; i <= plz - 4; i++){
			string fn;
			string txt = inod[inum]->p1->bp[i]->text;
			int j = 0;
			while (j <= txt.length()){
				fn = txt.substr(j, 14);
				cout << fn << "\t";
				j += 16;
			}
		}
	}
	else if (plz > 7 && plz <= 23){
		for (int i = 0; i <= 3; i++){
			string fn;
			string txt = inod[inum]->bp[i]->text;
			int j = 0;
			while (j <= txt.length()){
				fn = txt.substr(j, 14);
				cout << fn << "\t";
				j += 16;
			}
		}
		for (int i = 0; i <= 3; i++){
			string fn;
			string txt = inod[inum]->p1->bp[i]->text;
			int j = 0;
			while (j <= txt.length()){
				fn = txt.substr(j, 14);
				cout << fn << "\t";
				j += 16;
			}
		}
		for (int k = 0; k <= plz / 4 - 2; k++){
			int hl = 3;
			if (k == (plz / 4 - 2)) hl = plz % 4;
			for (int i = 0; i <= hl; i++){
				string fn;
				string txt = inod[inum]->p2->p1[k]->bp[i]->text;
				int j = 0;
				while (j <= txt.length()){
					fn = txt.substr(j, 14);
					cout << fn << "\t";
					j += 16;
				}
			}
		}
	}
	cout << endl;
}

void user::ls(string arg){
	if (arg == "-l"){
		string node = "";
		int preinode = 0;
		int inum = findPath(crtpath, preinode);
		preinode = inum;
		int plz = inod[preinode]->linknum;
		if (plz <= 3){
			for (int i = 0; i <= plz; i++){
				string fn;			
				string txt = inod[preinode]->bp[i]->text;
				int j = 0;
				int inum;
				while (j < txt.length()){
					fn = txt.substr(j, 14);	
					inum = atoi(txt.substr(j + 14, 2).c_str());
					inode* tem = inod[inum];
					cout << tem->permission << " " << tem->linknum << " " << tem->owner << " " << tem->group << " " << tem->size << " " << fn << "\n";

					j += 16;
				}
			}
		}
		else if (plz > 3 && plz <= 7){
			for (int i = 0; i <= 3; i++){
				string fn;
				string txt = inod[preinode]->bp[i]->text;
				int j = 0;
				int inum;
				while (j < txt.length()){
					fn = txt.substr(j, 14);
					inum = atoi(txt.substr(j + 14, 2).c_str());
					inode* tem = inod[inum];
					cout << tem->permission << " " << tem->linknum << " " << tem->owner << " " << tem->group << " " << tem->size << " " << fn << "\n";
					j += 16;
				}
			}
			for (int i = 0; i <= plz - 4; i++){
				string fn;
				string txt = inod[preinode]->p1->bp[i]->text;
				int j = 0;
				int inum;
				while (j < txt.length()){
					fn = txt.substr(j, 14);
					inum = atoi(txt.substr(j + 14, 2).c_str());
					inode* tem = inod[inum];
					cout << tem->permission << " " << tem->linknum << " " << tem->owner << " " << tem->group << " " << tem->size << " " << fn << "\n";
					j += 16;
				}
			}
		}
		else if (plz > 7 && plz <= 23){
			for (int i = 0; i <= 3; i++){
				string fn;
				string txt = inod[preinode]->bp[i]->text;
				int j = 0;
				int inum;
				while (j < txt.length()){
					fn = txt.substr(j, 14);
					inum = atoi(txt.substr(j + 14, 2).c_str());
					inode* tem = inod[inum];
					cout << tem->permission << " " << tem->linknum << " " << tem->owner << " " << tem->group << " " << tem->size << " " << fn << "\n";
					j += 16;
				}
			}
			for (int i = 0; i <= 3; i++){
				string fn;
				string txt = inod[preinode]->p1->bp[i]->text;
				int j = 0;
				int inum;
				while (j < txt.length()){
					fn = txt.substr(j, 14);
					inum = atoi(txt.substr(j + 14, 2).c_str());
					inode* tem = inod[inum];
					cout << tem->permission << " " << tem->linknum << " " << tem->owner << " " << tem->group << " " << tem->size << " " << fn << "\n";
					j += 16;
				}
			}
			for (int k = 0; k <= plz / 4 - 2; k++){
				int hl = 3;
				if (k == (plz / 4 - 2)) hl = plz % 4;
				for (int i = 0; i <= hl; i++){
					string fn;
					string txt = inod[preinode]->p2->p1[k]->bp[i]->text;
					int j = 0;
					int inum;
					while (j < txt.length()){
						fn = txt.substr(j, 14);
						inum = atoi(txt.substr(j + 14, 2).c_str());
						inode* tem = inod[inum];
						cout << tem->permission << " " << tem->linknum << " " << tem->owner << " " << tem->group << " " << tem->size << " " << fn << "\n";
						j += 16;
					}
				}
			}
		}
		cout << endl;
	}
}

bool user::rmdir(string path){
	int preinode = 0;
	int inum = findPath(path, preinode);
	if (inum == -1){
		cout << "rmdir:不存在的文件夹！" << endl;
		return false;
	}
	if (inod[inum]->permission[0] != 'd'){
		cout << "rmdir:删除的必须是文件夹！" << endl;
		return false;
	}
	if (inod[inum]->size != 0){
		cout << "rmdir:删除的文件夹非空！" << endl;
		return false;//非空文件夹
	}
	int plk = poslk;
	int pct = posct;
	//之后的操作会改变这两个全局变量

	updateSize(path, 16, false);	//记录文件夹的空间有16

	freeblk(inum);

	block* tblk = NULL;
	inode* fn = inod[preinode];
	if (plk < 4){
		tblk = fn->bp[plk];
	}
	else if (plk >= 4 && plk < 8){
		tblk = fn->p1->bp[plk - 4];
	}
	else if (plk >= 8){
		tblk = fn->p2->p1[plk / 4 - 2]->bp[plk % 4];
	}
	tblk->text.erase(tblk->text.begin() + pct , tblk->text.begin() + pct +16);

	return true;
}

bool user::touch(string filename){
	int len = filename.length();
	if (len > 14){
		cout << "touch:文件名只能小于等于14字节！" << endl;
		return false;
	}
	else{
		if (len<14)
			for (int i = 0; i < 14 - len; i++){
				filename += " ";
			}
	}

	string node = "";
	int preinode = 0;
	int inum = findPath(crtpath, preinode);
	
	int x = findInode(inum, filename);
	if (x != -1){
		cout << "touch:目录下存在重名文件！创建失败！" << endl;
		return false;
	}

	x = allocInode();
	string n = to_string(x);
	if (n.length() < 2) n = " " + n;
	string appendText = filename + n;
	append2LastBlk(inum, appendText);

	inod[x]->bp[0] = blk[allocBlock()];
	inod[x]->linknum = 0;

	inod[x]->owner = crtuser->getUsrname();
	inod[x]->group = crtuser->getGroup();
	inod[x]->permission = "-rwxr-xr-x";	//新建目录的默认权限
	inod[x]->size = 0;
	updateSize(filename, 16, true);
	return true;
}

bool user::rm(string path){
	int preinode = 0;
	int inum = findPath(path, preinode);
	if (inum == -1){
		cout << "rm:要删除的文件不存在！" << endl;
		return false;
	}
	if (inod[inum]->permission[0] == 'd'){
		cout << "rm:删除的必须是文件！" << endl;
		return false;
	}

	int plk = poslk;
	int pct = posct;
	//之后的操作会改变这两个全局变量

	updateSize(path, inod[inum]->size+16, false);

	string node = "";
	for (int i = 0; i < path.length(); i++){
		if (path[i] == '/' || i == path.length() - 1){
			if (i == path.length() - 1 && path[i] != '/') node += path[i];
			if (i == path.length() - 1) break;
			node = "";
		}
		else node += path[i];
	}

	block* tblk = NULL;
	inode* fn = inod[preinode];

	if (plk < 4){
		tblk = fn->bp[plk];
	}
	else if (plk >= 4 && plk < 8){
		tblk = fn->p1->bp[plk - 4];
	}
	else if (plk >= 8){
		tblk = fn->p2->p1[plk / 4 - 2]->bp[plk % 4];
	}
	tblk->text.erase(tblk->text.begin() + pct, tblk->text.begin() + pct + 16);

	freeblk(inum);
	return true;
}

bool user::mv(string newname,string old){
	//只改当前目录下的文件名
	//检查字符规范性
	for (int i = 0; i < newname.length(); i++){
		if (newname[i] == '/' || newname[i] == ' ' || newname[i] == '*' || newname[i] == '&')
			return false;
	}
	if (newname.length() > 14 || old.length()>14){
		cout << "mv:文件名超出长度限制！" << endl;
		return false;
	}
	int len = old.length();
	if (len < 14){
		for (int i = 0; i < 14 - len; i++){
			old += " ";
		}
	}
	len = newname.length();
	if (len < 14){
		for (int i = 0; i < 14 - len; i++){
			newname += " ";
		}
	}
	int preinode = 0;
	int inum = findPath(crtpath, preinode);
	
	preinode = inum;
	inum = findInode(preinode, old);
	if (inum == -1){
		cout << "mv:不存在的文件名！" << endl;
		return false;
	}
	block* tblk = NULL;
	inode* fn = inod[preinode];
	if (poslk < 4){
		tblk = fn->bp[poslk];
	}
	else if (poslk >= 4 && poslk < 8){
		tblk = fn->p1->bp[poslk - 4];
	}
	else if (poslk >= 8){
		tblk = fn->p2->p1[poslk / 4 - 2]->bp[poslk % 4];
	}
	string num = tblk->text.substr(posct + 14, 2);
	tblk->text.erase(tblk->text.begin() + posct, tblk->text.begin() + posct + 16);
	tblk->text.append(newname + num);
	return true;
	
}

bool user::cat(string path){
	int preinode = 0;
	int inum = findPath(path, preinode);

	preinode = inum;
	inode* t = inod[preinode];
	if (t->permission[0] == 'd'){
		cout << "只能查看文件内容！" << endl;
		return false;
	}
	int plz = t->linknum;
	if (plz <= 3){
		for (int i = 0; i <= plz; i++){
			cout << t->bp[i]->text;
		}
	}
	else if (plz > 3 && plz <= 7){
		for (int i = 0; i <= 3; i++){
			cout << t->bp[i]->text;
		}
		for (int i = 0; i <= plz - 4; i++){
			cout << t->p1->bp[i]->text;
		}
	}
	else if (plz > 7){
		for (int i = 0; i <= 3; i++){
			cout << t->bp[i]->text;
		}
		for (int i = 0; i <= 4; i++){
			cout << t->p1->bp[i]->text;
		}
		for (int i = 0; i <= plz / 4 - 2; i++){
			if (i != plz / 4 - 2){
				for (int k = 0; k <= 3; k++){
					cout << t->p2->p1[i]->bp[k]->text;
				}
			}
			else{
				for (int k = 0; k <= plz % 4; k++){
					cout << t->p2->p1[i]->bp[k]->text;
				}
			}
		}
	}
	cout << endl;
	return true;
}

bool user::cp(string path1, string path2){
	string fn1, fn2;
	int pre1 = 0;
	int inum1 = findPath(path1, pre1);
	//源文件不存在
	if (inum1 == -1){
		cout << "cp:不存在的源文件!" << endl;
		return false;
	}
	//不支持文件夹复制
	else if (inod[inum1]->permission[0] == 'd'){
		cout << "cp:无法复制文件夹!" << endl;
		return false;
	}
	int size = inod[inum1]->size;

	string node = "";
	for (int i = 0; i < path1.length(); i++){
		if (path1[i] == '/' || i == path1.length() - 1){
			if (i == path1.length() - 1 && path1[i] != '/') node += path1[i];
			if (i == path1.length() - 1) break;
			node = "";
		}
		else node += path1[i];
	}
	fn1 = node;

	int pre2 = 0;
	bool dir = false;
	int inum2 = findPath(path2, pre2);
	//若目标路径带文件名，则查找父目录下有无同名文件，若只是目录名字则查找目录是否存在。
	if (inum2 != -1){
		if (inod[inum2]->permission[0] != 'd'){
			cout << "cp:目的地址需是一个不存在的文件或是文件夹！" << endl;
			return false;
		}
		else dir = true;
	}

	string crt = crtpath;
	int inum = 0;
	node = "";
	if (dir){	
		if (!cd(path2)) return false;
		else touch(fn1);

		inum = findInode(inum2, fn1);
	}
	else{
		for (int i = 0; i < path2.length(); i++){
			if (path2[i] == '/' || i == path2.length() - 1){
				if (i == path2.length() - 1 && path2[i] != '/') node += path2[i];
				if (i == path2.length() - 1) break;
				node = "";
			}
			else node += path2[i];
		}
		fn2 = node;
		
		string path22 = path2;
		path22.erase(path22.end() - fn2.length(), path22.end());
		if (path22[path22.length() - 1 == '/']) path22.erase(path22.end() - 1, path22.end());
		
		if (path22.length() == 0) path22 = crt;
		int pnum = findPath(path22);
		if (pnum == -1){
			cout << "cp:不合法的目的文件地址！" << endl;
			return false;
		}
		cd(path22);
		touch(fn2);
		inum2 = findPath(fn2, pre2);
		inum = inum2;
	}
	if (inum == -1){
		cout << "cp:不合法的文件地址!" << endl;
		return false;
	}
	inod[inum]->permission = inod[inum1]->permission;
	inod[inum]->linknum = inod[inum1]->linknum;
	inod[inum]->group = inod[inum1]->group;
	inod[inum]->owner = inod[inum1]->owner;
	inod[inum]->size = inod[inum1]->size;

	int plz = inod[inum]->linknum;
	if (plz <= 3){
		for (int i = 0; i <= plz; i++){
			int blknum=allocBlock();
			blk[blknum] = inod[inum1]->bp[i];
			inod[inum]->bp[i] = blk[blknum];
		}
	}
	else if (plz > 3 && plz <= 7){
		for (int i = 0; i <= 3; i++){
			int blknum = allocBlock();
			blk[blknum] = inod[inum1]->bp[i];
			inod[inum]->bp[i] = blk[blknum];
		}
		for (int i = 0; i <= plz-4; i++){
			int blknum = allocBlock();
			blk[blknum] = inod[inum1]->p1->bp[i];
			inod[inum]->p1->bp[i] = blk[blknum];
		}
	}
	else if (plz > 7 ){
		for (int i = 0; i <= 3; i++){
			int blknum = allocBlock();
			blk[blknum] = inod[inum1]->bp[i];
			inod[inum]->bp[i] = blk[blknum];
		}
		for (int i = 0; i <= 3; i++){
			int blknum = allocBlock();
			blk[blknum] = inod[inum1]->p1->bp[i];
			inod[inum]->p1->bp[i] = blk[blknum];
		}
		for (int i = 0; i <= 3; i++){
			int blknum = allocBlock();
			blk[blknum] = inod[inum1]->bp[i];
			inod[inum]->bp[i] = blk[blknum];
		}
		for (int i = 0; i <= plz / 4 - 2; i++){
			int p = 3;
			if (i == plz / 4 - 2) p = plz % 4;
			for (int k = 0; k <= p; k++){
				int blknum = allocBlock();
				blk[blknum] = inod[inum1]->p2->p1[i]->bp[k];
				inod[inum]->p2->p1[i]->bp[k] = blk[blknum];
			}		
		}
	}
	crtpath = crt;
	updateSize(path2, size,true);
	return true;
}

void user::vi(string path, string txt){
	int inum = findPath(path);
	if (inum == -1){
		cout << "vi:不存在的文件！" << endl;
		return;
	}
	if (inod[inum]->permission[0] == 'd'){
		cout << "vi:不能对文件夹进行操作！" << endl;
		return;
	}
	
	append2LastBlk(inum, txt);
	inod[inum]->size += txt.length();
	updateSize(path, txt.length(), true);
}
/***************************************getter&setter*************************************/

void user::setUsrname(string str){
	usrname = str;
}
void user::setPwd(string str){
	pwd = str;
}
void user::setGroup(string str){
	group = str;
}
string user::getUsrname(){
	return usrname;
}
string user::getPwd(){
	return pwd;
}
string user::getGroup(){
	return group;
}

void user::printinfo(){
	cout << "用户名：" << getUsrname() << endl;
	cout << "密码：" << getPwd() << endl;
	cout << "所属组：" << getGroup() << endl;
	cout << "家目录：" << home << endl;

}