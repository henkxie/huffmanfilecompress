#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include<iostream>
#include<cassert>
#include<vector>
using namespace std;

template<class T>
struct Less
{
	bool operator() (const T& l, const T& r)
	{
		return l < r; // operator<
	}
};
template<class T>
struct Greater
{
	bool operator() (const T& l, const T& r)
	{
		return l > r; // operator<
	}
};
template<class T, class Compare = Less<T>>
class Heap
{
public:
	Heap()
	{

	}
	Heap(T*a, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			_array.push_back(a[i]);

		}
		//½¨¶Ñ
		for (int i = (_array.size() - 2) / 2; i >= 0; --i)
		{
			AdjustDown(i);
		}
	}
	void AdjustDown(int root)
	{
		int child = 2 * root + 1;
		Compare com;
		while (child < _array.size())
		{

			if (child + 1 < _array.size() && com(_array[child + 1], _array[child]))
			{
				++child;
			}
			if (com(_array[child], _array[root]))
			{
				swap(_array[child], _array[root]);
				root = child;
				child = 2 * root + 1;
			}
			else
			{
				break;
			}
		}

	}
	void AdjustUp(int child)
	{
		int parent = (child - 1) / 2;
		while (child > 0)
		{
			if (Compare()(_array[child], _array[parent]))
			{
				swap(_array[child], _array[parent]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}
	void Push(const T &x)
	{

		//assert(_array.size() > 1);
		_array.push_back(x);
		AdjustUp(_array.size() - 1);
	}
	void Pop()
	{
		assert(_array.size() > 0);
		swap(_array[0], _array[_array.size() - 1]);
		_array.pop_back();
		AdjustDown(0);
	}
	T& Top()
	{
		return _array[0];
	}
	size_t Size()
	{
		return _array.size();
	}
	bool Empty()
	{
		return _array.empty();
	}
	void Print()
	{
		for (size_t i = 0; i < _array.size(); i++)
		{
			cout << _array[i] << " ";
		}
		cout << endl;
	}
public:
	vector<T> _array;

};


