#pragma once
// here you can include whatever you want :)
#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <utility>
#include <limits>
#include <cmath>

// if you do not plan to implement bonus, you can delete those lines
// or just keep them as is and do not define the macro to 1
#define SUPPORT_IFSTREAM 0
#define SUPPORT_MORE_OPS 0
#define SUPPORT_EVAL 0 // special bonus

#define MODULO 1'000'000'000
#define DIGITS 9
#define SHOW false

class BigInteger
{
public:
    // constructors
    BigInteger()
        : numbers{0}
        , negative(false)
        , zero(true)
    {
        if (SHOW) { std::cout << "BigInteger default created\n"; }
    }
    
    BigInteger(int64_t n)
        : numbers()
        , negative(false)
        , zero(false)
    {   
        if (SHOW) {std::cout << "BigInteger from INT created\n";}
        if (n == 0) {
            numbers.push_back(0);
            this->zero = true;
            return;
        }
        // Ulozenie zaporneho znamienka
        if (n < 0) {
            this->negative = true;
            n = -n;
        }
        // Rýchle vybavenie čísla pod limitom
        if (n < MODULO) {
            numbers.push_back(n);
            return;
        }
        // Ak číslo presahuje limit MODULO
        while (n != 0) {
            numbers.push_back(n % MODULO);
            n /= MODULO;
        }
    }
    
    explicit BigInteger(const std::string& str)
        : numbers()
        , negative(false)
        , zero(false)
    {
        // Zachytenie prazdneho stringu
        if (str.empty()) {
            throw std::runtime_error("Empty BigInteger!");
        }

        // Ak sa maju overit iba ' ', tak staci iba: str.front/back() == ' '
        if (std::isspace(str.front()) || std::isspace(str.back())) {
            throw std::runtime_error("White space(s) in BigInteger!");
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
            throw std::runtime_error("No digit(s) in BigInteger!");
        }

        // Overenie, ze v stringu su iba cifry
        for (size_t i = position; i < str.size(); i++) {
            if (!isdigit(str[i])) {
                throw std::runtime_error("Only digit(s) allowed in BigInteger!");
            }
        }

        // Hladanie prveho nenuloveho znaku
        size_t non_zero_position = str.find_first_not_of('0', position);

        // Ak same 0 v stringu
        if (non_zero_position == std::string::npos) {
            this->numbers.push_back(0);
            this->zero = true;
            return;
        }
        std::string clean_string = str.substr(non_zero_position);

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

        // Priradenie znamienka
        this->negative = negative;
    };
    
    // move
    BigInteger(BigInteger&& other) noexcept
        : numbers(std::move(other.numbers))
        , negative(other.negative)
        , zero(other.zero)
    {
        if (SHOW) {std::cout << "BigInteger MOVE\n";}
        other.negative = false;
        other.zero = false;
    }


    // copy
    BigInteger(const BigInteger& other)
        : numbers(other.numbers)
        , negative(other.negative)
        , zero(other.zero)
    {
        if (SHOW) {std::cout << "BigInteger COPY\n";}
    }
    
    BigInteger& operator=(const BigInteger& rhs) {
        // Ide o ten isty objekt
        if (this == &rhs) {
            if (SHOW) {std::cout << "BigInteger COPY NO\n";}
            return *this;
        }
        // Ak sa cisla rovanaju
        if (this->numbers == rhs.numbers) {
            this->negative = rhs.negative;
            if (SHOW) {std::cout << "BigInteger COPY only negative\n";}
            return *this;
        }
        // Cisla sa nerovnaju -> komplet kopirovanie
        this->numbers = rhs.numbers;
        this->negative = rhs.negative;
        this->zero = rhs.zero;
        
        if (SHOW) {std::cout << "BigInteger COPY\n";}
    
        return *this;
    };
    

    // unary operators
    const BigInteger& operator+() const {
        return *this;
    };

    BigInteger operator-() const {
        if (this->zero) {
            return *this;
        }
        // Potreba vytvorenia noveho objektu + zmena znamienka
        BigInteger copied = *this;
        copied.negative = !(this->negative);
        
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

        uint64_t carrying = 0;
        // Pripocitavanie aj 'rhs' pokial je, inak iba pripocitanie 'this' ku carrying
        for (size_t i = 0; i < this->numbers.size(); i++) {
            if (i < rhs.numbers.size()) {
                carrying += rhs.numbers[i];
            }
            // Ak sa uz nepripocitava z 'rhs'
            else {
                // Identifikovanie mozneho predcasneho ukoncenie
                if (carrying == 0) { break; }
            }
            carrying += this->numbers[i];
            this->numbers[i] = carrying % MODULO;
            carrying /= MODULO;
        }

        // Pridanie zvysku do vektora z posledneho scitania
        if (carrying > 0) {
            this->numbers.push_back(carrying);
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
            SetToZero(*this);
            return *this;
        }

        bool this_smaller = (*this < rhs);
        // 2 pripady, kedy sa odcitava vacsie od mensieho; Treba cisla prehodit a zmenit znamienko
        if ((!this->negative && this_smaller) || (this->negative && !this_smaller)) {
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

    BigInteger& operator*=(const BigInteger& rhs) {
        // Ak 0*B
        if (this->zero) {
            return *this;
        }
        // Ak A*0
        if (rhs.zero) {
            SetToZero(*this);
            return *this;
        }
        // Nastavenie spravneho znamienka
        this->negative = !(this->negative == rhs.negative);

        // Ak 1*B
        if (this->numbers.size() == 1 && this->numbers[0] == 1) {
            *this = rhs;
            return *this;
        }
        // Ak A*1
        if (rhs.numbers.size() == 1 && rhs.numbers[0] == 1) {
            return *this;
        }

        // Vytvorenie noveho vektora na ukladanie medzivysledkov
        std::vector<uint64_t> storage(this->numbers.size() + rhs.numbers.size(), 0);

        // Loop, ktory berie kazde cislo z 'this'
        for (size_t i = 0; i < this->numbers.size(); i++) {
            // Nasobenie 0-lou nema ziadny efekt na vysledok
            if (this->numbers[i] == 0) { continue; }
            
            uint64_t carrying = 0;

            // Loop, ktory nasobi kazde 'rhs' s aktualnym 'this'
            for (size_t j = 0; (j < rhs.numbers.size() || carrying > 0); j++) {
                // Aktualna hodnota indexu + co sa prenasa
                uint64_t total = storage[i+j] + carrying;
                
                // Klasicke nasobenie (kazde s kazdym); Na oddelenie, ak na konci nasobeni zostanu prebytocne zvysky, ktore treba spracovat osobitne
                if (j < rhs.numbers.size()) {
                    total +=  (this->numbers[i] * rhs.numbers[j]);
                }

                storage[i+j] = total % MODULO;
                carrying = total / MODULO;
            }
        }

        // Odstranenie zbytocnych 0 z konca vektora
        while (storage.back() == 0) {
            storage.pop_back();
        }

        // Priradenie vysledneho vektora
        this->numbers = std::move(storage);
        
        return *this;
    };

    BigInteger& operator/=(const BigInteger& rhs) {
        // Ak A/0
        if (rhs.zero) {
            throw std::runtime_error("Dividing by 0!");
        }
        // Ak 0/B 
        if (this->zero) {
            return *this;
        }
        
        // // Nastvanenie spravneho znamienka
        // this->negative = !(this->negative == rhs.negative);

        // Ak A/1
        if ((rhs.numbers.size() == 1) && (rhs.numbers[0] == 1)) {
            return *this;
        }
        // Ak A/B, kde A = B
        if (this->numbers == rhs.numbers) {
            this->numbers = {1};
            return *this;
        }
        // Ak A/B, kde A < B
        if (FirstSmaller(*this, rhs)) {
            SetToZero(*this);
            return *this;
        }

        // // Ulozenie znamienka a zmena na 'false' (kladne)
        // bool this_negative = this->negative;
        // this->negative = false;

        // // Presun vysledku a spravne nastavenie 'this' parametrov
        // this->negative = this->zero ? false : this_negative;

        DivisionModuloLogic(*this, rhs, true);

        return *this;
    };

    BigInteger& operator%=(const BigInteger& rhs) {
        // Ak A%0
        if (rhs.zero) {
            throw std::runtime_error("Modulling by 0!");
        }
        // Ak 0%B 
        if (this->zero) {
            return *this;
        }
        // Ak A%1 alebo A%B, kde A = B
        if (((rhs.numbers.size() == 1) && (rhs.numbers[0] == 1)) || EqualVectors(*this, rhs)) {
            SetToZero(*this);
            return *this;
        }
        // Ak A%B, kde A < B; Da sa vyhodnotit aj cez '*this' < 'rhs'...
        if (FirstSmaller(*this, rhs)) {
            return *this;
        }

        DivisionModuloLogic(*this, rhs, false);

        return *this;
    };

    // sqrt
    double sqrt() const {
        // Ak zaporne cislo
        if (this->negative) {
            throw std::runtime_error("No SQRT of negative BigInteger");
        }
        // Ak sqrt(0)
        if (this->zero) {
            return 0;
        }
        // Ak sqrt(1)
        if (this->numbers.size() == 1 && this->numbers[0] == 1) {
            return 1;
        }

        double converted = 0;
        // Overenie overflowu pre kazde cislo a nasledna konverzia na double
        for (auto iterator = this->numbers.rbegin(); iterator != this->numbers.rend(); iterator++) {
            if (((converted * MODULO) + *iterator) > std::numeric_limits<double>::max()) {
                throw std::runtime_error("BigInteger too big!");
            }
            converted = (converted * MODULO) + *iterator;
        }

        return std::sqrt(converted);
    };

#if SUPPORT_MORE_OPS == 1
    BigInteger isqrt() const;
    bool is_prime(size_t k) const; // use rabbin-miller test with k rounds
#endif

private:
    std::vector<uint64_t> numbers;
    bool negative;
    bool zero;

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
    friend std::ostream& operator<<(std::ostream& lhs, const BigInteger& rhs);

    friend inline void SetToZero(BigInteger& biginteger);
    friend inline bool FirstSmaller(const BigInteger& lhs, const BigInteger& rhs);
    friend inline void DivisionModuloLogic(BigInteger& lhs, const BigInteger& rhs, bool divison);
    friend inline bool GetNegative(const BigInteger& biginteger);
    friend inline void SetNegative(BigInteger& biginteger, bool value);
    friend inline bool GetZero(const BigInteger& biginteger);
    friend inline void SetZero(BigInteger& biginteger, bool value);
    friend inline bool EqualVectors(const BigInteger& lhs, const BigInteger& rhs);
};

inline BigInteger operator+(BigInteger lhs, const BigInteger& rhs) { lhs += rhs; return lhs; };
inline BigInteger operator-(BigInteger lhs, const BigInteger& rhs) { lhs -= rhs; return lhs; };
inline BigInteger operator*(BigInteger lhs, const BigInteger& rhs) { lhs *= rhs; return lhs; };
inline BigInteger operator/(BigInteger lhs, const BigInteger& rhs) { lhs /= rhs; return lhs; };
inline BigInteger operator%(BigInteger lhs, const BigInteger& rhs) { lhs %= rhs; return lhs; };

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
inline std::ostream& operator<<(std::ostream& os, const BigInteger& rhs) {
    // Ak cislo je 0
    if (rhs.zero) {
        os << 0;
        return os;
    }
    
    // Ak cislo zaporne, tak vypis znamienka
    if (rhs.negative) {
        os << '-';
    }

    // Posledna, respektive prva cast cisla sa vypise bez prefixovych 0-ul
    os << rhs.numbers.back();
    // Vsetky dalsie casti cisla na vypise aj s prefixovymi 0-mi
    for (int64_t i = rhs.numbers.size() - 2; i >= 0; i--) {
        os << std::setw(9) << std::setfill('0') << rhs.numbers[i];
    }

    return os;
};

inline void SetToZero(BigInteger& biginteger) {
    biginteger.numbers = {0};
    biginteger.negative = false;
    biginteger.zero = true;
}

inline bool FirstSmaller(const BigInteger& lhs, const BigInteger& rhs) {
    // Znamienka sa neberu do uvahy!
    // Rozne velkosti vektorov
    if (lhs.numbers.size() != rhs.numbers.size()) {
        return (lhs.numbers.size() < rhs.numbers.size());
    }

    // Rovnake velkosti vektorov
    for (int64_t i = lhs.numbers.size() - 1; i >= 0; i--) {
        // Ak su cisla rozne
        if (lhs.numbers[i] != rhs.numbers[i]) {
            return (lhs.numbers[i] < rhs.numbers[i]);
        }
    }

    return false;
};

inline void DivisionModuloLogic(BigInteger& lhs, const BigInteger& rhs, bool divison) {
    // Vytvorenie konstanty pre nasobenie, buduceho vysledku
    BigInteger constant(2);
    BigInteger answer;

    // Kedze toto je implementacia delenia/modula, hodnoty treba uchovat do vektora, aby sa k nim mohlo vratit
    // Pridanie kopie 'rhs' do vektora
    std::vector<BigInteger> denom;
    denom.reserve(5);
    denom.push_back(rhs);
    //denom[0].negative = false;
    
    // Priame vytvorenie objektu do vektora
    std::vector<BigInteger> current;
    current.reserve(5);
    current.emplace_back(1);

    int64_t index = 0;
    // Ukladanie hodnot 'denom' a 'current' az do platnosti podmienky
    while (denom[index] <= lhs) {
        denom.push_back(constant * denom[index]);
        current.push_back(constant * current[index]);
        index++;
    }
    index--;

    // Proces pocitania vysledku a zvysku zaroven
    while (index >= 0) {
        if (lhs >= denom[index]) {
            lhs -= denom[index];
            answer += current[index];
        }
        index--;
    }

    // Presun vektora ak ide o /; Pri % je vysledok uz priamo ulozeny v 'this'
    if (divison) {
        lhs.numbers = std::move(answer.numbers);
    }
    // Nastavenie 'zero' pre oba pripady
    lhs.zero = (lhs.numbers.size() == 1 && lhs.numbers[0] == 0);

    return;
}

inline bool GetNegative(const BigInteger& biginteger) {
    return biginteger.negative;
}

inline void SetNegative(BigInteger& biginteger, bool value) {
    biginteger.negative = value;
}

inline bool GetZero(const BigInteger& biginteger) {
    return biginteger.zero;
}

inline void SetZero(BigInteger& biginteger, bool value) {
    biginteger.zero = value;
}

inline bool EqualVectors(const BigInteger& lhs, const BigInteger& rhs) {
    return (lhs.numbers == rhs.numbers);
}

#if SUPPORT_IFSTREAM == 1
// this should behave exactly the same as reading int with respect to 
// whitespace, consumed characters etc...
inline std::istream& operator>>(std::istream& lhs, BigInteger& rhs); // bonus
#endif


class BigRational
{
public:
    // constructors
    BigRational()
        : numerator(0)
        , denominator(1)
        , negative(false)
    {
        if (SHOW) {std::cout << "BigRational DEFAULT created\n";}
    }

    BigRational(int64_t a, int64_t b)
        : numerator(std::abs(a))
        , denominator(std::abs(b))
        , negative(!((a < 0) == (b < 0)))
    {
        if (SHOW) {std::cout << "BigRational 2 INTS created\n";}
        // Ak A / 0
        if (b == 0) {
            throw std::runtime_error("BigRational with 0 as denominator!");
        }
        // Ak 0 / B
        if (a == 0) {
            this->denominator = BigInteger(1);
            return;
        }
        // Ak |A| = |B|
        if (std::abs(a) == std::abs(b)) {
            // Ak cisla nie su v zakladnom tvare
            if (std::abs(a) != 1) {
                this->numerator = BigInteger(1);
                this->denominator = BigInteger(1);
            }
            return;
        }
        // Zjednodusenie cisla
        SimplifyNumber(*this);
    }

    BigRational(const std::string& a, const std::string& b)
        : numerator(a)
        , denominator(b)
        , negative(false)
    {
        if (SHOW) {std::cout << "BigRational 2 STRINGS created\n";}
        // Ak A / 0
        if (GetZero(this->denominator)) {
            throw std::runtime_error("BigRational with 0 as denominator!");
        }
        // Ak 0 / B
        if (GetZero(this->numerator)) {
            this->denominator = BigInteger(1);
            return;
        }

        // Nastavenie spravneho znamienka
        this->negative = !(GetNegative(this->numerator) == GetNegative(this->denominator));

        // Ak A = B -> rovnaju sa vektory
        if (EqualVectors(this->numerator, this->denominator)) {
            this->numerator = BigInteger(1);
            this->denominator = BigInteger(1);
            return;
        }
        // Prenastavenie znamienok na 'false'
        SetNegative(this->numerator, false);
        SetNegative(this->denominator, false);

        // Zjednodusenie cisla
        SimplifyNumber(*this);
    }
    
    // move
    BigRational(BigRational&& other) noexcept
        : numerator(std::move(other.numerator))
        , denominator(std::move(other.denominator))
        , negative(other.negative)
    {
        if (SHOW) {std::cout << "BigRational MOVE\n";}
        other.negative = false;
    }


    // copy
    BigRational(const BigRational& other)
        : numerator(other.numerator)
        , denominator(other.denominator)
        , negative(other.negative)
    {
        if (SHOW) {std::cout << "BigRational COPY\n";}
    }

    BigRational& operator=(const BigRational& rhs) {
        if (this != &rhs) {
            this->numerator = rhs.numerator;
            this->denominator = rhs.denominator;
            this->negative = rhs.negative;
            if (SHOW) {std::cout << "BigRational COPY NO\n";}
            return *this;
        }
        if (SHOW) {std::cout << "BigRational COPY\n";}
        
        return *this;
    }


    // unary operators
    const BigRational& operator+() const {
        return *this;
    };

    BigRational operator-() const {
        // Ak je to 0
        if (GetZero(this->numerator)) {
            return *this;
        }
        // Potreba vytvorenia noveho objektu + zmena znamienka
        BigRational copied = *this;
        copied.negative = !(this->negative);
        
        return copied;
    };


    // binary arithmetics operators
    BigRational& operator+=(const BigRational& rhs) {
        // Ak A+0
        if (GetZero(rhs.numerator)) {
            return *this;
        }
        // Ak 0+B
        if (GetZero(this->numerator)) {
            *this = rhs;
            return *this;
        }
        
        // ABY SA VYKONALO SCITANIE, ZNAMIENKA SA MUSIA ROVNAT
        // Musi sa zavolat odcitanie; Nerovnost znamienok
        if (this->negative != rhs.negative) {
            *this -= -rhs;
            return *this;
        }

        // Ako na ZŠ ... menovatele nasobit az potom -> boli by zbytocne delenia -> narocnejsie ako nasobenia
        this->numerator = ((rhs.denominator * this->numerator) + (this->denominator * rhs.numerator));
        this->denominator *= rhs.denominator;

        SimplifyNumber(*this);
        return *this;
    };
    
    BigRational& operator-=(const BigRational& rhs) {
        // Ak A-0
        if (GetZero(rhs.numerator)) {
            return *this;
        }
        // Ak 0-B; Potreba zmenit znamienko
        if (GetZero(this->numerator)) {
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
        // Ak A=B; Vysledok je 0
        if (*this == rhs) {
            SetToZero(this->numerator);
            this->denominator = BigInteger(1);
            this->negative = false;
            return *this;
        }

        bool this_smaller = (*this < rhs);
        // 2 pripady, kedy sa odcitava vacsie od mensieho; Treba cisla prehodit a zmenit znamienko
        if ((!this->negative && this_smaller) || (this->negative && !this_smaller)) {
            *this = (rhs - *this);
            this->negative = !(this->negative);
            SimplifyNumber(*this); 
            return *this;
        }

        // Ako na ZŠ ... menovatele nasobit az potom -> boli by zbytocne delenia -> narocnejsie ako nasobenia
        this->numerator = ((rhs.denominator * this->numerator) - (this->denominator * rhs.numerator)); 
        this->denominator *= rhs.denominator;

        SimplifyNumber(*this);
        return *this; 
    };
    
    BigRational& operator*=(const BigRational& rhs);
    
    BigRational& operator/=(const BigRational& rhs);

    double sqrt() const;

#if SUPPORT_MORE_OPS == 1
    BigInteger isqrt() const;
#endif

private:
    BigInteger numerator;
    BigInteger denominator;
    bool negative;

    friend inline BigRational operator+(BigRational lhs, const BigRational& rhs);
    friend inline BigRational operator-(BigRational lhs, const BigRational& rhs);
    friend inline BigRational operator*(BigRational lhs, const BigRational& rhs);
    friend inline BigRational operator/(BigRational lhs, const BigRational& rhs);

    friend inline bool operator==(const BigRational& lhs, const BigRational& rhs);
    friend inline bool operator!=(const BigRational& lhs, const BigRational& rhs);
    friend inline bool operator<(const BigRational& lhs, const BigRational& rhs);
    friend inline bool operator>(const BigRational& lhs, const BigRational& rhs);
    friend inline bool operator<=(const BigRational& lhs, const BigRational& rhs);
    friend inline bool operator>=(const BigRational& lhs, const BigRational& rhs);
    friend inline std::ostream& operator<<(std::ostream& lhs, const BigRational& rhs);

    friend inline void SimplifyNumber(BigRational& bigrational);
};

inline BigRational operator+(BigRational lhs, const BigRational& rhs) { lhs += rhs; return lhs; };
inline BigRational operator-(BigRational lhs, const BigRational& rhs) { lhs -= rhs; return lhs; };
inline BigRational operator*(BigRational lhs, const BigRational& rhs) { lhs *= rhs; return lhs; };
inline BigRational operator/(BigRational lhs, const BigRational& rhs) { lhs /= rhs; return lhs; };

inline bool operator==(const BigRational& lhs, const BigRational& rhs) {
    // Ak rozne znamienka, automaticky sa cisla nerovnaju
    if (lhs.negative != rhs.negative) {
        return false;
    }
    // Znamienka rovnake, menovatele rovnake -> citatele sa musia tiez rovnat
    if (lhs.denominator == rhs.denominator) {
        return (lhs.numerator == rhs.numerator);
    }
    // Znamienka rovnake, citatele rovnake -> menovatele sa musia tiez rovnat
    if (lhs.numerator == rhs.numerator) {
        return (lhs.denominator == rhs.denominator);
    }
    // Vynasobenie a porovnanie 'lavej' a 'pravej' strany
    return ((rhs.denominator * lhs.numerator) == (lhs.denominator * rhs.numerator));
};
inline bool operator!=(const BigRational& lhs, const BigRational& rhs) {
    // Spravime negaciu, cize 'lhs' sa musi rovnat 'rhs'; Samotna negacia musi byt znegovana, aby to platilo.
    return !(lhs == rhs);
};
inline bool operator<(const BigRational& lhs, const BigRational& rhs) {
    // Ak znamienka rozne
    if (lhs.negative != rhs.negative) {
        return lhs.negative;
    }
    // Znamienka rovnake, menovatele rovnake
    if (lhs.denominator == rhs.denominator) {
        // Ak zaporne cisla, citatel musi byt vacsi
        if (lhs.negative) {
            return (lhs.numerator > rhs.numerator);
        }
        // Ak kladne cisla, citatel musi byt mensi
        return (lhs.numerator < rhs.numerator);
    }
    // Znamienka rovnake, citatele rovnake
    if (lhs.numerator == rhs.numerator) {
        // Ak zaporne cisla, menovatel musi byt mensi -> vysledok vacsi
        if (lhs.negative) {
            return (lhs.denominator < rhs.denominator);
        }
        // Ak kladne cisla, menovatel musi byt vacsi -> vysledok mensi
        return (lhs.denominator > rhs.denominator);
    }   
    // Rozne cisla, rovnake znamienka (-), lava cast musi byt vacsia ako prava
    if (lhs.negative) {
        return ((rhs.denominator * lhs.numerator) > (lhs.denominator * rhs.numerator));
    }
    // Rozne cisla, rovnake znamienka (+), lava cast musi byt mensia ako prava
    return ((rhs.denominator * lhs.numerator) < (lhs.denominator * rhs.numerator));
};
inline bool operator>(const BigRational& lhs, const BigRational& rhs) {
    // Ak ma byt: 5 > 3, potom 3 < 5
    return (rhs < lhs);
};
inline bool operator<=(const BigRational& lhs, const BigRational& rhs) {
    // Spravime negaciu, cize 'lhs' musi byt vacsie ako 'rhs'; Samotna negacia musi byt znegovana, aby to platilo.
    return !(lhs > rhs);
};
inline bool operator>=(const BigRational& lhs, const BigRational& rhs) {
    // Spravime negaciu, cize 'lhs' musi byt mensie ako 'rhs'; Samotna negacia musi byt znegovana, aby to platilo.
    return !(lhs < rhs);
};
inline std::ostream& operator<<(std::ostream& os, const BigRational& rhs) {
    // Ak je cislo zaporne, tak vypis znamienka
    if (rhs.negative) {
        os << '-';
    }
    // Vypis hlavnej casti
    os << rhs.numerator;
    // Vypis zlomkovej casti, ak to nie je 1
    if (rhs.denominator != BigInteger(1)) {
        os << '/' << rhs.denominator;
    }
    
    return os;
};

inline void SimplifyNumber(BigRational& bigrational) {
    // Ak 'nominator' = 0
    if (GetZero(bigrational.numerator)) {
        bigrational.denominator = BigInteger(1);
        bigrational.negative = false;
        return;
    }

    BigInteger x = bigrational.numerator;
    BigInteger y = bigrational.denominator;
    BigInteger z;

    // Najdenie NSD
    while (true) {
        z = x % y;
        if (GetZero(z)) { break; }
        x = y;
        y = z;
    }
    // Predelenie NSD
    bigrational.numerator /= y;
    bigrational.denominator /= y;
}

#if SUPPORT_IFSTREAM == 1
// this should behave exactly the same as reading int with respect to 
// whitespace, consumed characters etc...
inline std::istream& operator>>(std::istream& lhs, BigRational& rhs); // bonus
#endif

#if SUPPORT_EVAL == 1
inline BigInteger eval(const std::string&);
#endif