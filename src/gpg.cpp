#include "gpg.h"
#include <math.h>
#include <random>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#define print(a) std::cout<<#a<<'='<<(a)<<std::endl;
gpg pgp;


gpg::gpg()
{
    public_key.first=public_key.second=private_ket.first=private_ket.second=0;
    std::string s ="zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";
    for(unsigned int i=0;i<s.size();++i) {
        slovar.insert(std::pair<char,unsigned int>(s[i],i));
    }
}

std::string gpg::int_to_char(unsigned int i)
{
    std::stringstream ss;
    ss<<i;
    std::string s;
    ss>>s;
    return s;
}

uint_fast16_t gpg::string_to_int(std::string s)
{
    std::stringstream ss;
    uint_fast16_t num;
    ss<<s;
    ss>>num;
    return num;
}



bool gpg::is_simple(uint_fast16_t num)
{
    if(num==2) {
        return true;
    }
    else if(num<2||num%2==0) {
        return false;
    }
    uint_fast16_t sq = sqrt(num);
    for(uint_fast16_t i=3;i<=sq;i+=2) {
        if(num%i==0) {
            return false;
        }
    }
    return true;
}

bool gpg::import_key()
{
    std::ifstream fin("import.txt");
    std::string s;
    uint_fast16_t num;
    if(!fin) {
        return false;
    }
    while(!fin.eof()) {
        fin>>s;
        if(s=="Public key:") {
            fin>>num;
            public_key.first = num;
            fin>>num;
            public_key.second = num;
        }
        else if(s=="Private key:") {
            fin>>num;
            private_ket.first=num;
            fin>>num;
            private_ket.second=num;
        }
    }
    fin.close();
    return true;
}

uint_fast32_t gpg::binpow(uint_fast32_t x,uint_fast32_t y,uint_fast32_t m)
{
    if(y==0) {
        return 1;
    }
    else if(y%2==0) {
        uint_fast32_t num = binpow(x,y/2,m)%m;
        return (num*num)%m;
    }
    else {
        return (x*binpow(x,y-1,m)%m)%m;
    }
}

uint_fast32_t gpg::gcd(uint_fast32_t a,uint_fast32_t b,uint_fast32_t &x,uint_fast32_t &y)
{
    if(a==0){
        x=0;
        y=1;
        return b;
    }
    uint_fast32_t x1,y1;
    uint_fast32_t d = gcd(b%a,a,x1,y1);
    x = y1-(b/a)*x1;
    y=x1;
    return d;
}





bool gpg::generate_keys()
{
    uint_fast8_t max = pow(2,bits)-1;
    uint_fast8_t min = pow(2,bits-1);
    uint_fast8_t p,q;
    uint_fast16_t n,fn,e,d;
    while(true) {
        srand(std::time(0));
        while(true) {
            q=rand()%max;
            if(q<min&&q+min>max) {
                continue;
            }
            else if(q<min&&q+min<max) {
                q+=min;
            }
            if(is_simple(q)) {
                break;
            }
        }
        while(true) {
            p=rand()%max;
            if(p<min&&p+min>max) {
                continue;
            }
            else if(p<min&&p+min<max) {
                q+=min;
            }
            if(is_simple(p)&&p!=q) {
                break;
            }
        }
        n=p*q;
        fn = (p-1)*(q-1);
        while(true) {
            e = rand();
            uint_fast32_t x,y;
            if(e<=1||e>=fn) {
                continue;
            }
            if(gcd(e,fn,x,y)==1) {
                break;
            }
        }
        uint_fast32_t x,y;
        int g = gcd(e,fn,x,y);
        if(g!=1) {
            std::cout<<"Error(gcd!=1)\n";
            return false;
        }
        d = (x%fn+fn)%fn;
        uint_fast64_t z= e*d;
        z=z%fn;
        if(z!=1||e>=fn||d>=fn) {
            continue;
        }
        else {
            std::ofstream fout("keys.txt",std::ofstream::ate);
            public_key.second=private_ket.second = n;
            public_key.first = e;
            private_ket.first = d;
            fout << "Public key: "<<public_key.first<<' '<<public_key.second<<std::endl;
            fout<<"Private key: "<<private_ket.first<<' '<<private_ket.second<<std::endl;
            fout.close();
            return true;
        }
    }
}


std::string gpg::encrypt(std::string s)
{
    std::vector<uint_fast32_t> shifr;
    for(unsigned int i=0;i<s.size();++i) {
        uint_fast32_t k = static_cast<uint_fast32_t>(s[i]);
        k = binpow(k,public_key.first,public_key.second)%public_key.second;
        shifr.push_back(k);
    }
    std::ofstream fout("res.txt");
    s="";
    for(auto x : shifr) {
        fout<<x<<' ';
        s+=int_to_char(x)+' ';
    }
    fout.close();
    return s;
}

std::string gpg::decrypt()
{
    std::ifstream fin("message.txt");
    if(!fin) {
        std::cout<<"File not found\n";
        return "";
    }
    std::vector<uint_fast32_t> shifr;
    while(!fin.eof()) {
        uint_fast32_t num;
        fin>>num;
        shifr.push_back(num);
    }
    fin.close();
    std::string s;
    s.resize(shifr.size());
    for(unsigned int i=0;i<shifr.size();++i) {
        uint_fast32_t k = binpow(shifr[i],private_ket.first,private_ket.second);
        s[i] = static_cast<char>(k);
    }
    return s;
}

std::string gpg::decrypt(std::string  v)
{
    std::string s="";
    std::vector<uint_fast32_t> shifr;
    for(unsigned int i=0;i<v.size();++i) {
        if(v[i]!=' ') {
            s+=v[i];
        }
        else if(v[i]==' ') {
            if(s!="") {
                uint_fast32_t num = string_to_int(s);
                shifr.push_back(num);
                s="";
            }
        }
    }
    s="";
    s.resize(shifr.size());
    for(unsigned int i=0;i<shifr.size();++i) {
        uint_fast32_t k = binpow(shifr[i],private_ket.first,private_ket.second);
        s[i] = static_cast<char>(k);
    }
    return s;
}

