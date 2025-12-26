#include <iostream>
#include <stdexcept>
#include <utility>

class uniquePtr {
    int* ptr = nullptr;

public:
    uniquePtr() = default;

    uniquePtr(int* p)
        : ptr(p)
    {
    }

    uniquePtr(int value)
        : ptr(new int(value))
    {
    }

    ~uniquePtr() {
        delete ptr;
    }

    uniquePtr(const uniquePtr&) = delete;
    uniquePtr& operator=(const uniquePtr&) = delete;

    uniquePtr(uniquePtr&& other) noexcept
        : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    uniquePtr& operator=(uniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    bool isValid() const {
        return ptr != nullptr;
    }

    operator bool() const {
        return ptr != nullptr;
    }

    int* get() const {
        return ptr;
    }

    int* release() {
        int* raw = ptr;
        ptr = nullptr;
        return raw;
    }

    void reset(int* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }

    int& operator*() const {
        if (!ptr) {
            throw std::runtime_error("Dereferencing null uniquePtr");
        }
        return *ptr;
    }

    int* operator->() const {
        if (!ptr) {
            throw std::runtime_error("Accessing null uniquePtr");
        }
        return ptr;
    }
};

class SharedPtrInt {
    int* ptr = nullptr;
    size_t* count = nullptr;

public:
    SharedPtrInt() = default;

    SharedPtrInt(int* p)
        : ptr(p)
    {
        if (p) {
            count = new size_t(1);
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

    SharedPtrInt(SharedPtrInt&& other) noexcept
        : ptr(other.ptr), count(other.count)
    {
        other.ptr = nullptr;
        other.count = nullptr;
    }

    SharedPtrInt& operator=(SharedPtrInt&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            count = other.count;
            other.ptr = nullptr;
            other.count = nullptr;
        }
        return *this;
    }

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
    std::cout << "=== HW5: Move semantics demo ===\n\n";

    std::cout << "[uniquePtr] Move constructor demo\n";
    uniquePtr up1(new int(10));
    std::cout << "up1 valid: " << std::boolalpha << static_cast<bool>(up1) << ", *up1 = " << *up1 << "\n";

    uniquePtr up2(std::move(up1));
    std::cout << "After move: up1 valid: " << static_cast<bool>(up1) << "\n";
    std::cout << "After move: up2 valid: " << static_cast<bool>(up2) << ", *up2 = " << *up2 << "\n\n";

    std::cout << "[uniquePtr] Move assignment demo\n";
    uniquePtr up3(new int(33));
    std::cout << "up3 valid: " << static_cast<bool>(up3) << ", *up3 = " << *up3 << "\n";
    up3 = std::move(up2);
    std::cout << "After move assign: up2 valid: " << static_cast<bool>(up2) << "\n";
    std::cout << "After move assign: up3 valid: " << static_cast<bool>(up3) << ", *up3 = " << *up3 << "\n\n";

    std::cout << "[SharedPtrInt] Move constructor demo\n";
    SharedPtrInt sp1(new int(100));
    std::cout << "sp1 valid: " << static_cast<bool>(sp1) << ", *sp1 = " << *sp1 << "\n";

    SharedPtrInt sp2(std::move(sp1));
    std::cout << "After move: sp1 valid: " << static_cast<bool>(sp1) << "\n";
    std::cout << "After move: sp2 valid: " << static_cast<bool>(sp2) << ", *sp2 = " << *sp2 << "\n\n";

    std::cout << "[SharedPtrInt] Move assignment demo\n";
    SharedPtrInt sp3(new int(777));
    std::cout << "sp3 valid: " << static_cast<bool>(sp3) << ", *sp3 = " << *sp3 << "\n";

    sp3 = std::move(sp2);
    std::cout << "After move assign: sp2 valid: " << static_cast<bool>(sp2) << "\n";
    std::cout << "After move assign: sp3 valid: " << static_cast<bool>(sp3) << ", *sp3 = " << *sp3 << "\n\n";

    std::cout << "=== End ===\n";
    return 0;
}
