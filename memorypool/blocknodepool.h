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
    BlockNode* m_nodes;//预分配的连续内存块存储所有节点
    BlockNode* m_freeListHead;// 空闲链表头指针
};