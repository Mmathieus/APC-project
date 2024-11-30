#pragma once
// here you can include whatever you want :)
#include <string>
#include <stdint.h>
#include <vector>
#include <cstdint>
#include <stdexcept>

// if you do not plan to implement bonus, you can delete those lines
// or just keep them as is and do not define the macro to 1
#define SUPPORT_IFSTREAM 0
#define SUPPORT_MORE_OPS 0
#define SUPPORT_EVAL 0 // special bonus

#define MODULO 1'000'000'000
#define DIGITS 9

class BigInteger
{
public:
    // Prazdny konstruktor
    BigInteger() {
        
        vv.push_back(0);
    };
    
    // Ciselny konstruktor
    BigInteger(int64_t n) {
        
        if (n == 0) {
            vv.push_back(0);
            return;
        }

        if (n < 0) {
            negative = true;
            n = -n;
        }

        // Rychle vybavenie cisla pod limitom
        if (n < MODULO) {
            vv.push_back(n);
            return;
        }

        // Ak cislo nad limitom
        while (n != 0) {
            int64_t nn = n % MODULO;
            vv.push_back(nn);
            n /= MODULO;
        }
    };
    
    // Stringovy konstruktor
    explicit BigInteger(const std::string& str) {
        
        // Zachytenie prazdneho stringu
        if (str.empty()) {
            throw std::runtime_error("Empty string!");
        }

        //! Zachytenie medzier v stringu | MOZNO POUZIT std::isspace()!!!
        if (str.front() == ' ' || str.back() == ' ') {
            throw std::runtime_error("White space(s) in string!");
        }

        int64_t position = 0;
        bool positive = true;
        
        // Overenie znaku +/-
        if (str[0] == '+') {
            position = 1;
        }
        else if (str[0] == '-') {
            position = 1;
            positive = false;
        }

        // Ak string ma iba jeden znak (znamienko)
        if (position == str.size()) {
            throw std::runtime_error("No digit(s) in string!");
        }

        // Overenie, ze v stringu su iba cifry
        for (int64_t i = position; i < str.size(); i++) {
            if (!isdigit(str[i])) {
                throw std::runtime_error("Only digit(s) allowed!");
            }
        }

        // Hladanie leading 0
        while ((position < str.size()) && (str[position] == '0')) {
            position++;
        }

        // Vyhodnotenie leading 0
        if (position >= str.size()) {
            vv.push_back(0);
            return;
        }
        std::string clean_string = str.substr(position);

        std::string part;
        // Rozdelenie stringu od konca po castiach do vektora
        for (int64_t i = clean_string.size(); i > 0; i -= DIGITS) {
            if (i <= DIGITS) {
                part = clean_string.substr(0, i);
            }
            else {
                part = clean_string.substr(i - DIGITS, DIGITS);
            }
            vv.push_back(std::stoull(part));
        }

        // Ak je cislo zaporne
        if (!positive) {
            negative = true;
        }
    };
    
    // copy
    BigInteger(const BigInteger& other);
    BigInteger& operator=(const BigInteger& rhs);
    // unary operators
    const BigInteger& operator+() const;
    BigInteger operator-() const;
    // binary arithmetics operators
    BigInteger& operator+=(const BigInteger& rhs);
    BigInteger& operator-=(const BigInteger& rhs);
    BigInteger& operator*=(const BigInteger& rhs);
    BigInteger& operator/=(const BigInteger& rhs);
    BigInteger& operator%=(const BigInteger& rhs);

    double sqrt() const;
#if SUPPORT_MORE_OPS == 1
    BigInteger isqrt() const;
    bool is_prime(size_t k) const; // use rabbin-miller test with k rounds
#endif
private:
    // here you can add private data and members, but do not add stuff to 
    // public interface, also you can declare friends here if you want
    std::vector<uint64_t> vv;
    bool negative = false;
};

inline BigInteger operator+(BigInteger lhs, const BigInteger& rhs);
inline BigInteger operator-(BigInteger lhs, const BigInteger& rhs);
inline BigInteger operator*(BigInteger lhs, const BigInteger& rhs);
inline BigInteger operator/(BigInteger lhs, const BigInteger& rhs);
inline BigInteger operator%(BigInteger lhs, const BigInteger& rhs);

// alternatively you can implement 
// std::strong_ordering operator<=>(const BigInteger& lhs, const BigInteger& rhs);
// idea is, that all comparison should work, it is not important how you do it
inline bool operator==(const BigInteger& lhs, const BigInteger& rhs);
inline bool operator!=(const BigInteger& lhs, const BigInteger& rhs);
inline bool operator<(const BigInteger& lhs, const BigInteger& rhs);
inline bool operator>(const BigInteger& lhs, const BigInteger& rhs);
inline bool operator<=(const BigInteger& lhs, const BigInteger& rhs);
inline bool operator>=(const BigInteger& lhs, const BigInteger& rhs);

inline std::ostream& operator<<(std::ostream& lhs, const BigInteger& rhs);

#if SUPPORT_IFSTREAM == 1
// this should behave exactly the same as reading int with respect to 
// whitespace, consumed characters etc...
inline std::istream& operator>>(std::istream& lhs, BigInteger& rhs); // bonus
#endif


class BigRational
{
public:
    // constructors
    BigRational();
    BigRational(int64_t a, int64_t b);
    BigRational(const std::string& a, const std::string& b);
    // copy
    BigRational(const BigRational& other);
    BigRational& operator=(const BigRational& rhs);
    // unary operators
    const BigRational& operator+() const;
    BigRational operator-() const;
    // binary arithmetics operators
    BigRational& operator+=(const BigRational& rhs);
    BigRational& operator-=(const BigRational& rhs);
    BigRational& operator*=(const BigRational& rhs);
    BigRational& operator/=(const BigRational& rhs);

    double sqrt() const;
#if SUPPORT_MORE_OPS == 1
    BigInteger isqrt() const;
#endif
private:
    // here you can add private data and members, but do not add stuff to 
    // public interface, also you can declare friends here if you want
};

inline BigRational operator+(BigRational lhs, const BigRational& rhs);
inline BigRational operator-(BigRational lhs, const BigRational& rhs);
inline BigRational operator*(BigRational lhs, const BigRational& rhs);
inline BigRational operator/(BigRational lhs, const BigRational& rhs);

// alternatively you can implement 
// std::strong_ordering operator<=>(const BigRational& lhs, const BigRational& rhs);
// idea is, that all comparison should work, it is not important how you do it
inline bool operator==(const BigRational& lhs, const BigRational& rhs);
inline bool operator!=(const BigRational& lhs, const BigRational& rhs);
inline bool operator<(const BigRational& lhs, const BigRational& rhs);
inline bool operator>(const BigRational& lhs, const BigRational& rhs);
inline bool operator<=(const BigRational& lhs, const BigRational& rhs);
inline bool operator>=(const BigRational& lhs, const BigRational& rhs);

inline std::ostream& operator<<(std::ostream& lhs, const BigRational& rhs);

#if SUPPORT_IFSTREAM == 1
// this should behave exactly the same as reading int with respect to 
// whitespace, consumed characters etc...
inline std::istream& operator>>(std::istream& lhs, BigRational& rhs); // bonus
#endif

#if SUPPORT_EVAL == 1
inline BigInteger eval(const std::string&);
#endif