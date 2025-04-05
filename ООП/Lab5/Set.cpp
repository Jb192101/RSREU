#include "Set.h"

template<typename T, size_t MaxSize>
Set<T, MaxSize>::Set() : volume(0) {
    elems = std::vector<T>();
}

template<typename T, size_t volume>
Set<T, volume>::Set(const Set& other) : currentSize(other.currentSize) {
    elements = new T[volume];
    for (size_t i = 0; i < currentSize; ++i) {
        elements[i] = other.elements[i];
    }
}

template<typename T, size_t volume>
size_t Set<T, volume>::getLength() {
    return this->volume;
}

template<typename T, size_t volume>
bool Set<T, volume>::isExist(T _elem) {
    for (size_t i = 0; i < volume; ++i) {
        if (elements[i] == _elem) {
            return true;
        }
    }
    return false;
}

template<typename T, size_t volume>
void Set<T, volume>::unite(Set _set) {
}

template<typename T, size_t volume>
Set<T, volume> Set<T, volume>::operator + (const T& _elem) const {
    Set result(*this);
    if (!result.isExist(elem) && result.currentSize < volume) {
        result.elements[result.currentSize++] = _elem;
    }
    return result;
}

template<typename T, size_t volume>
Set<T, volume> Set<T, volume>::operator - (const T& elem) const {
    Set result(*this);
    for (size_t i = 0; i < result.currentSize; ++i) {
        if (result.elements[i] == elem) {
            for (size_t j = i; j < result.currentSize - 1; ++j) {
                result.elements[j] = result.elements[j + 1];
            }
            result.currentSize--;
            break;
        }
    }
    return result;
}

template<typename T, size_t volume>
Set<T, volume> Set<T, volume>::operator * (const Set& _other) {
    Set result;
    for (size_t i = 0; i < currentSize; ++i) {
        if (other.isExist(elements[i])) {
            if (result.currentSize < volume) {
                result.elements[result.currentSize++] = elements[i];
            } else {
                break;
            }
        }
    }
    return result;
}

template<typename T, size_t volume>
bool Set<T, volume>::operator == (const Set& _other) const {
    if (currentSize != other.currentSize) {
        return false;
    }
    for (size_t i = 0; i < currentSize; ++i) {
        if (!other.isExist(elements[i])) {
            return false;
        }
    }
    return true;
}

template<typename T, size_t volume>
bool Set<T, volume>::operator != (const Set& _other) const {
    return !(*this == other);
}