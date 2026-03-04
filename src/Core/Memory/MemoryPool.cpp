#include <Zgine/Core/Memory/MemoryPool.h>
#include <new>

namespace Zgine {

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
    : m_BlockSize(blockSize), m_BlockCount(blockCount), m_Storage(blockSize * blockCount) {
    m_FreeList.reserve(blockCount);
    for (size_t i = 0; i < blockCount; ++i) {
        m_FreeList.push_back(m_Storage.data() + i * blockSize);
    }
}

void* MemoryPool::Allocate() {
    if (m_FreeList.empty()) {
        return ::operator new(m_BlockSize);
    }
    void* ptr = m_FreeList.back();
    m_FreeList.pop_back();
    return ptr;
}

void MemoryPool::Deallocate(void* ptr) {
    if (!ptr) {
        return;
    }
    if (Owns(ptr)) {
        m_FreeList.push_back(ptr);
    } else {
        ::operator delete(ptr);
    }
}

bool MemoryPool::Owns(void* ptr) const {
    const uint8_t* begin = m_Storage.data();
    const uint8_t* end = begin + m_Storage.size();
    const uint8_t* addr = static_cast<const uint8_t*>(ptr);
    return addr >= begin && addr < end;
}

}
