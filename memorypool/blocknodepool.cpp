#include"blocknodepool.h"
BlockNodePool::BlockNodePool(size_t nodeNum)
{
    // Ԥ���������ڴ��洢���нڵ�
    m_nodes = new BlockNode[nodeNum * sizeof(BlockNode)];

    for (size_t i = 0; i < nodeNum - 1; ++i)
    {
        m_nodes[i].setNext(&m_nodes[i + 1]);
        m_nodes[i + 1].setLast(&m_nodes[i]);
    }
    m_nodes[nodeNum - 1].setNext(nullptr);
    m_freeListHead = &m_nodes[0];
}
BlockNodePool::~BlockNodePool()
{
    delete[] m_nodes;
}
BlockNode* BlockNodePool::AllocateNode(std::byte* start, size_t blockNodeSize)
{
    if (m_freeListHead == nullptr)
    {
        return nullptr; // ���Ѻľ�
    }
    BlockNode* node = m_freeListHead;
    node->setBlockNodeSize(blockNodeSize);
    node->setStart(start);
    node->setEnd(start + blockNodeSize);
    m_freeListHead = m_freeListHead->getNext();
    return node;
}

void BlockNodePool::FreeNode(BlockNode* node)
{
    // ���ýڵ�״̬
    node->setStart(nullptr);
    node->setEnd(nullptr);
    node->setUsed(false);
    node->setLast(nullptr);

    // ���ڵ�����������ͷ��
    node->setNext(m_freeListHead);
    m_freeListHead = node;
}