#ifndef __POOL__
#define __POOL__

#include <cstdint>

template<uint32_t itemSize = 4>
class Pool
{
private:
    uint32_t    blockSize;

    struct _FreeNode 
    {
        union
        {
            _FreeNode   *prePtr;
            uint8_t     data[itemSize];
        };
        explicit _FreeNode():prePtr(nullptr)
        {

        }
    };

    struct _MemBlock
    {
        _MemBlock   *prePtr;
        _FreeNode   *data;

        explicit _MemBlock(_MemBlock *pre):prePtr(pre), data(nullptr)
        {

        }
    };

    _MemBlock   *blockHeader;
    _FreeNode   *freeNodeHeader;

public:
    Pool(uint32_t bsize = 32) : blockSize(bsize),
        blockHeader(nullptr), 
        freeNodeHeader(nullptr)
    {

    }

    ~Pool()
    {
        for(_MemBlock *it = blockHeader; it != nullptr; it = it->prePtr)
        {
            delete []it->data;
            it->data = nullptr;
        }
        for(_MemBlock *it = blockHeader, *pre = it; it != nullptr; it = pre)
        {
            pre = it->prePtr;
            delete it;
        }
    }

    void* Malloc()
    {
        if(freeNodeHeader == nullptr)
        {
            _MemBlock *block = new _MemBlock(blockHeader);
            block->data = new _FreeNode[blockSize];
            for(int i = 0; i < blockSize; ++i)
            {
                block->data[i].prePtr = freeNodeHeader;
                freeNodeHeader = &block->data[i];
            }
            blockHeader = block;
            blockSize <<= 2;
        }
        void *ret = freeNodeHeader;
        freeNodeHeader = freeNodeHeader->prePtr;
        return ret;
    }

    void Free(void *ptr)
    {
        _FreeNode *temp = reinterpret_cast<_FreeNode*>(ptr);
        temp->prePtr = freeNodeHeader;
        freeNodeHeader = temp;
    }
};

#endif