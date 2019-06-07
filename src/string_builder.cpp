//
// Created by Alexey Makarenya on 2019-06-04.
//

#include <cstdlib>
#include "string_builder.h"

TStringBuilder::TStringBuilder(char* buffer, int len)
{
    Buffer = buffer;
    Remaining = len;
    Len = 0;
}

void TStringBuilder::print(const char* src, int len)
{
    for (int i = 0; i<len && Remaining>0; ++i, --Remaining, ++Len) {
        *(Buffer++) = src[i];
    }
}

void TStringBuilder::print(int value)
{
    int tmp = value;
    int cnt = 1;
    char minus = '-';
    if (value < 0)
    {
        value = -value;
        print(&minus, 1);
    }
    while (tmp>=10) {
        tmp /= 10;
        cnt++;
    }
    if (cnt>Remaining) return;
    tmp = value;
    for(int i = cnt - 1; i >= 0; --i) {
        Buffer[i] = '0'+(tmp%10);
        tmp /= 10;
    }
    Buffer += cnt;
    Remaining -= cnt;
    Len += cnt;
}
