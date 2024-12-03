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
    // constructors
    BigInteger() {
        this->numbers.push_back(0);
        this->zero = true;
    };
    
    BigInteger(int64_t n) {
        if (n == 0) {
            this->numbers.push_back(0);
            this->zero = true;
            return;
        }

        if (n < 0) {
            this->negative = true;
            n = -n;
        }

        // Rychle vybavenie cisla pod limitom
        if (n < MODULO) {
            this->numbers.push_back(n);
            return;
        }

        // Ak cislo nad limitom
        while (n != 0) {
            uint64_t nn = n % MODULO;
            this->numbers.push_back(nn);
            n /= MODULO;
        }
    };
    
    explicit BigInteger(const std::string& str) {
        // Zachytenie prazdneho stringu
        if (str.empty()) {
            throw std::runtime_error("Empty string!");
        }

        //! Zachytenie medzier v stringu | MOZNO POUZIT std::isspace()
        if (str.front() == ' ' || str.back() == ' ') {
            throw std::runtime_error("White space(s) in string!");
        }

        uint64_t position = 0;
        bool negative = false;
        
        // Overenie znaku +/-
        if (str[0] == '+') {
            position = 1;
        }
        else if (str[0] == '-') {
            position = 1;
            negative = true;
        }

        // Ak string ma iba jeden znak (znamienko)
        if (position == str.size()) {
            throw std::runtime_error("No digit(s) in string!");
        }

        // Overenie, ze v stringu su iba cifry
        for (size_t i = position; i < str.size(); i++) {
            if (!isdigit(str[i])) {
                throw std::runtime_error("Only digit(s) totalowed!");
            }
        }

        // Hladanie leading 0
        while ((position < str.size()) && (str[position] == '0')) {
            position++;
        }

        // Vyhodnotenie leading 0
        if (position == str.size()) {
            this->numbers.push_back(0);
            this->zero = true;
            return;
        }
        std::string clean_string = str.substr(position);

        std::string part;
        // Rozdelenie stringu od konca, po castiach, do vektora
        for (int64_t i = clean_string.size(); i > 0; i -= DIGITS) {
            if (i <= DIGITS) {
                part = clean_string.substr(0, i);
            }
            else {
                part = clean_string.substr(i - DIGITS, DIGITS);
            }
            this->numbers.push_back(std::stoull(part));
        }

        // Ak je cislo zaporne
        if (negative) {
            this->negative = true;
        }
    };
    

    // copy
    BigInteger(const BigInteger& other) = default; // BigInteger two = one;
    BigInteger& operator=(const BigInteger& rhs) = default; // three = one;
    

    // unary operators
    const BigInteger& operator+() const { // x = +y
        return *this;
    };

    BigInteger operator-() const { // x = -y
        if (this->zero) {
            return *this;
        }
        
        BigInteger copied(*this);
        if (this->negative) {
            copied.negative = false;
        }
        else {
            copied.negative = true;
        }
        
        return copied;
    };
    
    
    // binary arithmetics operators
    BigInteger& operator+=(const BigInteger& rhs) {
        // Ak 'this' + 0
        if (rhs.zero) {
            return *this;
        }
        // Ak 0 + 'rhs'
        if (this->zero) {
            *this = rhs;
            return *this;
        }

        // ABY SA VYKONALO SCITANIE, ZNAMIENKA SA MUSIA ROVNAT
        // Musi sa zavolat odcitanie; Nerovnost znamienok
        if (this->negative != rhs.negative) {
            *this -= -rhs;
            return *this;
        }

        // Resize vektora na znamu velkost
        this->numbers.resize(std::max(this->numbers.size(), rhs.numbers.size()), 0);

        uint64_t total = 0;
        // Pripocitavanie aj 'rhs' pokial je, inak iba pripocitanie 'this' ku total
        for (size_t i = 0; i < this->numbers.size(); i++) {
            if (i < rhs.numbers.size()) {
                total += rhs.numbers[i];
            }
            // Ak sa uz nepripocitava z 'rhs'
            else {
                // Identifikovanie mozneho predcasneho ukoncenie
                if (total == 0) { break; }
            }
            total += this->numbers[i];
            this->numbers[i] = total % MODULO;
            total /= MODULO;
        }

        // Pridanie zvysku do vektora z posledneho scitania
        if (total > 0) {
            this->numbers.push_back(total);
        }
    
        return *this;
    };

    BigInteger& operator-=(const BigInteger& rhs) {
        // Ak 'this' - 0
        if (rhs.zero) {
            return *this;
        }
        // Ak 0 - 'rhs'; Potreba zmenit znamienko
        if (this->zero) {
            *this = rhs;
            this->negative = !(this->negative);
            return *this;
        }

        // ABY SA VYKONALO ODCITANIE, ZNAMIENKA SA MUSIA ROVNAT
        // Musi sa zavolat scitanie; Nerovnost znamienok
        if (this->negative != rhs.negative) {
            *this += -rhs;
            return *this;
        }

        // Ak sa 'this' a 'rhs' rovnaju; Vysledok je 0
        if (*this == rhs) {
            this->numbers = {0};
            this->negative = false;
            this->zero = true;
            return *this;
        }

        // 2 pripady, kedy sa odcitava vacsie od mensieho; Treba cisla prehodit a zmenit znamienko
        if ((!this->negative && (*this < rhs)) || (this->negative && (*this > rhs))) {
            *this = (rhs - *this);
            this->negative = !(this->negative);
            return *this;
        }

        uint64_t taking = 0;
        // Odcitavanie 'taking' a 'rhs' pokial je
        for (size_t i = 0; i < this->numbers.size(); i++) {
            int64_t difference = this->numbers[i] - taking;
            if (i < rhs.numbers.size()) {
                difference -= rhs.numbers[i];
            }
            // Ak sa uz neodcitava z 'rhs'
            else {
                // Identifikovanie mozneho predcasneho ukoncenie
                if (static_cast<uint64_t>(difference) == this->numbers[i]) {
                    break;
                }
            }
            // Musime odcitat 1 z nasledujuceho + opravit 'difference'
            if (difference < 0) {
                difference += MODULO;
                taking = 1;
            }
            // Nemusime odcitat 1 z nasledujuceho
            else { taking = 0; }
            
            this->numbers[i] = difference;
        }

        // Odstranenie zbytocnych 0 z konca vektora
        while (this->numbers.back() == 0) {
            this->numbers.pop_back();
        }

        return *this;
    };

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
    std::vector<uint64_t> numbers;
    bool negative = false;
    bool zero = false;


    friend inline BigInteger operator+(BigInteger lhs, const BigInteger& rhs);
    friend inline BigInteger operator-(BigInteger lhs, const BigInteger& rhs);
    friend inline BigInteger operator*(BigInteger lhs, const BigInteger& rhs);
    friend inline BigInteger operator/(BigInteger lhs, const BigInteger& rhs);
    friend inline BigInteger operator%(BigInteger lhs, const BigInteger& rhs);

    friend inline bool operator==(const BigInteger& lhs, const BigInteger& rhs);
    friend inline bool operator!=(const BigInteger& lhs, const BigInteger& rhs);
    friend inline bool operator<(const BigInteger& lhs, const BigInteger& rhs);
    friend inline bool operator>(const BigInteger& lhs, const BigInteger& rhs);
    friend inline bool operator<=(const BigInteger& lhs, const BigInteger& rhs);
    friend inline bool operator>=(const BigInteger& lhs, const BigInteger& rhs);
};

inline BigInteger operator+(BigInteger lhs, const BigInteger& rhs) { lhs += rhs; return lhs; };
inline BigInteger operator-(BigInteger lhs, const BigInteger& rhs) { lhs -= rhs; return lhs; };
inline BigInteger operator*(BigInteger lhs, const BigInteger& rhs) { lhs *= rhs; return lhs; };
inline BigInteger operator/(BigInteger lhs, const BigInteger& rhs) { lhs /= rhs; return lhs; };
inline BigInteger operator%(BigInteger lhs, const BigInteger& rhs) { lhs %= rhs; return lhs; };

// alternatively you can implement 
// std::strong_ordering operator<=>(const BigInteger& lhs, const BigInteger& rhs);
// idea is, that total comparison should work, it is not important how you do it
inline bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
    // Znamienka sa musia rovnat; Takisto vsetky cisla vo vektore a ich pocet sa musia rovnat
    return ((lhs.negative == rhs.negative) && (lhs.numbers == rhs.numbers));
};

inline bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
    // Spravime negaciu, cize 'lhs' sa musi rovnat 'rhs'; Samotna negacia musi byt znegovana, aby to platilo.
    return !(lhs == rhs);
};

inline bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
    // Rozne znamienka; Samotna hodnota lhs.negative hovori vysledok
    if (lhs.negative != rhs.negative) {
        return lhs.negative;
    }

    // Rovnake znamienka; Rozne velkosti vektorov
    if (lhs.numbers.size() != rhs.numbers.size()) {
        // Ak cisla ZAPORNE, tak velkost vektora 'lhs' musi byt vacsia
        if (lhs.negative) {
            return (lhs.numbers.size() > rhs.numbers.size());
        }
        // Ak cisla KLADNE, tak velkost vektora 'lhs' musi byt mensia
        else {
            return (lhs.numbers.size() < rhs.numbers.size());
        }
    }

    // Rovnake znamienka; Rovnake velkosti vektorov
    for (int64_t i = lhs.numbers.size() - 1; i >= 0; i--) {
        // Ak su cisla rozne
        if (lhs.numbers[i] != rhs.numbers[i]) {
            // Ak cisla ZAPORNE, tak 'lhs' cislo musi byt vacsie
            if (lhs.negative) {
                return (lhs.numbers[i] > rhs.numbers[i]);
            }
            // Ak cisla KLADNE, tak 'lhs' cislo musi byt mensie
            else {
                return (lhs.numbers[i] < rhs.numbers[i]);
            }
        }
    }

    return false;
};

inline bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
    // Ak 'lhs' ma byt vacsie ako 'rhs', tak potom 'rhs' musi byt mensie ako 'lhs'
    return (rhs < lhs);
};

inline bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
    // Spravime negaciu, cize 'lhs' musi byt vacsie ako 'rhs'; Samotna negacia musi byt znegovana, aby to platilo.
    return !(lhs > rhs);
};

inline bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
    // Spravime negaciu, cize 'lhs' musi byt mensie ako 'rhs'; Samotna negacia musi byt znegovana, aby to platilo.
    return !(lhs < rhs);
};

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
// idea is, that total comparison should work, it is not important how you do it
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