#include<iostream>
#include <cstddef> 
#include"memoryblock.h"

MemoryBlock::MemoryBlock(const size_t blockSize, BlockNodePool& nodePoll)
	:m_blockSize(blockSize), m_blockStart(nullptr), m_blockEnd(nullptr), m_nextBlock(nullptr), m_startBlockNode(nullptr),m_nodePool(nodePoll)
{
	//�����ڴ��
	m_realAdress = new std::byte[blockSize];
	if (m_realAdress == nullptr)
	{
		throw std::bad_alloc();
	}
	//��ʼ����һ���ڴ�ڵ�Ϊ�����ڴ��
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
			//�ҵ���С���ú��ʵĽڵ�
			if (currentNode->getBlockNodeSize() == blockNodeSize)
			{
				currentNode->setUsed(true);
				return reinterpret_cast<std::byte*>(currentNode);
			}
			//�ҵ���С���������ڴ�Ľڵ㣬�ֳ������貿����Ϊ�½ڵ㣬ʣ�µ���Ϊ��һ���ڵ����ں��
			BlockNode* tempNode = m_nodePool.AllocateNode(currentNode->getStart(), blockNodeSize);
			BlockNode* newNode = m_nodePool.AllocateNode(currentNode->getStart() + blockNodeSize, currentNode->getBlockNodeSize() - blockNodeSize);
			tempNode->setUsed(true);
			//�ڵ��������
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

			// �ϲ�ǰһ�����нڵ�
			BlockNode* prevNode = currentNode->getLast();
			if (prevNode != nullptr && !prevNode->isUsed())
			{
				// �ϲ� prevNode �� currentNode
				prevNode->setEnd(currentNode->getEnd());
				prevNode->setNext(currentNode->getNext());
				if (currentNode->getNext() != nullptr) {
					currentNode->getNext()->setLast(prevNode);
				}
				m_nodePool.FreeNode(currentNode);
				currentNode = prevNode;
			}

			// �ϲ���һ�����нڵ�
			BlockNode* nextNode = currentNode->getNext();
			if (nextNode != nullptr && !nextNode->isUsed())
			{
				// �ϲ� currentNode �� nextNode
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