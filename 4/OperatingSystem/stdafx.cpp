// stdafx.cpp : 只包括标准包含文件的源文件
// OperatingSystem.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO:  在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


const int ISIZE = 20;
const int BSIZE = 100;

vector<user*> users;
string crtpath = "";

inode* inod[ISIZE];
block* blk[BSIZE];
cls1* c1blk[ISIZE];
cls2* c2blk[ISIZE];


user* cyan=new user();	//root
user* crtuser = cyan;

