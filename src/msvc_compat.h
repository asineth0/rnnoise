#ifndef MSVC_COMPAT_H
#define MSVC_COMPAT_H

/* MSVC compatibility definitions */
#ifdef _MSC_VER

/* MSVC doesn't support C99 inline keyword in C mode */
#ifndef __cplusplus
#if _MSC_VER >= 1900  /* Visual Studio 2015 and later */
#define inline __inline
#else
#define inline __forceinline
#endif
#endif

/* MSVC doesn't support restrict keyword */
#define restrict __restrict

/* MSVC doesn't support __attribute__ */
#define __attribute__(x)

/* MSVC uses _inline instead of inline for some contexts */
#ifndef _inline
#define _inline __inline
#endif

/* Handle builtin functions */
#if _MSC_VER < 1900
#define snprintf _snprintf
#endif

/* Math constants */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Handle MSVC-specific intrinsics */
#include <intrin.h>

/* CPUID function for MSVC */
#ifdef _WIN64
#define cpuid(info, x) __cpuidex(info, x, 0)
#else
#define cpuid(info, x) __cpuid(info, x)
#endif

#endif /* _MSC_VER */

#endif /* MSVC_COMPAT_H */