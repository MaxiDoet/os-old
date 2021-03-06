#ifndef _cpuid_h
#define _cpuid_h
#endif

/* Códes
	0 = Manufacturer
	1 = Model
*/

static inline void cpuid(int code, uint32_t* a, uint32_t* d)
{
    asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}
