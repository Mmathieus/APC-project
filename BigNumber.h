#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <utility>
#include <limits>
#include <cmath>

#define SUPPORT_IFSTREAM 0
#define SUPPORT_MORE_OPS 1
#define SUPPORT_EVAL 0

#define MODULO 1'000'000'000
#define DIGITS 9


class BigInteger
{
public:
    // constructors
    BigInteger()
        : numbers{0}
        , negative(false)
        , zero(true)
    {}
    
    BigInteger(int64_t n)
        : numbers()
        , negative(false)
        , zero(false)
    {   
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
        other.negative = false;
        other.zero = false;
    }

    BigInteger& operator=(BigInteger&& other) noexcept {
        // Ide o ten isty objekt
        if (this == &other) {
            return *this;
        }
        this->numbers = std::move(other.numbers);
        this->negative = other.negative;
        this->zero = other.zero;

        other.negative = false;
        other.zero = false;
        
        return *this;
    };

    // copy
    BigInteger(const BigInteger& other)
        : numbers(other.numbers)
        , negative(other.negative)
        , zero(other.zero)
    {}
    
    BigInteger& operator=(const BigInteger& rhs) {
        // Ide o ten isty objekt
        if (this == &rhs) {
            return *this;
        }
        this->numbers = rhs.numbers;
        this->negative = rhs.negative;
        this->zero = rhs.zero;
        
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
        // Ak A+0
        if (rhs.zero) {
            return *this;
        }
        // Ak 0+B
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
        // Ak A-0
        if (rhs.zero) {
            return *this;
        }
        // Ak 0-B; Potreba zmenit znamienko
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
        // Ak A=B; Vysledok je 0
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
        // Ak 1*B
        if (IsOne(*this)) {
            bool old_negative = this->negative;
            *this = rhs;
            this->negative = !(old_negative == rhs.negative);
            return *this;
        }
        // Ak A*1
        if (IsOne(rhs)) {
            this->negative = !(this->negative == rhs.negative);
            return *this;
        }

        // Nastavenie spravneho znamienka
        this->negative = !(this->negative == rhs.negative);

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
        if (IsOne(rhs)) {
            return *this;
        }
        // Ak A/B, kde A = B, ich vektory
        if (EqualVectors(*this, rhs)) {
            this->numbers = {1};
            return *this;
        }
        // Ak A/B, kde A < B
        if (*this < rhs) {
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
        // Ak A%1 alebo A%B, kde A = B, ich vektory
        if (IsOne(rhs) || EqualVectors(*this, rhs)) {
            SetToZero(*this);
            return *this;
        }
        // Ak A%B, kde A < B
        if (*this < rhs) {
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
        if (IsOne(*this)) {
            return 1;
        }

        return std::sqrt(ConvertToDouble(*this));
    };

#if SUPPORT_MORE_OPS == 1
    BigInteger isqrt() const {
        // Ak zaporne cislo
        if (this->negative) {
            throw std::runtime_error("No SQRT of negative BigInteger");
        }
        // Ak 0 alebo 1
        if (this->zero || IsOne(*this)) {
            return *this;
        }
 
        BigInteger guess = (*this / 2);
        BigInteger value;

        while (true) {
            // Newton method
            value = ((guess + (*this / guess)) / 2);

            // Overenie dosiahnutia hranice
            if ((guess == value) || ((guess+1) == value) || ((guess-1) == value)) {
                break;
            }
            guess = value;
        }

        // Ak vacsie, potreba odcitat -1
        if ((value * value) > *this)
            value -= 1;

        return value;
    };

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

    friend inline void DivisionModuloLogic(BigInteger& lhs, const BigInteger& rhs, bool divison);
    friend inline bool EqualVectors(const BigInteger& lhs, const BigInteger& rhs);
    friend inline void SetToZero(BigInteger& biginteger);
    friend inline void SetToOne(BigInteger& biginteger);
    friend inline bool IsOne(const BigInteger& biginteger);
    friend inline bool GetNegative(const BigInteger& biginteger);
    friend inline void SetNegative(BigInteger& biginteger, bool value);
    friend inline bool GetZero(const BigInteger& biginteger);
    friend inline double ConvertToDouble(const BigInteger& biginteger);
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
    // Ak cislo je 1
    if (IsOne(rhs)) {
        os << 1;
        return os;
    }

    // Posledna, respektive prva cast cisla sa vypise bez prefixovych 0-ul
    os << rhs.numbers.back();
    // Vsetky dalsie casti cisla na vypise aj s prefixovymi 0-mi
    for (int64_t i = rhs.numbers.size() - 2; i >= 0; i--) {
        os << std::setw(9) << std::setfill('0') << rhs.numbers[i];
    }

    return os;
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

inline bool EqualVectors(const BigInteger& lhs, const BigInteger& rhs) {
    return (lhs.numbers == rhs.numbers);
}

inline void SetToZero(BigInteger& biginteger) {
    biginteger.numbers = {0};
    biginteger.negative = false;
    biginteger.zero = true;
}

inline void SetToOne(BigInteger& biginteger) {
    biginteger.numbers = {1};
    biginteger.negative = false;
    biginteger.zero = false;
}

inline bool IsOne(const BigInteger& biginteger) {
    return ((biginteger.numbers.size() == 1) && (biginteger.numbers[0] == 1));
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

inline double ConvertToDouble(const BigInteger& biginteger) {
    double converted = 0;
    // Overenie overflowu pre kazde cislo a nasledna konverzia na double
    for (auto iterator = biginteger.numbers.rbegin(); iterator != biginteger.numbers.rend(); iterator++) {
        if (((converted * MODULO) + *iterator) > std::numeric_limits<double>::max()) {
            throw std::runtime_error("BigInteger too big!");
        }
        converted = (converted * MODULO) + *iterator;
    }
    return converted;
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
    {}

    BigRational(int64_t a, int64_t b)
        : numerator(a)
        , denominator(b)
        , negative(false)
    {
        // Ak A / 0
        if (b == 0) {
            throw std::runtime_error("BigRational with 0 as denominator!");
        }
        // Ak 0 / B
        if (a == 0) {
            SetToOne(this->denominator);
            return;
        }

        // Nastavenie spravneho znamienka
        this->negative = !(GetNegative(this->numerator) == GetNegative(this->denominator));
        // Prenastavenie znamienok na 'false'
        SetNegative(this->numerator, false);
        SetNegative(this->denominator, false);
        
        // Ak 1 / B alebo A / 1 -> najzakladnejsi tvar cisla
        if (IsOne(this->numerator) || IsOne(this->denominator)) {
            return;
        }
        // Ak A=B, ich vektory
        if (EqualVectors(this->numerator, this->denominator)) {
            SetToOne(this->numerator);
            SetToOne(this->denominator);
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
        // Ak A / 0
        if (GetZero(this->denominator)) {
            throw std::runtime_error("BigRational with 0 as denominator!");
        }
        // Ak 0 / B
        if (GetZero(this->numerator)) {
            SetToOne(this->denominator);
            return;
        }
        
        // Nastavenie spravneho znamienka
        this->negative = !(GetNegative(this->numerator) == GetNegative(this->denominator));
        // Prenastavenie znamienok na 'false'
        SetNegative(this->numerator, false);
        SetNegative(this->denominator, false);

        // Ak 1 / B alebo A / 1 -> najzakladnejsi tvar cisla
        if (IsOne(this->numerator) || IsOne(this->denominator)) {
            return;
        }
        // Ak A=B, ich vektory
        if (EqualVectors(this->numerator, this->denominator)) {
            SetToOne(this->numerator);
            SetToOne(this->denominator);
            return;
        }

        // Zjednodusenie cisla
        SimplifyNumber(*this);
    }
    
    // move
    BigRational(BigRational&& other) noexcept
        : numerator(std::move(other.numerator))
        , denominator(std::move(other.denominator))
        , negative(other.negative)
    {
        other.negative = false;
    }

    BigRational& operator=(BigRational&& other) noexcept {
        // Ide o ten isty objekt
        if (this == &other) {
            return *this;
        }
        this->numerator = std::move(other.numerator);
        this->denominator = std::move(other.denominator);
        this->negative = other.negative;

        other.negative = false;
        
        return *this;
    }

    // copy
    BigRational(const BigRational& other)
        : numerator(other.numerator)
        , denominator(other.denominator)
        , negative(other.negative)
    {}

    BigRational& operator=(const BigRational& rhs) {
        // Ide o ten isty objekt
        if (this == &rhs) {
            return *this;
        }
        this->numerator = rhs.numerator;
        this->denominator = rhs.denominator;
        this->negative = rhs.negative;
        
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
            SetToZeroR(*this);
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
    
    BigRational& operator*=(const BigRational& rhs) {
        // Nastavenie spravneho znamienka
        this->negative = !(this->negative == rhs.negative);
        
        // Ak 0*B
        if (GetZero(this->numerator)) {
            return *this;
        }
        // Ak A*0
        if (GetZero(rhs.numerator)) {
            SetToZeroR(*this);
            return *this;
        }
        // Ak 1*B
        if (IsOneR(*this)) {
            *this = rhs;
            return *this;
        }
        // Ak A*1
        if (IsOneR(rhs)) {
            return *this;
        }

        this->numerator *= rhs.numerator;
        this->denominator *= rhs.denominator;

        SimplifyNumber(*this);
        return *this;
    };
    
    BigRational& operator/=(const BigRational& rhs) {
        // Ak A/0
        if (GetZero(rhs.numerator)) {
            throw std::runtime_error("Dividing by 0!");
        }
        // Ak 0/A
        if (GetZero(this->numerator)) {
            return *this;
        }

        // Nastavenie spravneho znamienka
        this->negative = !(this->negative == rhs.negative);

        // Ak A/1
        if (IsOneR(rhs)) {
            return *this;
        }
        // Ak A/B, kde A=B, vysledok je 1
        if (*this == rhs) {
            SetToOne(this->numerator);
            SetToOne(this->denominator);
            return *this;
        }
        // Ak A/B, kde na kriz sa zlomky A a B rovnaju; Znamienka sa tu neriesia
        if (EqualVectors(this->numerator, rhs.denominator) && EqualVectors(this->denominator, rhs.numerator)) {
            return *this;
        }

        this->numerator *= rhs.denominator;
        this->denominator *= rhs.numerator;

        SimplifyNumber(*this);
        return *this;
    };

    // sqrt
    double sqrt() const {
        // Ak zaporne cislo
        if (this->negative) {
            throw std::runtime_error("No SQRT of negative BigRational");
        }
        // Ak je cislo = 0
        if (GetZero(this->numerator)) {
            return 0;
        }
        // Ak je cislo = 1
        if (IsOneR(*this)) {
            return 1;
        }
        
        return std::sqrt(ConvertToDouble(this->numerator) / ConvertToDouble(this->denominator));
    };

#if SUPPORT_MORE_OPS == 1
    BigInteger isqrt() const {
        // Ak zaporne cislo
        if (this->negative) {
            throw std::runtime_error("No SQRT of negative BigInteger");
        }
        // Ak 0 alebo 1
        if (GetZero(this->numerator) || IsOneR(*this)) {
            return this->numerator;
        }

        // Ak A / 1 -> len A; Ak A / B -> tak A/B
        BigInteger simple = (IsOne(this->denominator)) ? this->numerator : (this->numerator / this->denominator);
        return simple.isqrt();
    };
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
    friend inline void SetToZeroR(BigRational& bigrational);
    friend inline bool IsOneR(const BigRational& bigrational);
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
    if (EqualVectors(lhs.denominator, rhs.denominator)) {
        return EqualVectors(lhs.numerator, rhs.numerator);
    }
    // Znamienka rovnake, citatele rovnake -> menovatele sa musia tiez rovnat
    if (EqualVectors(lhs.numerator, rhs.numerator)) {
        return EqualVectors(lhs.denominator, rhs.denominator);
    }
    // Cisla rozne; Vynasobenie a porovnanie 'lavej' a 'pravej' strany
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
    if (EqualVectors(lhs.denominator, rhs.denominator)) {
        // Ak zaporne cisla, citatel musi byt vacsi
        if (lhs.negative) {
            return (lhs.numerator > rhs.numerator);
        }
        // Ak kladne cisla, citatel musi byt mensi
        return (lhs.numerator < rhs.numerator);
    }
    // Znamienka rovnake, citatele rovnake
    if (EqualVectors(lhs.numerator, rhs.numerator)) {
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
    // Ak cislo = 0
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
    // Delenie 1-kou nema zmysel
    if (IsOne(y)) { return; }
    
    // Predelenie NSD
    bigrational.numerator /= y;
    bigrational.denominator /= y;
}

inline void SetToZeroR(BigRational& bigrational) {
    SetToZero(bigrational.numerator);
    SetToOne(bigrational.denominator);
    bigrational.negative = false;
}

inline bool IsOneR(const BigRational& bigrational) {
    return (IsOne(bigrational.numerator) && IsOne(bigrational.denominator));
}


#if SUPPORT_IFSTREAM == 1
// this should behave exactly the same as reading int with respect to 
// whitespace, consumed characters etc...
inline std::istream& operator>>(std::istream& lhs, BigRational& rhs); // bonus
#endif

#if SUPPORT_EVAL == 1
inline BigInteger eval(const std::string&);
#endif