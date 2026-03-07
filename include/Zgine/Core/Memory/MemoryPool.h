#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Zgine {

class MemoryPool {
public:
    MemoryPool(size_t blockSize, size_t blockCount);

    [[nodiscard]] void* Allocate();
    void Deallocate(void* ptr);

    [[nodiscard]] size_t GetBlockSize() const { return m_BlockSize; }
    [[nodiscard]] size_t GetBlockCount() const { return m_BlockCount; }
    [[nodiscard]] size_t GetFreeCount() const { return m_FreeList.size(); }

    [[nodiscard]] bool Owns(void* ptr) const;

private:
    size_t m_BlockSize = 0;
    size_t m_BlockCount = 0;
    std::vector<uint8_t> m_Storage;
    std::vector<void*> m_FreeList;
};

template <typename T>
[[nodiscard]] T* AllocateFromPool(MemoryPool& pool) {
    return static_cast<T*>(pool.Allocate());
}

template <typename T>
void DeallocateFromPool(MemoryPool& pool, T* ptr) {
    pool.Deallocate(static_cast<void*>(ptr));
}

}
