#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include"FileCompress.hpp"
#include"HuffmanTree.hpp"
#include<windows.h>
void TestFileCompress()
{

	FileCompress  f;
	f.Compress("File.txt");
}
void TestFileUnCompress()
{
	FileCompress  t;
	t.UnCompress("File.txt");
}

int main()
{
	
	TestFileCompress();
	TestFileUnCompress();
	system("pause");
	return 0;
}


