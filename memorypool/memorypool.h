#pragma once 
#include<iostream>
#include<cstddef>
#include"memoryblock.h"
#include"blocknodepool.h"
#define MEMORY_POLL_SIZE 10*1024*1024
class MemoryPool
{
public:
	MemoryPool(const size_t blocksize,const size_t pollBlockNodeNum);//指定块大小和块节点池内节点数量
	~MemoryPool();
	void* Allocate(const size_t allocatedSize);
	bool deAllocate(void* ptr);
	void printStatus() const;
	void PrintMemoryLeaks()const;
private:
	size_t m_memorySize;//内存池大小
	size_t m_blockSize;//单个内存块大小
	MemoryBlock* m_blockStart;//内存块链表第一个节点
	size_t m_currentMemorySize;//已用空间
	BlockNodePool m_blockNodePool;
};