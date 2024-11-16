#ifndef _CTYPE_H_
#define _CTYPE_H_

static inline int isalnum(int c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static inline int isalpha(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static inline int iscntrl(int c) {
    return (c >= 0 && c <= 0x1F) || (c == 0x7F);
}

static inline int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

static inline int isgraph(int c) {
    return (c > 0x20 && c < 0x7F);
}

static inline int islower(int c) {
    return (c >= 'a' && c <= 'z');
}

static inline int isprint(int c) {
    return (c >= 0x20 && c < 0x7F);
}

static inline int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

static inline int isspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

static inline int isupper(int c) {
    return (c >= 'A' && c <= 'Z');
}

static inline int isxdigit(int c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static inline int tolower(int c) {
    if (isupper(c)) {
        return c + ('a' - 'A');
    }
    return c;
}

static inline int toupper(int c) {
    if (islower(c))
        return c - ('a' - 'A');
    return c;
}

#endif // _CTYPE_H_
