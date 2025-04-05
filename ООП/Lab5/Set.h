#pragma once

#include <vector>

template<typename T, size_t V>
class Set {
    private:
        size_t volume;
        std::vector<T> elems;
    public:
        Set() : volume(0);
        Set(const Set& other);

        size_t getLength();
        void unite(Set _set);
        bool isExist(T _elem);

        Set operator + (const T& _elem) const;
        Set operator - (const T& _elem) const;
        Set operator * (const Set& _set) const;
        bool operator == (const Set& _set) const;
        bool operator != (const Set& _set) const;

        void printSet() const;
};