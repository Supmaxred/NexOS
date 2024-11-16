#pragma once
#include <stdint.h>

#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 

static inline uint32_t div_ceil(uint32_t num, uint32_t den) {
    return (num + den - 1) / den;
}

static inline uint32_t align_up(uint32_t address, uint32_t align) {
    return div_ceil(address, align) * align;
}

static inline uint32_t align_down(uint32_t address, uint32_t align) {
    return (address / align) * align;
}

static inline uint32_t clamp(uint32_t value, uint32_t min_val, uint32_t max_val) {
    return max(min(value, max_val), min_val);
}

static inline uint32_t safe_div(uint32_t num, uint32_t den) {
    return den == 0 ? 0 : num / den;
}