#pragma once
#include<iostream>
#include<string>
#include<vector>
using namespace std;
class block{
public:
	//非目录文件则使用文件内容
	int bnum;
	bool status=false;
	string text;//length=512;
	//目录文件维护文件数组

};
class cls1{
public:
	block* bp[4];
	cls1(){
		for (int i = 0; i < 4; i++){
			bp[i] = NULL;
		}
	};
};
class cls2{
public:
	cls1* p1[4];
	cls2(){
		for (int i = 0; i < 4; i++){
			p1[i] = new cls1();
		}
	};
};
class inode{
public:
	bool status=false;
	string owner="";	//length=20
	string group="";	//length=20
	int linknum=0;	//用来表明使用到哪个存储节点 默认每个文件都有一个block
	int size=0;
	block* bp[4];//直接地址 
	cls1* p1;//一次间址 
	cls2* p2;//两次间址 
	string permission="";//-rw-r--r-- length=10
	//string time;

	inode(){
		for (int i = 0; i < 4; i++){
			bp[i] = NULL;
		}
		p1 = new cls1();
		p2 = new cls2();
	};
};
class filenode
{
	
};

