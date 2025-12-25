#pragma once
#include <atomic>
#include <cstddef>

template <typename T, size_t Size>
class LockFreeQueue
{
public:
    LockFreeQueue()
        : writeIndex(0), readIndex(0)
    {
        static_assert(Size > 1, "LockFreeQueue size must be > 1");
    }

    // ======================================================
    // PRODUCER (DSP thread) - wait-free
    // ======================================================
    bool push(const T& item)
    {
        const size_t currentWrite = writeIndex.load(std::memory_order_relaxed);
        const size_t nextWrite = increment(currentWrite);

        // Queue is full
        if (nextWrite == readIndex.load(std::memory_order_acquire))
            return false;

        buffer[currentWrite] = item;

        // Commit write
        writeIndex.store(nextWrite, std::memory_order_release);
        return true;
    }

    // ======================================================
    // CONSUMER (UI thread)
    // ======================================================
    bool pop(T& out)
    {
        const size_t currentRead = readIndex.load(std::memory_order_relaxed);

        // Queue empty?
        if (currentRead == writeIndex.load(std::memory_order_acquire))
            return false;

        out = buffer[currentRead];

        // Commit read
        readIndex.store(increment(currentRead), std::memory_order_release);
        return true;
    }

    // ======================================================
    // Helpers
    // ======================================================
    bool isEmpty() const
    {
        return readIndex.load(std::memory_order_acquire) ==
               writeIndex.load(std::memory_order_acquire);
    }

    bool isFull() const
    {
        const size_t nextWrite = increment(writeIndex.load(std::memory_order_relaxed));
        return nextWrite == readIndex.load(std::memory_order_acquire);
    }

private:
    static constexpr size_t increment(size_t idx)
    {
        return (idx + 1) % Size;
    }

    T buffer[Size];
    std::atomic<size_t> writeIndex;
    std::atomic<size_t> readIndex;
};
