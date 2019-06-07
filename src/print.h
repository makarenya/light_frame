#pragma once
#include <cstdint>

class TPrint {
public:
    virtual size_t write(uint8_t byte) = 0;
    virtual size_t write(const uint8_t* message, size_t size) = 0;

    size_t print(const char* message, size_t length);
    template<int N>
    size_t print(const char (& message)[N]) { print(message, N - 1); }
    size_t print(char c);
    size_t print(unsigned char b, int base);
    size_t print(int num, int base);
    size_t print(unsigned int num, int base);
    size_t print(long num, int base);
    size_t print(unsigned long num, int base);
    size_t print(double num, int digits);

    size_t println();
    size_t println(const char* message, size_t length);
    template<int N>
    size_t println(const char (&message)[N]) { println(message, N - 1); }
    size_t println(char c);
    size_t println(unsigned char b, int base);
    size_t println(int num, int base);
    size_t println(unsigned int num, int base);
    size_t println(long num, int base);
    size_t println(unsigned long num, int base);
    size_t println(double num, int digits);

private:
    size_t printNumber(unsigned long n, uint8_t base);
    size_t printFloat(double number, uint8_t digits);
};
