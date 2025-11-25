#include <iostream>
#include <stdexcept>

// Реалізація демонструє:
//   • RAII — автоматичне звільнення пам'яті у деструкторі
//   • заборону копіювання та переміщення
//   • оператори *, ->, приведення до bool
//   • метод reset()
//   • ВАЖЛИВО: конструктори БЕЗ explicit

class uniquePtr {
    int* ptr = nullptr;   // сирий вказівник на ресурс

public:
    // Конструктор за замовчуванням: ресурс відсутній
    uniquePtr() = default;

    // Конструктор від сирого вказівника (без explicit)
    uniquePtr(int* p)
        : ptr(p)
    {
    }

    // Конструктор від значення (без explicit)
    uniquePtr(int value)
        : ptr(new int(value))
    {
    }

    // Деструктор — автоматично звільняє ресурс
    ~uniquePtr() {
        delete ptr;
    }

    // Забороняємо копіювання
    uniquePtr(const uniquePtr&) = delete;
    uniquePtr& operator=(const uniquePtr&) = delete;

    // Забороняємо переміщення
    uniquePtr(uniquePtr&&) = delete;
    uniquePtr& operator=(uniquePtr&&) = delete;

    // Перевірка валідності
    bool isValid() const {
        return ptr != nullptr;
    }

    // Приведення до bool
    operator bool() const {
        return ptr != nullptr;
    }

    // Оператор розіменування
    int& operator*() const {
        if (!ptr)
            throw std::runtime_error("Dereferencing null uniquePtr");
        return *ptr;
    }

    // Оператор доступу через ->
    int* operator->() const {
        if (!ptr)
            throw std::runtime_error("Accessing null uniquePtr");
        return ptr;
    }

    // reset — замінює або очищає ресурс
    void reset(int* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }
};

//
// ------------------------------------------------------------
// Демонстрація роботи uniquePtr
// ------------------------------------------------------------
//
int main() {
    try {
        std::cout << "=== Demonstration of uniquePtr ===\n\n";

        // 1. Створення через сирий вказівник
        uniquePtr p1 = new int(10);
        std::cout << "p1 created from new int(10)\n";

        if (p1.isValid())
            std::cout << "p1.isValid() == true\n";

        if (p1)
            std::cout << "p1 is also true in if(p1)\n";

        std::cout << "Initial *p1: " << *p1 << "\n";
        *p1 = 20;
        std::cout << "After *p1 = 20: " << *p1 << "\n\n";

        // 2. Заміна ресурсу через reset
        std::cout << "Calling p1.reset(new int(42))\n";
        p1.reset(new int(42));
        std::cout << "New *p1: " << *p1 << "\n\n";

        // 3. Очищення ресурсу
        std::cout << "Calling p1.reset() (release resource)\n";
        p1.reset();

        std::cout << "After reset(): p1.isValid() == "
            << std::boolalpha << p1.isValid() << "\n";
        std::cout << "After reset(): if(p1) -> "
            << (p1 ? "true" : "false") << "\n\n";

        // 4. Створення з автоматичним виділенням пам'яті
        uniquePtr p2 = 100;   // тепер це допускається (немає explicit)
        std::cout << "p2 created from value 100\n";
        std::cout << "*p2: " << *p2 << "\n\n";

        std::cout << "=== End of demonstration ===\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}
