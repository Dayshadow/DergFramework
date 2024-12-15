#pragma once
#include <optional>
#include <iostream>
#include <unordered_set>
template <typename T>
struct DynArrayBlock {
    T data;
    bool valid = false;
};


template <typename T, size_t MAX_BLOCK_COUNT = 2048>
// It's kinda just a dynamic array, but with a maximum size
// It exists because it doesn't do the same copying a vector does, marks data as invalidated out of bounds, and should be safer multithreaded.
// For example, it's ok to read past the end of the array, as long as it's lower than the maximum block count. 
// That should be impossible to do, unless an outside reader keeps around a begin iterator for too long. 
// depreciated due to issues
class DynArray {
public:
    DynArray() {
        m_arr = (DynArrayBlock<T>*)calloc(MAX_BLOCK_COUNT, sizeof(DynArrayBlock<T>));
        m_end = m_arr;
    };
    ~DynArray() {
        for (auto elm : m_newAllocedList) delete &m_arr[elm].data;
        free(m_arr);
    }

    inline size_t length() const {
        return m_end - m_arr;
    }

    // could be useful for deciding when to clean
    float densityFactor() {
        return ((float)length() - (float)m_invalidCount) / (float)length();
    }
    // adds one element to the end of the array, comes first if being iterated.
    // not too recommended for complex objects
    bool push(const T& p_elm) {
        if (length() >= MAX_BLOCK_COUNT) return false;
        m_end->data = p_elm;
        m_end->valid = true;
        m_end++;
        return true;
    }
    // adds one element to the end of the array, comes first if being iterated.
    // not too recommended for complex objects
    bool push(T&& p_elm) {
        if (length() >= MAX_BLOCK_COUNT) return false;
        m_end->data = p_elm;
        m_end->valid = true;
        m_end++;
        return true;
    }

    template <typename... Args>
    bool emplace(Args&&... args) {
        if (length() >= MAX_BLOCK_COUNT) return false;
        if (m_newAllocedList.contains(length())) delete &m_end->data;
        new (&m_end->data) T(std::forward<Args>(args)...);
        m_newAllocedList.insert(length());
        m_end->valid = true;
        m_end++;
        return true;
    }

    // takes a single element out and keeps the internal array clean.
    // @returns a boolean stating if an element was removed or not.
    bool remove(size_t p_index) {
        if (p_index >= length()) return false;
        memmove(&m_arr[p_index], &m_arr[p_index] + 1, (length() - p_index - 1) * sizeof(DynArrayBlock<T>));
        (--m_end)->valid = false;
        return true;
    }
    // marks a single element as deleted, treated like it's gone.
    // @returns a boolean stating if an element was invalidated or not.
    bool invalidate(size_t p_index) {
        if (length() == 0 || p_index >= length() || !m_arr[p_index].valid) return false;
        m_arr[p_index].valid = false;
        m_invalidCount++;
        return true;
    }

    // shift memory around to remove gaps
    void clean() {
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
    }

    // for if you don't care about the internal block format, and just want a reference to T (safely)
    std::optional<T*> at(size_t p_index) {
        if (length() == 0 || p_index >= length() || !m_arr[p_index].valid) return std::nullopt;
        return  &m_arr[p_index].data;
    }

    void dump() {
        for (auto itr = m_arr; itr != m_end; itr++) {
            std::cout << "Data: " << itr->data << ", Valid: " << itr->valid << "\n";
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

    size_t m_invalidCount;
    std::unordered_set<size_t> m_newAllocedList; // kinda scuffed, but deals with the pain of the emplace new operator
};



