#pragma once
#include <optional>
#include <iostream>
#include <shared_mutex>

#include "util/DynArray.hpp"

template <typename T, size_t MAX_BLOCK_COUNT = 2048>
// same as the other one but threadsafe and a singleton
// depreciated due to issues
// long name lol
class SharedDynArray {
public:
    SharedDynArray() {
        m_arr = (DynArrayBlock<T>*)calloc(MAX_BLOCK_COUNT, sizeof(DynArrayBlock<T>));
        m_end = m_arr;
        m_invalidCount = 0;
    };
    ~SharedDynArray() {
        //for (auto elm : m_newAllocedList) m_arr[elm].data.~T();
        for (size_t i = 0; i < MAX_BLOCK_COUNT; i++) { m_arr->data.~T(); }
        free(m_arr);
    }

    static SharedDynArray<T, MAX_BLOCK_COUNT>& Get() {
        static SharedDynArray<T, MAX_BLOCK_COUNT> s_instance;
        return s_instance;
    }
    inline size_t length() {
        return m_end - m_arr;
    }

    // could be useful for deciding when to clean
    float densityFactor() {
        return ((float)length() - (float)m_invalidCount) / (float)length();
    }

    // adds one element to the end of the array, comes first if being iterated.
    // not too recommended for complex objects
    bool push(const T& p_elm) {
        std::unique_lock<std::shared_mutex> lock(m_mtx);
        if (length() >= MAX_BLOCK_COUNT) return false;
        m_end->data = p_elm;
        m_end->valid = true;
        m_end++;
        return true;
    }
    // adds one element to the end of the array, comes first if being iterated.
    // not too recommended for complex objects
    bool push(T&& p_elm) {
        std::unique_lock<std::shared_mutex> lock(m_mtx);
        if (length() >= MAX_BLOCK_COUNT) return false;
        m_end->data = p_elm;
        m_end->valid = true;
        m_end++;
        return true;
    }

    template <typename... Args>
    bool emplace(Args&&... args) {
        std::unique_lock<std::shared_mutex> lock(m_mtx);
        if (length() >= MAX_BLOCK_COUNT) return false;
        if (m_newAllocedList.contains(length() + 1)) m_end->data.~T();
        new (&m_end->data) T(std::forward<Args>(args)...);
        m_newAllocedList.insert(length() + 1);
        m_end->valid = true;
        m_end++;
        return true;
    }
    // takes a single element out and keeps the internal array clean.
    // @returns a boolean stating if an element was removed or not.
    bool remove(size_t p_index) {
        std::unique_lock<std::shared_mutex> lock(m_mtx);
        if (p_index >= length()) return false;
        memmove(&m_arr[p_index], &m_arr[p_index] + 1, (length() - p_index - 1) * sizeof(DynArrayBlock<T>));
        (--m_end)->valid = false;
        m_end->data.~T();
        return true;
    }
    // marks a single element as deleted, treated like it's gone.
    // @returns a boolean stating if an element was invalidated or not.
    bool invalidate(size_t p_index) {
        std::unique_lock<std::shared_mutex> lock(m_mtx);
        if (length() == 0 || p_index >= length() || !m_arr[p_index].valid) return false;
        m_arr[p_index].valid = false;
        m_arr[p_index].~T();
        m_invalidCount++;
        return true;
    }

    // shift memory around to remove gaps
    void clean() {
        std::unique_lock<std::shared_mutex> lock(m_mtx);
        if (length() == 0) return;
        // keeps track of the last time it went from valid to invalid; 
        DynArrayBlock<T>* lastFallingEdge = m_arr;
        bool prevValid = true;
        for (size_t i = 0; i < length(); i++) {
            if (prevValid && !m_arr[i].valid) { // falling edge
                lastFallingEdge = &m_arr[i];
            }
            else if (!prevValid && m_arr[i].valid) { // rising edge
                size_t removedCount = (&m_arr[i] - lastFallingEdge);
                memmove(lastFallingEdge, &m_arr[i], (m_end - &m_arr[i]) * sizeof(DynArrayBlock<T>));
                memset(m_end - removedCount, 0, removedCount * sizeof(DynArrayBlock<T>));
                m_end -= removedCount;
                i -= removedCount;
                m_invalidCount -= removedCount;
            }
            prevValid = m_arr[i].valid;
        }
        if (!m_arr[length() - 1].valid) {
            m_invalidCount -= m_end - lastFallingEdge;
            m_end = lastFallingEdge;
        };
        memset(m_end, 0, (MAX_BLOCK_COUNT - length()) * sizeof(DynArrayBlock<T>));
    }

    // for if you don't care about the internal block format, and just want a reference to T (safely)
    std::optional<T*> at(size_t p_index) {
        std::shared_lock<std::shared_mutex> lock(m_mtx);
        if (length() == 0 || p_index >= length() || !m_arr[p_index].valid) return std::nullopt;
        return &m_arr[p_index].data;
    }

    void dump() const {
        for (auto itr = m_arr; itr != m_end; itr++) {
            std::cout << "Data: " << itr->data.m_eptr << ", Valid: " << itr->valid << "\n";
        }
    }

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = DynArrayBlock<T>*;
        using reference = DynArrayBlock<T>&;

        iterator(DynArrayBlock<T>* ptr, DynArrayBlock<T>* start) : m_ptr(ptr), m_start(start) {};

        T& operator*() const { return (*m_ptr).data; };

        // yes, they are "increment" operators, but this is just how the list is used
        iterator& operator++() {
            while (--m_ptr != m_start && !m_ptr->valid) {};
            return *this;
        }
        iterator operator++(int) {
            iterator temp = *this;
            while (--m_ptr != m_start && !m_ptr->valid) {};
            return temp;
        }

        bool operator!=(const iterator& other) const { return m_ptr != other.m_ptr; }

        ptrdiff_t operator-(const iterator& other) const { return m_ptr - other.m_ptr; }

    private:
        DynArrayBlock<T>* m_ptr;
        DynArrayBlock<T>* m_start;
    };

    iterator begin() { // yeah yeah the start is the end what about it
        DynArrayBlock<T>* validStart = m_end;
        while (!(--validStart)->valid) {};
        return iterator(validStart, m_arr - 1);
    }
    iterator end() {
        return iterator(m_arr - 1, m_arr - 1);
    }
private:
    DynArrayBlock<T>* m_arr;
    // keep track of the official end of the array to make it faux-dynamic
    DynArrayBlock<T>* m_end;
    std::shared_mutex m_mtx;

    size_t m_invalidCount;
    std::unordered_set<size_t> m_newAllocedList; // kinda scuffed, but deals with the pain of the emplace new operator
};



