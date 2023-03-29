#pragma once
#include <vector>
#include <utility> 

template<typename T>
class ObjectPool {
public:
    ObjectPool(int size) {
        for (int i = 0; i < size; i++) {
            T* obj = new T();
            m_pool.push_back(obj);
            m_freeList.push_back(obj);
        }
    }

    ~ObjectPool() {
        for (auto obj : m_pool) { 
            delete obj;
        }
    }

    T* acquire() {
        if (m_freeList.empty()) {
            T* obj = new T();
            m_pool.push_back(obj);
            return obj;
        }
        T* obj = m_freeList.back();
        m_freeList.pop_back();
        return obj;
    }

    void release(T* obj) {
        m_freeList.push_back(obj);
    }

private:
    std::vector<T*> m_pool;
    std::vector<T*> m_freeList;
};
