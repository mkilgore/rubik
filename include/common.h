#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/* Inspired via the Linux-kernel macro 'container_of' */
#define container_of(ptr, type, member) \
    ((type *) ((char*)(ptr) - offsetof(type, member)))

#define QQ(x) #x
#define Q(x) QQ(x)

#define TP2(x, y) x ## y
#define TP(x, y) TP2(x, y)

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(*(arr)))

#define HEX(n) 0x ## n ## UL
#define BINARY(x) (((x & 0x0000000FUL)? 1: 0) \
        + ((x & 0x000000F0UL)? 2: 0) \
        + ((x & 0x00000F00UL)? 4: 0) \
        + ((x & 0x0000F000UL)? 8: 0) \
        + ((x & 0x000F0000UL)? 16: 0) \
        + ((x & 0x00F00000UL)? 32: 0) \
        + ((x & 0x0F000000UL)? 64: 0) \
        + ((x & 0xF0000000UL)? 128: 0))

#define b8(bin) ((unsigned char)BINARY(HEX(bin)))
#define b16(binhigh, binlow) (((unsigned short)b8(binhigh) << 8) + b8(binlow))
#define b32(binhigh, binmid1, binmid2, binlow) (((unsigned int)b8(binhigh) << 24) + (b8(binmid1) << 16) + (b8(binmid2) << 8) + b8(binlow))

/* This const is supplied by the Makefile
 * We define it here just to silence warnings from autocomplete or similar
 * features */
#ifndef GBEMUC_VERSION
# define GBEMUC_VERSION "0.0.0"
#endif

#endif
