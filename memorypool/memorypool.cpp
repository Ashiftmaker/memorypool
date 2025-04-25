#include<iostream>
#include<cstddef>
#include"blocknodepool.h"
#include"memorypool.h"


MemoryPool::MemoryPool(const size_t blocksize, const size_t pollBlockNodeNum)
	:m_blockSize(blocksize), m_blockStart(nullptr), m_memorySize(MEMORY_POLL_SIZE), m_currentMemorySize(0), m_blockNodePool(pollBlockNodeNum)
{
	if (m_memorySize % m_blockSize != 0 || m_blockSize > m_memorySize)
	{
		throw std::runtime_error("内存池不能平均分给此大小的内存块");
	}
}

MemoryPool::~MemoryPool()
{
	PrintMemoryLeaks();//打印内存泄露情况一定要在销毁内存块之前，不然越界了
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
		//块链表为空，直接创建新快
		if (m_blockStart == nullptr)
		{
			m_blockStart = new MemoryBlock(m_blockSize,m_blockNodePool);
			if (m_blockStart == nullptr)
			{
				throw std::bad_alloc();
			}
		}
		//在已有内存块里分配
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
		//已有内存块里面分配失败，创建新内存块分配
		if (m_currentMemorySize + m_blockSize <= m_memorySize)
		{
			MemoryBlock* newBlock = new MemoryBlock(m_blockSize, m_blockNodePool);
			std::byte* ptr = newBlock->Allocate(allocatedSize);
			if (ptr != nullptr)
			{
				//头插法将新内存块插入链表
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
			throw std::runtime_error("内存池已满");
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
		//头插法将新内存块插入链表
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
		throw std::runtime_error("销毁内存地址为空");
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

// 输出内存池状态
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
	std::cout << "\n************* 内存泄露情况 *************\n";

	size_t totalLeaked = 0;
	size_t leakCount = 0;

	// 遍历所有内存块
	const MemoryBlock* currentBlock = m_blockStart;
	while (currentBlock != nullptr) 
	{
		// 遍历块内的所有内存节点
		const BlockNode* currentNode = currentBlock->getStartBlockNode();
		while (currentNode != nullptr)
		{
			if (currentNode->isUsed())
			{
				// 计算泄露内存大小
				size_t leakedSize = currentNode->getBlockNodeSize();
				totalLeaked += leakedSize;
				leakCount++;

				// 输出泄露内存详细信息
				std::cout << "泄露地址: " << static_cast<void*>(currentNode->getStart())<< ", 大小: " << leakedSize << " bytes\n";
			}
			currentNode = currentNode->getNext();
		}
		currentBlock = currentBlock->getNextBlock();
	}

	if (totalLeaked == 0)
	{
		std::cout << "未检测到内存泄露。\n";
	}
	else
	{
		std::cout << "\n总计泄露: " << leakCount << " 处内存块, "
			<< totalLeaked << " bytes ("
			<< (totalLeaked / 1024.0) << " KB)\n";
	}

	std::cout << "******************************************\n\n";
}