#include<iostream>
#include<cstddef>
#include"blocknodepool.h"
#include"memorypool.h"


MemoryPool::MemoryPool(const size_t blocksize, const size_t pollBlockNodeNum)
	:m_blockSize(blocksize), m_blockStart(nullptr), m_memorySize(MEMORY_POLL_SIZE), m_currentMemorySize(0), m_blockNodePool(pollBlockNodeNum)
{
	if (m_memorySize % m_blockSize != 0 || m_blockSize > m_memorySize)
	{
		throw std::runtime_error("�ڴ�ز���ƽ���ָ��˴�С���ڴ��");
	}
}

MemoryPool::~MemoryPool()
{
	PrintMemoryLeaks();//��ӡ�ڴ�й¶���һ��Ҫ�������ڴ��֮ǰ����ȻԽ����
	MemoryBlock* currentBlock = m_blockStart;
	while (currentBlock != nullptr)
	{
		MemoryBlock* nextBlock = currentBlock->getNextBlock();
		delete currentBlock;
		currentBlock = nextBlock;
	}
	
}

void* MemoryPool::Allocate(const size_t allocatedSize)
{
	if (allocatedSize <= m_blockSize)
	{
		//������Ϊ�գ�ֱ�Ӵ����¿�
		if (m_blockStart == nullptr)
		{
			m_blockStart = new MemoryBlock(m_blockSize,m_blockNodePool);
			if (m_blockStart == nullptr)
			{
				throw std::bad_alloc();
			}
		}
		//�������ڴ�������
		MemoryBlock* currentBlock = m_blockStart;
		while (currentBlock != nullptr)
		{
			std::byte* ptr = currentBlock->Allocate(allocatedSize);
			if (ptr != nullptr)
			{
				m_currentMemorySize += allocatedSize;
				return reinterpret_cast<void*>(ptr);
			}
			currentBlock = currentBlock->getNextBlock();

		}
		//�����ڴ���������ʧ�ܣ��������ڴ�����
		if (m_currentMemorySize + m_blockSize <= m_memorySize)
		{
			MemoryBlock* newBlock = new MemoryBlock(m_blockSize, m_blockNodePool);
			std::byte* ptr = newBlock->Allocate(allocatedSize);
			if (ptr != nullptr)
			{
				//ͷ�巨�����ڴ���������
				newBlock->setNextBlock(m_blockStart);
				m_blockStart = newBlock;
				newBlock = nullptr;
				m_currentMemorySize += allocatedSize;
				return reinterpret_cast<void*>(ptr);
			}
			else
			{
				delete newBlock;
				return nullptr;
			}
		}
		else
		{
			throw std::runtime_error("�ڴ������");
			return nullptr;
		}
	}
	else
	{
		MemoryBlock* newBlock = new MemoryBlock(allocatedSize, m_blockNodePool);
		if (newBlock == nullptr)
		{
			throw std::bad_alloc();
		}
		//ͷ�巨�����ڴ���������
		std::byte* ptr = newBlock->Allocate(allocatedSize);
		newBlock->setNextBlock(m_blockStart);
		m_blockStart = newBlock;
		newBlock = nullptr;
		m_currentMemorySize += allocatedSize;
		return reinterpret_cast<void*>(ptr);
	}
}

bool MemoryPool::deAllocate(void* ptr)
{
	std::byte* deallocateNode = reinterpret_cast<std::byte*>(ptr);
	if (ptr == nullptr)
	{
		throw std::runtime_error("�����ڴ��ַΪ��");
		return false;
	}
	MemoryBlock* currentBlock = m_blockStart;
	while (currentBlock)
	{
		size_t deNodeSize = 0;
		if (currentBlock->deAllocate(deallocateNode,deNodeSize) == true)
		{
			m_currentMemorySize -= deNodeSize;
			return true;
		}
		currentBlock = currentBlock->getNextBlock();
	}
	return false;
}

// ����ڴ��״̬
void MemoryPool::printStatus() const
{
	std::cout << "\n\n--------Memory Pool Status---------\n";
	std::cout << "Total Size: " << m_memorySize << " bytes\n";
	std::cout << "Used Size: " << m_currentMemorySize << " bytes\n";
	std::cout << "Block Size: " << m_blockSize << " bytes\n";
	MemoryBlock* current = m_blockStart;
	int blockIdx = 0;
	while (current)
	{
		std::cout << "\nBlock #" << ++blockIdx << " at " << current->getRealAdress() << ":\n";
		BlockNode* node = current->getStartBlockNode();
		int nodeIdx = 0;
		while (node)
		{
			std::cout << "  Node #" << ++nodeIdx << ": "
				<< (node->isUsed() ? "[Used]" : "[Free]") << " "
				<< static_cast<void*>(node->getStart()) << " - "
				<< static_cast<void*>(node->getEnd()) << " ("
				<< (node->getBlockNodeSize()) << " bytes)\n";
			node = node->getNext();
		}
		current = current->getNextBlock();
	}
}
void MemoryPool::PrintMemoryLeaks() const {
	std::cout << "\n************* �ڴ�й¶��� *************\n";

	size_t totalLeaked = 0;
	size_t leakCount = 0;

	// ���������ڴ��
	const MemoryBlock* currentBlock = m_blockStart;
	while (currentBlock != nullptr) 
	{
		// �������ڵ������ڴ�ڵ�
		const BlockNode* currentNode = currentBlock->getStartBlockNode();
		while (currentNode != nullptr)
		{
			if (currentNode->isUsed())
			{
				// ����й¶�ڴ��С
				size_t leakedSize = currentNode->getBlockNodeSize();
				totalLeaked += leakedSize;
				leakCount++;

				// ���й¶�ڴ���ϸ��Ϣ
				std::cout << "й¶��ַ: " << static_cast<void*>(currentNode->getStart())<< ", ��С: " << leakedSize << " bytes\n";
			}
			currentNode = currentNode->getNext();
		}
		currentBlock = currentBlock->getNextBlock();
	}

	if (totalLeaked == 0)
	{
		std::cout << "δ��⵽�ڴ�й¶��\n";
	}
	else
	{
		std::cout << "\n�ܼ�й¶: " << leakCount << " ���ڴ��, "
			<< totalLeaked << " bytes ("
			<< (totalLeaked / 1024.0) << " KB)\n";
	}

	std::cout << "******************************************\n\n";
}