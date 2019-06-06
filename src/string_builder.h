#pragma once

class TStringBuilder {
public:
    TStringBuilder(char* buffer, int len);
    template<int N>
    void print(const char (& src)[N])
    {
        print(src, sizeof(src) - 1);
    }
    void print(const char *src, int len);
    void print(int value);
    int length() { return Len; }

private:
    char* Buffer{};
    int Remaining{};
    int Len{};
};
