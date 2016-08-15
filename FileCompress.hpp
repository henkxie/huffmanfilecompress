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
	unsigned char _ch;//�ַ�
	longtype _count;//����
	string _code;//��Ӧ��huffman����
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

	// 1.���ļ���ͳ���ļ��ַ����ֵ�����
	// 2.���ɶ�Ӧ��Huffman��
	// 3.��ȡ��Ӧ��Huffman����
	// 4.ѹ���ļ�
	// 5.д�����ļ�����������Ľ�ѹ��
public:

	FileCompress()
	{
		for (int i = 0; i < 256; i++)
		{
			_info[i]._ch = i;//��ʼ��_info��
		}
	}
public:
	//***********************�ļ�ѹ��***********************
	bool Compress(const char*filename)
	{
		//1.���ļ�
		assert(filename);
		FILE * fp = fopen(filename, "rb");
		assert(fp);
		 unsigned char c;
		 fread(&c, sizeof(unsigned char), 1, fp);
		while (!feof(fp))
		{
			_info[c]._count++;//ͳ���ַ�����
			fread(&c, sizeof(unsigned char), 1, fp);

		}
		fseek(fp, 0, SEEK_SET);    //���ļ�ָ������Ϊ�ļ���ͷλ��
		//����Ȩ�ع�����������
		HuffmanTree<Fileinfo> t;
		Fileinfo invalid;
		t.CreateTree(_info, 256, invalid);
		//��ȡ����������
		_GenerateHuffmanCode(t.GetRootNode());
		//�ļ���ѹ��
		_Compress(filename, fp);
		//д�����ļ�
		//��_info[256] �е���Ϣд���ļ���������������룬�Թ�����ʱʹ��
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
	void _GenerateHuffmanCode(HuffmanTreeNode<T>* root)//��ȡhuffman����(����)
	{
		if (root == NULL)
			return;
		if (root)
		{
			_GenerateHuffmanCode(root->_left);

			_GenerateHuffmanCode(root->_right);

			// �����ǰ�ڵ���Ҷ�ӽ�㣬�����ɶ�Ӧ��Huffman����
			if (root->_left == NULL && root->_right == NULL)
			{
				HuffmanTreeNode<T>* cur = root;
				HuffmanTreeNode<T>* parent = cur->_parent;
				string& code = _info[cur->_weight._ch]._code;//&code����_info[cur->_weight._ch]._code�ı���
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
		string filecompress = filename ; //ѹ���ļ�
		filecompress += ".huffman";
		FILE * fw = fopen(filecompress.c_str(), "wb");
		assert(fw);
		 unsigned char c;
		unsigned char ch = 0;   //��ѹ���ļ���д���ַ�
		size_t index=0 ;       //�����Ƶ�λ��.......
		fread(&c, sizeof(unsigned char), 1, fp); 
		//c = fgetc(fp);//�ܶ���eof���ô���
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
	
	//*********************��ѹ��**********************
	bool UnCompress(const string& filename)
	{
		string Configfile = filename;   //�����ļ�
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
		//����Ȩ�ع�����������
		HuffmanTree<Fileinfo> t;
		Fileinfo invalid;
		t.CreateTree(_info, 256, invalid);
		HuffmanTreeNode<Fileinfo>* root = t.GetRootNode();


		//��ѹ��
		string filecompress = filename; //ѹ���ļ�
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
