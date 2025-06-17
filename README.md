# 🧮 BigNumber

#### A C++ header file providing support for large integers and rational numbers with mathematical operations.

## ✨ Features

### BigInteger Class
- **Constructors**: Default, integer, string, copy, and move constructors
- **Operators**: `+`, `-`, `*`, `/`, `%`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Mathematical**: `sqrt()`, `isqrt()`, `is_prime(size_t k)`

### BigRational Class
- **Constructors**: Default, integer pair, string, copy, and move constructors
- **Operators**: `+`, `-`, `*`, `/`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Mathematical**: `sqrt()`, `isqrt()`

## 🛠️ Requirements
- C++ version used: **C++20**
- Compiler used: **GCC**

## 💻 Usage Examples

### BigInteger

```cpp
#include "BigNumber.h"
#include <iostream>

int main() {

    BigInteger zero;             // 0
    
    BigInteger a(33);
    BigInteger b(-28);

    BigInteger c("1572");
    BigInteger d("-4473");

    BigInteger x = a + c - b;  
    
    std::cout << x << '\n';      // 1633
    std::cout << c - d << '\n';  // 6045
    std::cout << a * -c << '\n'; // 51876

    // and so on ...

    return 0;
}
```

### BigRational

```cpp
#include "BigNumber.h"
#include <iostream>

int main() {

    BigRational zero;            // 0/1 = 0
    
    BigRational e(33, 3);        // 33/3 = 11
    BigRational f(-28, 11);      // -28/11

    BigRational g("1572", "25"); // 1572/25
    BigRational h("-4473", "9"); // -4473/9 = -497

    BigRational y = e + g - f;
    
    std::cout << y << '\n';      // 21017/275
    std::cout << g - h << '\n';  // 13997/25
    std::cout << e * -g << '\n'; // -17292/25

    // and so on ...

    return 0;
}
```

## 💡 Feel free to explore all available operations such as:
- **Comparison**
- **Square root**
- **...**

## 🎓 Project Context
#### This project was created as a semester assignment for the course *Applied Programming in C++ (APC)* at FIIT STU.