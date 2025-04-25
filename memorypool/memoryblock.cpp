#include<iostream>
#include <cstddef> 
#include"memoryblock.h"

MemoryBlock::MemoryBlock(const size_t blockSize, BlockNodePool& nodePoll)
	:m_blockSize(blockSize), m_blockStart(nullptr), m_blockEnd(nullptr), m_nextBlock(nullptr), m_startBlockNode(nullptr),m_nodePool(nodePoll)
{
	//申请内存块
	m_realAdress = new std::byte[blockSize];
	if (m_realAdress == nullptr)
	{
		throw std::bad_alloc();
	}
	//初始化第一个内存节点为整个内存块
	m_startBlockNode = new BlockNode(m_realAdress, m_blockSize);
}

MemoryBlock::~MemoryBlock()
{
	delete[] m_realAdress;  
}

std::byte* MemoryBlock::Allocate(size_t blockNodeSize)
{
	BlockNode* currentNode = m_startBlockNode;
	while (currentNode != nullptr)
	{
		if (currentNode->getBlockNodeSize() >= blockNodeSize && currentNode->isUsed() == false)
		{
			//找到大小正好合适的节点
			if (currentNode->getBlockNodeSize() == blockNodeSize)
			{
				currentNode->setUsed(true);
				return reinterpret_cast<std::byte*>(currentNode);
			}
			//找到大小大于所需内存的节点，分出来所需部分作为新节点，剩下的作为另一个节点连在后边
			BlockNode* tempNode = m_nodePool.AllocateNode(currentNode->getStart(), blockNodeSize);
			BlockNode* newNode = m_nodePool.AllocateNode(currentNode->getStart() + blockNodeSize, currentNode->getBlockNodeSize() - blockNodeSize);
			tempNode->setUsed(true);
			//节点插入链表
			tempNode->setNext(newNode);
			tempNode->setLast(currentNode->getLast());
			newNode->setNext(currentNode->getNext());
			newNode->setLast(tempNode);
			if (currentNode->getNext() != nullptr)
			{
				currentNode->getNext()->setLast(newNode);
			}
			if (currentNode->getLast() != nullptr)
			{
				currentNode->getLast()->setNext(tempNode);
			}
			else
			{
				m_startBlockNode = tempNode;
			}
			m_nodePool.FreeNode(currentNode);
			currentNode = tempNode;
			tempNode = nullptr;
			return currentNode->getStart();
		}
		currentNode = currentNode->getNext();
	}
	return nullptr;
}

bool MemoryBlock::deAllocate(const std::byte* deallocateNode,size_t& deNodeSize) {
	BlockNode* currentNode = m_startBlockNode;
	while (currentNode != nullptr) {
		if (currentNode->getStart() == deallocateNode)
		{
			currentNode->setUsed(false);
			deNodeSize = currentNode->getBlockNodeSize();

			// 合并前一个空闲节点
			BlockNode* prevNode = currentNode->getLast();
			if (prevNode != nullptr && !prevNode->isUsed())
			{
				// 合并 prevNode 和 currentNode
				prevNode->setEnd(currentNode->getEnd());
				prevNode->setNext(currentNode->getNext());
				if (currentNode->getNext() != nullptr) {
					currentNode->getNext()->setLast(prevNode);
				}
				m_nodePool.FreeNode(currentNode);
				currentNode = prevNode;
			}

			// 合并后一个空闲节点
			BlockNode* nextNode = currentNode->getNext();
			if (nextNode != nullptr && !nextNode->isUsed())
			{
				// 合并 currentNode 和 nextNode
				currentNode->setEnd(nextNode->getEnd());
				currentNode->setNext(nextNode->getNext());
				if (nextNode->getNext() != nullptr) {
					nextNode->getNext()->setLast(currentNode);
				}
				m_nodePool.FreeNode(nextNode);
			}
			return true;
		}
		currentNode = currentNode->getNext();
	}
	return false;
}


MemoryBlock* MemoryBlock::getNextBlock()const
{
	return m_nextBlock;
}
void MemoryBlock::setNextBlock(MemoryBlock* newBlock)
{
	this->m_nextBlock = newBlock;
}
std::byte* MemoryBlock::getRealAdress()const
{
	return m_realAdress;
}
BlockNode* MemoryBlock::getStartBlockNode()const
{
	return m_startBlockNode;
}