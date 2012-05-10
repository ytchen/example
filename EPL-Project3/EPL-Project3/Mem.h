#ifndef _Mem_h
#define _Mem_h

#include <iostream>
#include <new>
#if USE_GC
#include <gc.h>
inline
void* operator new(size_t s) { return GC_MALLOC(s); }
inline
void operator delete(void* p) { 
}

#endif /* USE_GC */
#endif /* _Mem_h */
