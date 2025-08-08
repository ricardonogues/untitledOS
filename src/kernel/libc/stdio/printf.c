#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/stdarg.h"
#include "../../include/drivers/tty/tty.h"

static void print_char(char c)
{
    terminal_putchar(c);
}

static void print_string(const char *str)
{
    while (*str)
    {
        print_char(*str++);
    }
}

static void print_number(int n)
{
    if (n < 0)
    {
        print_char('-');
        n = -n;
    }
    if (n / 10)
    {
        print_number(n / 10);
    }
    print_char(n % 10 + '0');
}

static void vprintf(const char *format, va_list args)
{
    while (*format)
    {
        if (*format == '%')
        {
            format++;
            switch (*format)
            {
            case 'c':
                print_char(va_arg(args, int));
                break;
            case 's':
                print_string(va_arg(args, char *));
                break;
            case 'd':
                print_number(va_arg(args, int));
                break;
            case 'x':
            {
                unsigned int num = va_arg(args, unsigned int);
                if (num == 0)
                {
                    print_string("0");
                }
                else
                {
                    char buffer[9]; // Enough for 8 hex digits + null terminator
                    int index = 0;
                    while (num > 0)
                    {
                        int digit = num % 16;
                        if (digit < 10)
                        {
                            buffer[index++] = '0' + digit;
                        }
                        else
                        {
                            buffer[index++] = 'a' + (digit - 10);
                        }
                        num /= 16;
                    }
                    // Reverse the buffer and print the result
                    for (int i = index - 1; i >= 0; i--)
                    {
                        print_char(buffer[i]);
                    }
                }
            }
            break;
            case 'u':
            {
                unsigned int num = va_arg(args, unsigned int);
                if (num == 0)
                {
                    print_string("0");
                }
                else
                {
                    char buffer[11]; // Enough for 10 digits + null terminator
                    int index = 0;
                    while (num > 0)
                    {
                        buffer[index++] = '0' + (num % 10);
                        num /= 10;
                    }
                    // Reverse the buffer and print the result
                    for (int i = index - 1; i >= 0; i--)
                    {
                        print_char(buffer[i]);
                    }
                }
            }
            break;
            case 'p':
            {
                void *ptr = va_arg(args, void *);
                print_string("0x");
                unsigned long num = (unsigned long)ptr;
                for (int i = 28; i >= 0; i -= 4)
                {
                    int digit = (num >> i) & 0xF;
                    if (digit < 10)
                    {
                        print_char('0' + digit);
                    }
                    else
                    {
                        print_char('a' + (digit - 10));
                    }
                }
            }
            break;
            case 'z':
            {
                size_t num = va_arg(args, size_t);
                if (num == 0)
                {
                    print_string("0");
                }
                else
                {
                    char buffer[20]; // Enough for 19 digits + null terminator
                    int index = 0;
                    while (num > 0)
                    {
                        buffer[index++] = '0' + (num % 10);
                        num /= 10;
                    }
                    // Reverse the buffer and print the result
                    for (int i = index - 1; i >= 0; i--)
                    {
                        print_char(buffer[i]);
                    }
                }
            }
            break;
            default:
                print_char('%');
                print_char(*format);
                break;
            }
        }
        else
        {
            print_char(*format);
        }
        format++;
    }
}

int printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    return 0; // Return value is not used in this implementation
}
