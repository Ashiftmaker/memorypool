#pragma once
# include"blocknode.h"

class BlockNodePool
{
public:
    BlockNodePool(size_t nodeNum);
     ~BlockNodePool();
     BlockNode* AllocateNode(std::byte* start, size_t blockNodeSize);
    void FreeNode(BlockNode* node);
private:
    BlockNode* m_nodes;//Ԥ����������ڴ��洢���нڵ�
    BlockNode* m_freeListHead;// ��������ͷָ��
};