#ifndef GPG_H
#define GPG_H
#include <utility>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

class gpg
{
    public:
        gpg();
        bool generate_keys();
        std::string encrypt(std::string s);
        std::string decrypt();
        std::pair<uint_fast16_t,uint_fast16_t> get_public_key() {return public_key;}
        std::pair<uint_fast16_t,uint_fast16_t> get_private_key() {return private_ket;}
        void set_public_key(std::pair<uint_fast16_t,uint_fast16_t> k) {public_key=k;}
        void set_private_key(std::pair<uint_fast16_t,uint_fast16_t> k) {private_ket = k;}
        bool import_key();
        std::string decrypt(std::string v);
    private:
        const int bits=8;
        std::pair<uint_fast16_t,uint_fast16_t> private_ket,public_key;
        bool is_simple(uint_fast16_t num);
        uint_fast32_t binpow(uint_fast32_t x,uint_fast32_t y,uint_fast32_t m);
        uint_fast32_t gcd(uint_fast32_t a,uint_fast32_t b,uint_fast32_t &x,uint_fast32_t &y);
        uint_fast64_t mod(uint_fast64_t a,uint_fast64_t b,uint_fast64_t m);
        std::map<char,unsigned int> slovar;
        uint_fast16_t string_to_int(std::string s);
        std::string int_to_char(unsigned int i);
};

extern gpg pgp;

#endif // GPG_H
