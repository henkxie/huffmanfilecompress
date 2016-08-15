#define _CRT_SECURE_NO_WARNINGS 
#pragma once

#include "Heap.h"
#include<queue>

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode<T>*_left;
	HuffmanTreeNode<T>*_right;
	HuffmanTreeNode<T>*_parent;
	T _weight;
	HuffmanTreeNode(const T &x)
		: _weight(x)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};
template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
	template<class T>
	struct CompareNode     //仿函数
	{
		bool operator()(Node*l, Node*r)
		{
			return l->_weight < r->_weight;
		}
	};
public:
	HuffmanTree()
		:_root(NULL)
	{}
	~HuffmanTree()
	{}

	void CreateTree(T *a, size_t size, const T& invalid)
	{
		assert(a);
		Heap<Node*, CompareNode<T>>minHeap;//仿函数的运用
		for (size_t i = 0; i < size; i++)
		{
			if (a[i] != invalid)
			{
				Node *node = new Node(a[i]);
				minHeap.Push(node);
			}
		}

		while (minHeap.Size()>1)
		{
			Node*left = minHeap.Top();
			minHeap.Pop();
			Node*right = minHeap.Top();
			minHeap.Pop();
			Node*parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;

			minHeap.Push(parent);
		}
		_root = minHeap.Top();

	}
	Node* GetRootNode()
	{
		return _root;
	}
	void LevelOrder(int *a, size_t size)
	{
		queue<Node*> q;
		if (_root != NULL)
		{
			q.push(_root);
		}
		while (!q.empty())
		{
			Node*front = q.front();
			cout << front;
			if (_root->_left)
			{
				q.push(_root->_left);
			}
			if (_root->_right)
			{
				q.push(_root->_right);
			}
		}
		cout << endl;
	}

protected:
	HuffmanTreeNode<T>*_root;
};
void Test()
{
	int a[] = {1,2,3};
	HuffmanTree<int>t;
	t.CreateTree(a, sizeof(a) / sizeof(int), -1);
	
	//t.LevelOrder(a, 10);
}
