#ifndef _UTIL_
#define _UTIL_

#include <stdint.h>

#define ASSERT(__ex) if (!(__ex)) { printf ("Assertion Failed:\n" #__ex "\n"); *(volatile int *) 0 = 0; }
#define LEN(__arr) (sizeof ((__arr)) / sizeof ((__arr)[0]))

#define MIN(__A, __B) ((__A) < (__B) ? (__A) : (__B))
#define MAX(__A, __B) ((__A) > (__B) ? (__A) : (__B))
#define CLAMP(__V, __MIN, __MAX) (MAX ((__MIN), MIN ((__MAX), (__V))))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#endif
