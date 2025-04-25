#pragma once 
#include<cstddef>
#include"blocknode.h"
#include"blocknodepool.h"
class MemoryBlock
{
public:
	MemoryBlock(const size_t blockSize,BlockNodePool& nodePoll );
	~MemoryBlock();
	std::byte* Allocate(size_t blockNodeSize);
	bool deAllocate(const std::byte* deallocateNode, size_t& deNodeSize);
	MemoryBlock* getNextBlock()const;
	void setNextBlock(MemoryBlock* newBlock);
	std::byte* getRealAdress()const;
	BlockNode* getStartBlockNode()const;
private:
	size_t m_blockSize;
	std::byte* m_realAdress;
	std::byte* m_blockStart;
	std::byte* m_blockEnd;
	MemoryBlock* m_nextBlock;
	BlockNode* m_startBlockNode;//内存结点链表
	BlockNodePool& m_nodePool;

	
};
