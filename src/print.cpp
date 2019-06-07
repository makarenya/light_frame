//
// Created by Alexey Makarenya on 2019-06-04.
//

#include <cstdlib>
#include <cmath>
#include "print.h"

size_t TPrint::print(const char* message, size_t length)
{
    return write(reinterpret_cast<const uint8_t*>(message), length);
}

size_t TPrint::println(const char* message, size_t length)
{
    size_t n = print(message, length);
    n += println();
    return n;
}

size_t TPrint::print(char c)
{
    return write(c);
}

size_t TPrint::print(unsigned char b, int base)
{
    return print((unsigned long) b, base);
}

size_t TPrint::print(int n, int base)
{
    return print((long) n, base);
}

size_t TPrint::print(unsigned int n, int base)
{
    return print((unsigned long) n, base);
}

size_t TPrint::print(long n, int base)
{
    if (base == 0) {
        return write(n);
    } else if (base == 10) {
        if (n < 0) {
            int t = print('-');
            n = -n;
            return printNumber(n, 10) + t;
        }
        return printNumber(n, 10);
    } else {
        return printNumber(n, base);
    }
}

size_t TPrint::print(unsigned long n, int base)
{
    if (base == 0) {
        return write(n);
    } else {
        return printNumber(n, base);
    }
}

size_t TPrint::print(double n, int digits)
{
    return printFloat(n, digits);
}

size_t TPrint::println() {
    char lf = '\n';
    return print(&lf, 1);
}

size_t TPrint::println(char c)
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t TPrint::println(unsigned char b, int base)
{
    size_t n = print(b, base);
    n += println();
    return n;
}

size_t TPrint::println(int num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t TPrint::println(unsigned int num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t TPrint::println(long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t TPrint::println(unsigned long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t TPrint::println(double num, int digits)
{
    size_t n = print(num, digits);
    n += println();
    return n;
}

size_t TPrint::printNumber(unsigned long n, uint8_t base)
{
    char buf[8*sizeof(long)+1]; // Assumes 8-bit chars plus zero byte.
    char* str = &buf[sizeof(buf)-1];

    // prevent crash if called with base == 1
    if (base<2) {
        base = 10;
    }

    do {
        unsigned long m = n;
        n /= base;
        char c = m-base*n;
        (*str--) = c<10 ? c+'0' : c+'A'-10;
    } while (n);

    return print(str, str-&buf[sizeof(buf)-1]);
}

size_t TPrint::printFloat(double number, uint8_t digits)
{
    size_t n = 0;

    if (std::isnan(number)) {
        return print("nan");
    }
    if (std::isinf(number)) {
        return print("inf");
    }
    if (number>4294967040.0) {
        return print("ovf");  // constant determined empirically
    }
    if (number<-4294967040.0) {
        return print("ovf");  // constant determined empirically
    }

    // Handle negative numbers
    if (number<0.0) {
        n += print('-');
        number = -number;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for (uint8_t i = 0; i<digits; ++i) {
        rounding /= 10.0;
    }

    number += rounding;

    // Extract the integer part of the number and print it
    auto int_part = (unsigned long) number;
    double remainder = number-(double) int_part;
    n += print(int_part);

    // Print the decimal point, but only if there are digits beyond
    if (digits>0) {
        n += print(".");
    }

    // Extract digits from the remainder one at a time
    while (digits-->0) {
        remainder *= 10.0;
        int toPrint = int(remainder);
        n += print(toPrint);
        remainder -= toPrint;
    }

    return n;
}

