#pragma once 
#include<iostream>
#include<cstddef>
#include"memoryblock.h"
#include"blocknodepool.h"
#define MEMORY_POLL_SIZE 10*1024*1024
class MemoryPool
{
public:
	MemoryPool(const size_t blocksize,const size_t pollBlockNodeNum);//ָ�����С�Ϳ�ڵ���ڽڵ�����
	~MemoryPool();
	void* Allocate(const size_t allocatedSize);
	bool deAllocate(void* ptr);
	void printStatus() const;
	void PrintMemoryLeaks()const;
private:
	size_t m_memorySize;//�ڴ�ش�С
	size_t m_blockSize;//�����ڴ���С
	MemoryBlock* m_blockStart;//�ڴ�������һ���ڵ�
	size_t m_currentMemorySize;//���ÿռ�
	BlockNodePool m_blockNodePool;
};