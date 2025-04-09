struct Vec2 {
    float x;
    float y;
    
    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Vec2& other) const {
        return !(*this == other);
    }
};

// Перегрузка оператора ввода для Vec2
std::istream& operator>>(std::istream& is, Vec2& vec) {
    char dummy;
    return is >> dummy >> vec.x >> dummy >> vec.y >> dummy; // Формат: {x, y}
}

// Перегрузка оператора вывода для Vec2
std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
    os << "{" << vec.x << ", " << vec.y << "}";
    return os;
}

// Шаблонный класс Set
template <typename T, size_t MAX_SIZE>
class Set {
private:
    T* elements;
    size_t length;

public:
    // Конструктор по умолчанию
    Set() : elements(new T[MAX_SIZE]), length(0) {}

    // Конструктор копирования
    Set(const Set& other) : elements(new T[MAX_SIZE]), length(other.length) {
        for (size_t i = 0; i < length; ++i) {
            elements[i] = other.elements[i];
        }
    }

    // Деструктор
    ~Set() {
        delete[] elements;
    }

    // Оператор присваивания
    Set& operator=(const Set& other) {
        if (this != &other) {
            delete[] elements;
            elements = new T[MAX_SIZE];
            length = other.length;
            for (size_t i = 0; i < length; ++i) {
                elements[i] = other.elements[i];
            }
        }
        return *this;
    }

    // Получить текущее количество элементов
    size_t getLength() const {
        return length;
    }

    // Проверить существование элемента
    bool isExist(const T& elem) const {
        for (size_t i = 0; i < length; ++i) {
            if (elements[i] == elem) {
                return true;
            }
        }
        return false;
    }

    // Объединить с другим множеством
    Set unite(const Set& other) const {
        Set result(*this);
        for (size_t i = 0; i < other.length; ++i) {
            if (!result.isExist(other.elements[i]) && result.length < MAX_SIZE) {
                result.elements[result.length++] = other.elements[i];
            }
        }
        return result;
    }

    // Добавить элемент в множество
    Set operator+(const T& elem) const {
        Set result(*this);
        if (!result.isExist(elem)) {
            if (result.length < MAX_SIZE) {
                result.elements[result.length++] = elem;
            } else {
                std::cerr << "Error: Maximum set size reached (" << MAX_SIZE << ")\n";
            }
        }
        return result;
    }

    // Удалить элемент из множества
    Set operator-(const T& elem) const {
        Set result(*this);
        for (size_t i = 0; i < result.length; ++i) {
            if (result.elements[i] == elem) {
                for (size_t j = i; j < result.length - 1; ++j) {
                    result.elements[j] = result.elements[j + 1];
                }
                result.length--;
                break;
            }
        }
        return result;
    }

    // Пересечение двух множеств
    Set operator*(const Set& other) const {
        Set result;
        for (size_t i = 0; i < length; ++i) {
            if (other.isExist(elements[i])) {
                result.elements[result.length++] = elements[i];
            }
        }
        return result;
    }

    // Проверка на равенство множеств
    bool operator==(const Set& other) const {
        if (length != other.length) return false;
        for (size_t i = 0; i < length; ++i) {
            if (!other.isExist(elements[i])) {
                return false;
            }
        }
        return true;
    }

    // Проверка на неравенство множеств
    bool operator!=(const Set& other) const {
        return !(*this == other);
    }

    // Вывод множества в поток
    friend std::ostream& operator<<(std::ostream& os, const Set& set) {
        os << "Set (" << set.length << " elements): [";
        for (size_t i = 0; i < set.length; ++i) {
            os << set.elements[i];
            if (i != set.length - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Set& set) {
        T elem;
        is >> elem;
        if (!set.isExist(elem) && set.length < MAX_SIZE) {
            set.elements[set.length++] = elem;
        }
        return is;
    }
};

// Специализация для char* (C-строк)
template <size_t MAX_SIZE>
class Set<char*, MAX_SIZE> {
private:
    char** elements;
    size_t length;

public:
    Set() : elements(new char*[MAX_SIZE]), length(0) {
        for (size_t i = 0; i < MAX_SIZE; ++i) {
            elements[i] = nullptr;
        }
    }

    Set(const Set& other) : elements(new char*[MAX_SIZE]), length(other.length) {
        for (size_t i = 0; i < length; ++i) {
            elements[i] = new char[strlen(other.elements[i]) + 1];
            strcpy(elements[i], other.elements[i]);
        }
        for (size_t i = length; i < MAX_SIZE; ++i) {
            elements[i] = nullptr;
        }
    }

    ~Set() {
        for (size_t i = 0; i < length; ++i) {
            delete[] elements[i];
        }
        delete[] elements;
    }

    Set& operator=(const Set& other) {
        if (this != &other) {
            for (size_t i = 0; i < length; ++i) {
                delete[] elements[i];
            }
            delete[] elements;
            
            elements = new char*[MAX_SIZE];
            length = other.length;
            for (size_t i = 0; i < length; ++i) {
                elements[i] = new char[strlen(other.elements[i]) + 1];
                strcpy(elements[i], other.elements[i]);
            }
            for (size_t i = length; i < MAX_SIZE; ++i) {
                elements[i] = nullptr;
            }
        }
        return *this;
    }

    size_t getLength() const {
        return length;
    }

    bool isExist(const char* elem) const {
        for (size_t i = 0; i < length; ++i) {
            if (strcmp(elements[i], elem) == 0) {
                return true;
            }
        }
        return false;
    }

    Set unite(const Set& other) const {
        Set result(*this);
        for (size_t i = 0; i < other.length; ++i) {
            if (!result.isExist(other.elements[i]) && result.length < MAX_SIZE) {
                result.elements[result.length] = new char[strlen(other.elements[i]) + 1];
                strcpy(result.elements[result.length], other.elements[i]);
                result.length++;
            }
        }
        return result;
    }

    Set operator+(const char* elem) const {
        Set result(*this);
        if (!result.isExist(elem)) {
            if (result.length < MAX_SIZE) {
                result.elements[result.length] = new char[strlen(elem) + 1];
                strcpy(result.elements[result.length], elem);
                result.length++;
            } else {
                std::cerr << "Error: Maximum set size reached (" << MAX_SIZE << ")\n";
            }
        }
        return result;
    }

    Set operator-(const char* elem) const {
        Set result(*this);
        for (size_t i = 0; i < result.length; ++i) {
            if (strcmp(result.elements[i], elem) == 0) {
                delete[] result.elements[i];
                for (size_t j = i; j < result.length - 1; ++j) {
                    result.elements[j] = result.elements[j + 1];
                }
                result.length--;
                result.elements[result.length] = nullptr;
                break;
            }
        }
        return result;
    }

    Set operator*(const Set& other) const {
        Set result;
        for (size_t i = 0; i < length; ++i) {
            if (other.isExist(elements[i])) {
                result.elements[result.length] = new char[strlen(elements[i]) + 1];
                strcpy(result.elements[result.length], elements[i]);
                result.length++;
            }
        }
        return result;
    }

    bool operator==(const Set& other) const {
        if (length != other.length) return false;
        for (size_t i = 0; i < length; ++i) {
            if (!other.isExist(elements[i])) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Set& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Set& set) {
        os << "Set (" << set.length << " elements): [";
        for (size_t i = 0; i < set.length; ++i) {
            os << "\"" << set.elements[i] << "\"";
            if (i != set.length - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Set& set) {
        char buffer[256];
        is.getline(buffer, sizeof(buffer));
        
        if (!set.isExist(buffer) && set.length < MAX_SIZE) {
            set.elements[set.length] = new char[strlen(buffer) + 1];
            strcpy(set.elements[set.length], buffer);
            set.length++;
        }
        return is;
    }
};