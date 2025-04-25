#pragma once 
#include<iostream>
#include<cstddef>
//双向链表结构存储内存块节点
class BlockNode
{
public:
	BlockNode() :m_blockNodeSize(0), m_start(nullptr), m_end(nullptr), m_last(nullptr), m_next(nullptr), m_used(false) {}
	BlockNode(std::byte* start, size_t blockNodeSize)
		:m_blockNodeSize(blockNodeSize), m_start(start), m_end(start + blockNodeSize), m_last(nullptr), m_next(nullptr), m_used(false)
	{
		if (start == nullptr)
		{
			throw std::bad_alloc();
		}
	}
	~BlockNode() {};

	size_t getBlockNodeSize() const{ return m_blockNodeSize; }
	bool isUsed() const{ return m_used; }
	std::byte* getStart() const{ return m_start; }
	std::byte* getEnd() const { return m_end; }
	BlockNode* getLast() const { return m_last; }
	BlockNode* getNext() const { return m_next; }

	void setBlockNodeSize(size_t size)
	{
		m_blockNodeSize = size;
	}

	void setUsed(bool used)
	{
		m_used = used;
	}

	void  setStart(std::byte* start)
	{
		m_start = start;
	}

	void setEnd(std::byte* end) 
	{
		m_end = end;
	}

	void setLast(BlockNode* last)
	{
		m_last = last;

	}

	void setNext(BlockNode* next)
	{
		m_next = next;
	}

	
private:
	size_t m_blockNodeSize;
	std::byte* m_start;
	std::byte* m_end;
	BlockNode* m_last;
	BlockNode* m_next;
	bool m_used;
};