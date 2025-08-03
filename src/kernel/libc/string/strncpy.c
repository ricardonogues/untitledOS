#include "../include/string.h"

char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    while (n && (*d++ = *s++))
        n--;
    while (n--)
        *d++ = '\0';
    return dest;
}
