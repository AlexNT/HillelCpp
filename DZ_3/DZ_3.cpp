#include <iostream>
#include <stdexcept>

class SharedPtrInt {
private:
    int* ptr;
    size_t* count;

public:
    SharedPtrInt()
        : ptr(nullptr), count(nullptr)
    {
    }

    SharedPtrInt(int* p)
        : ptr(p)
    {
        if (p) {
            count = new size_t(1);
        }
        else {
            count = nullptr;
        }
    }

    SharedPtrInt(int value)
        : ptr(new int(value)), count(new size_t(1))
    {
    }

    ~SharedPtrInt() {
        release();
    }

    SharedPtrInt(const SharedPtrInt& other)
        : ptr(other.ptr), count(other.count)
    {
        if (count) {
            ++(*count);
        }
    }

    SharedPtrInt& operator=(const SharedPtrInt& other) {
        if (this == &other) {
            return *this;
        }

        release();

        ptr = other.ptr;
        count = other.count;
        if (count) {
            ++(*count);
        }

        return *this;
    }

    SharedPtrInt(SharedPtrInt&&) = delete;
    SharedPtrInt& operator=(SharedPtrInt&&) = delete;

    void release() {
        if (count) {
            --(*count);
            if (*count == 0) {
                delete ptr;
                delete count;
            }
        }
        ptr = nullptr;
        count = nullptr;
    }

    int* get() const {
        return ptr;
    }

    bool isValid() const {
        return ptr != nullptr;
    }

    operator bool() const {
        return ptr != nullptr;
    }

    int& operator*() const {
        if (!ptr) {
            throw std::runtime_error("Dereferencing null SharedPtrInt");
        }
        return *ptr;
    }

    int* operator->() const {
        if (!ptr) {
            throw std::runtime_error("Accessing null SharedPtrInt");
        }
        return ptr;
    }
};

int main() {
    try {
        std::cout << "=== Demonstration of SharedPtrInt ===\n\n";

        SharedPtrInt p1(new int(10));
        std::cout << "p1 created from new int(10)\n";

        if (p1.isValid()) {
            std::cout << "p1.isValid() == true\n";
        }

        if (p1) {
            std::cout << "p1 is also true in if(p1)\n";
        }

        std::cout << "Initial *p1: " << *p1 << "\n";
        *p1 = 20;
        std::cout << "After *p1 = 20: " << *p1 << "\n\n";

        std::cout << "Creating p2 as a copy of p1\n";
        SharedPtrInt p2(p1);
        std::cout << "*p2 (shared with p1): " << *p2 << "\n";

        *p2 = 30;
        std::cout << "After *p2 = 30:\n";
        std::cout << "  *p1: " << *p1 << "\n";
        std::cout << "  *p2: " << *p2 << "\n\n";

        std::cout << "Creating p3 and assigning p3 = p1\n";
        SharedPtrInt p3;
        p3 = p1;
        std::cout << "*p3 (also shared with p1): " << *p3 << "\n\n";

        std::cout << "p1.get() raw pointer address: " << p1.get() << "\n\n";

        {
            std::cout << "Entering inner scope\n";
            SharedPtrInt p4(p1);
            std::cout << "*p4 (shared with p1): " << *p4 << "\n";
            std::cout << "Leaving inner scope (p4 destroyed)\n\n";
        }

        std::cout << "After inner scope:\n";
        if (p1) std::cout << "  p1 still valid, *p1 = " << *p1 << "\n";
        if (p2) std::cout << "  p2 still valid, *p2 = " << *p2 << "\n";
        if (p3) std::cout << "  p3 still valid, *p3 = " << *p3 << "\n\n";

        std::cout << "=== End of demonstration ===\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}
