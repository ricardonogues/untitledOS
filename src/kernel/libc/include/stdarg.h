#ifndef _STDARG_H
#define _STDARG_H

// For x86 (32-bit) architecture
typedef char *va_list;

// Size alignment macro - rounds up to nearest 4-byte boundary
#define _VA_SIZE(type) \
    (((sizeof(type) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

// Initialize va_list to point to first variadic argument
#define va_start(ap, last) \
    ((ap) = (va_list) & (last) + _VA_SIZE(last))

// Get next argument and advance pointer
#define va_arg(ap, type) \
    (*(type *)((ap) += _VA_SIZE(type), (ap) - _VA_SIZE(type)))

// Clean up (no-op on x86, but required by standard)
#define va_end(ap) \
    ((ap) = (va_list)0)

// Copy va_list (useful for vprintf implementations)
#define va_copy(dest, src) \
    ((dest) = (src))

#endif /* _STDARG_H */