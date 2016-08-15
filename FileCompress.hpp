#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include"HuffmanTree.hpp"
#include<algorithm>
#include<assert.h>
#include<cstring>
#include<stdlib.h>
typedef long longtype;
struct Fileinfo       
{
	unsigned char _ch;//字符
	longtype _count;//个数
	string _code;//对应的huffman编码
	Fileinfo(unsigned char ch = 0)
		:_ch(ch)
		, _count(0)
	{}
	bool operator<(const Fileinfo& info)
	{
		return this->_count < info._count;
	}

	Fileinfo operator+(const Fileinfo& info)
	{
		Fileinfo tmp;
		tmp._count = this->_count + info._count;
		return tmp;
	}

	bool operator != (const Fileinfo& info) const
	{
		return this->_count != info._count;
	}

};
class FileCompress
{

	// 1.打开文件，统计文件字符出现的字数
	// 2.生成对应的Huffman树
	// 3.获取对应的Huffman编码
	// 4.压缩文件
	// 5.写配置文件，方便后续的解压缩
public:

	FileCompress()
	{
		for (int i = 0; i < 256; i++)
		{
			_info[i]._ch = i;//初始化_info；
		}
	}
public:
	//***********************文件压缩***********************
	bool Compress(const char*filename)
	{
		//1.读文件
		assert(filename);
		FILE * fp = fopen(filename, "rb");
		assert(fp);
		 unsigned char c;
		 fread(&c, sizeof(unsigned char), 1, fp);
		while (!feof(fp))
		{
			_info[c]._count++;//统计字符个数
			fread(&c, sizeof(unsigned char), 1, fp);

		}
		fseek(fp, 0, SEEK_SET);    //将文件指针重置为文件开头位置
		//根据权重构建哈弗曼树
		HuffmanTree<Fileinfo> t;
		Fileinfo invalid;
		t.CreateTree(_info, 256, invalid);
		//获取哈弗曼编码
		_GenerateHuffmanCode(t.GetRootNode());
		//文件的压缩
		_Compress(filename, fp);
		//写配置文件
		//将_info[256] 中的信息写入文件，保存哈夫曼编码，以供解码时使用
			string Configfile = filename;   
			Configfile += ".configer";
			FILE * fc = fopen(Configfile.c_str(), "w");
			assert(fc);
			string instr;
			for (int i = 0; i < 256; ++i)
			{
				if (_info[i]._count>0)
				{
					char buffer[128];
					instr += _info[i]._ch;
					instr += ',';
					_itoa(_info[i]._count, buffer, 10);
					instr += buffer;
					instr += '\n';
					fputs(instr.c_str(), fc);
					instr.clear();	
				}
			}
			fclose(fc);
			return true;
		}
public:
	template<class T>
	void _GenerateHuffmanCode(HuffmanTreeNode<T>* root)//获取huffman编码(逆序)
	{
		if (root == NULL)
			return;
		if (root)
		{
			_GenerateHuffmanCode(root->_left);

			_GenerateHuffmanCode(root->_right);

			// 如果当前节点是叶子结点，则生成对应的Huffman编码
			if (root->_left == NULL && root->_right == NULL)
			{
				HuffmanTreeNode<T>* cur = root;
				HuffmanTreeNode<T>* parent = cur->_parent;
				string& code = _info[cur->_weight._ch]._code;//&code就是_info[cur->_weight._ch]._code的别名
				while (parent)
				{
					if (parent->_left == cur)
						code += '0';
					else
						code += '1';

					cur = parent;
					parent = cur->_parent;
				}
				reverse(code.begin(), code.end());

			}
		}
	}
	void _Compress(const char* filename, FILE *fp)
	{
		string filecompress = filename ; //压缩文件
		filecompress += ".huffman";
		FILE * fw = fopen(filecompress.c_str(), "wb");
		assert(fw);
		 unsigned char c;
		unsigned char ch = 0;   //向压缩文件中写的字符
		size_t index=0 ;       //二进制的位数.......
		fread(&c, sizeof(unsigned char), 1, fp); 
		//c = fgetc(fp);//能读到eof不好处理
		while (!feof(fp))
		{
			string& code = _info[(unsigned char)c]._code;
			for (int i = 0; i < code.size(); ++i)
			{
				ch <<= 1;
				++index;
				if (code[i] == '1') 
					ch |= 1;
				if (index == 8)
				{
					fwrite(&ch, sizeof(unsigned char), 1, fw);
					ch = 0;
					index = 0;
				}
			}
			fread(&c, sizeof(unsigned char), 1, fp);
			//c = fgetc(fp);

		}

		if (index > 0 && index < 8)
		{
			ch <<= (8 - index);
			fwrite(&ch, sizeof(char), 1, fw);
			//fputc(ch, fw);

		}
		fclose(fw);
		fclose(fp);
	}
	
	//*********************解压缩**********************
	bool UnCompress(const string& filename)
	{
		string Configfile = filename;   //配置文件
		Configfile += ".configer";
		FILE * fconfiger = fopen(Configfile.c_str(), "r");
		assert(fconfiger);
		string line;

		 long long chCount = 0;
		while (_ReadLine(fconfiger, line))
		{
			if (line.empty())
			{
				line += '\n';
				if (_ReadLine(fconfiger, line))
				{
					_info[10]._count = atoi(line.substr(2).c_str());
					chCount += _info[10]._count;
				}
			}
			else
			{
				unsigned char _ch = line[0];
				int count = atoi(line.substr(2).c_str());
				_info[_ch]._count = count;
				chCount += count;
			}
			
			line.clear();
		}
		//根据权重构建哈夫曼树
		HuffmanTree<Fileinfo> t;
		Fileinfo invalid;
		t.CreateTree(_info, 256, invalid);
		HuffmanTreeNode<Fileinfo>* root = t.GetRootNode();


		//解压缩
		string filecompress = filename; //压缩文件
		filecompress += ".huffman";
		FILE * fpCompress = fopen(filecompress.c_str(), "rb");
		assert(fpCompress);
		string uncompressFile = filename;
		uncompressFile += ".uncompress";
		FILE* fInUncompress = fopen(uncompressFile.c_str(), "w");
		assert(fInUncompress);
		unsigned char inch;
		fread(&inch, sizeof(unsigned char), 1, fpCompress);
		HuffmanTreeNode<Fileinfo>* cur = root;
		int pos = 8;
		while (1)
		{
			--pos;

			if (inch & (1 << pos))
			{
				cur = cur->_right;
			}
			else
			{
				cur = cur->_left;
			}

			if (cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight._ch, fInUncompress);
				if (--chCount == 0)
				{
					break;
				}
				cur = root;
			}

			if (pos == 0)
			{
				fread(&inch, sizeof(unsigned char), 1, fpCompress);
				pos = 8;
			}
		}

		return true;
	}
	public:
		bool _ReadLine(FILE * fout, string& line)
		{
		
			char ch = fgetc(fout);
			if (ch == EOF)
				return false;

			while (ch != EOF && ch != '\n')
			{
				line += ch;
				ch = fgetc(fout);
			}
			return true;
		}
		
private:
	Fileinfo _info[256];
};
