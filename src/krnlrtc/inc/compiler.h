#pragma once

#define C_ALIGN(s) __attribute__((aligned(s)))

#define C_FORCEINLINE __attribute__((always_inline)) inline

#define C_DEPRECATED __attribute__((deprecated))

#define C_UNUSED __attribute__((unused))

#define C_PACKED __attribute__((packed))

#define C_NORETURN __attribute__((noreturn))

#define C_OPTIMIZEO3 __attribute__((optimize("O3")))

#define C_SECTION(name) __attribute__((section(name)))