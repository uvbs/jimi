
#ifndef _JIMI_BASIC_STDDEF_H_
#define _JIMI_BASIC_STDDEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/archdef.h"
#include "jimi/basic/platform.h"
#include "jimi/basic/compiler.h"

#if JIMI_IS_WINDOWS
    #include "jimi/basic/targetver.h"
#endif

#include "jimi/config/config.h"
#include "jimi/basic/export.h"

#include "jimi/basic/stdint.h"
#include "jimi/basic/common.h"

// __has_feature() marco only works on clang.
#if !defined(__clang__)
    #ifndef __has_feature
        #define __has_feature(xxx)      (0)
    #endif
#endif

//
// How to deal with noexcept in Visual Studio
// See: http://stackoverflow.com/questions/18387640/how-to-deal-with-noexcept-in-visual-studio
//
// How can I properly detect the available C++11 features among GCC versions?
// See: http://stackoverflow.com/questions/10286447/how-can-i-properly-detect-the-available-c11-features-among-gcc-versions
//
// When should I really use noexcept?
// See: http://stackoverflow.com/questions/10787766/when-should-i-really-use-noexcept
//
// Allowing Move Constructors to Throw (Rev. 1)
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3050.html
//

// Is noexcept supported?
#if (defined(__clang__) && (defined(__has_feature) && __has_feature(cxx_noexcept))) || \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && (__GNUC__ * 10 + __GNUC_MINOR__ >= 46)) || \
    (defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190021114)  // _MSC_FULL_VER >= 180021114?

    // clang support noexcept if defined "cxx_noexcept",
    // GCC support noexcept since version 4.6,
    // MSVC support noexcept since _MSC_FULL_VER >= 190021114.
    #define NOEXCEPT        noexcept

#elif defined(_MSC_VER)

    // Use throw() in another MSVC version.
    #include <yvals.h>
    #ifndef NOEXCEPT
        #define NOEXCEPT    _NOEXCEPT
    #endif

#else  /* Is not noexcept supported. */

    // Don't support noexcept.
    #define NOEXCEPT

#endif  /* Is noexcept supported? */

#ifndef NO_EXCEPT
#define NO_EXCEPT NOEXCEPT
#endif

//
// C++0x/C++11 Support in GCC
// See: https://gcc.gnu.org/projects/cxx0x.html
//

// Is constexpr supported?
#if (defined(__clang__) && (defined(__has_feature) && __has_feature(cxx_constexpr))) || \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && (__GNUC__ * 10 + __GNUC_MINOR__ >= 46)) || \
    (defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190021114)

    // clang support constexpr if defined "cxx_constexpr",
    // GCC support constexpr since version 4.6, (__cpp_constexpr >= 200704)
    // MSVC support constexpr since _MSC_FULL_VER >= 190021114.
    #define CONST_EXPR      constexpr

#else  /* Is not constexpr supported. */

    // Don't supported constexpr.
    #define CONST_EXPR

#endif  /* Is noexcept supported? */

/**
 * define the name of jimi namespaces
 */
#include "jimi/basic/namespace.h"

/**
 * define jimic function declare type and inline defines
 */
#include "jimi/basic/declare.h"

/**
 * for jimi_winapi
 */
#if defined(JIMI_IS_MSVC)
#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

#define JIMI_WINAPI             __CLR_OR_STD_CALL
#else
#define JIMI_WINAPI
#endif  /* JIMI_IS_MSVC */

/* for static_assert() under VisualStudio 2010 */
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#ifndef static_assert
#define static_assert(expr, msg)
#endif
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef JIMI_MIN
#define JIMI_MIN(a, b)          ((a) < (b) ? (a) : (b))
#endif

#ifndef JIMI_MAX
#define JIMI_MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

#if defined(JIMI_ARCH_X64) || defined(JIMI_ARCH_IA64)
#define JIMI_SIZE_T_SIZEOF      8
#else
#define JIMI_SIZE_T_SIZEOF      4
#endif

/**
 * macro for address aligned to n bytes
 */
#define JIMI_ALIGNED_TO(n, alignment)   \
    (((n) + ((alignment) - 1)) & ~(size_t)((alignment) - 1))

#define JIMI_ADDR_ALIGNED_TO(p, alignment)   \
    ((void *)((((size_t)(void *)(p)) + ((alignment) - 1)) & ~(size_t)((alignment) - 1)))

#define JIMI_ADDR_ALIGNED_DOWNTO(p, alignment)   \
    ((void *)(((size_t)(void *)(p)) & ~(size_t)((alignment) - 1)))

#define JIMI_PVOID_INC(p, n)    ((void *)((size_t)(p) + 1))
#define JIMI_PVOID_DEC(p, n)    ((void *)((size_t)(p) - 1))

#define JIMI_PVOID_ADD(p, n)    ((void *)((size_t)(p) + (n)))
#define JIMI_PVOID_SUB(p, n)    ((void *)((size_t)(p) - (n)))

/**
 * macro for round to power of 2
 */
#define jimi_b2(x)              (        (x) | (        (x) >>  1))
#define jimi_b4(x)              ( jimi_b2(x) | ( jimi_b2(x) >>  2))
#define jimi_b8(x)              ( jimi_b4(x) | ( jimi_b4(x) >>  4))
#define jimi_b16(x)             ( jimi_b8(x) | ( jimi_b8(x) >>  8))  
#define jimi_b32(x)             (jimi_b16(x) | (jimi_b16(x) >> 16))
#define jimi_b64(x)             (jimi_b32(x) | (jimi_b32(x) >> 32))

#define jimi_next_power_of_2(x)     (jimi_b32((x) - 1) + 1)
#define jimi_next_power_of_2_64(x)  (jimi_b64((x) - 1) + 1)

#if defined(JIMI_SIZE_T_SIZEOF) && (JIMI_SIZE_T_SIZEOF == 8)
#define JIMI_ROUND_TO_POW2(N)   jimi_next_power_of_2_64(N)
#else
#define JIMI_ROUND_TO_POW2(N)   jimi_next_power_of_2(N)
#endif

namespace jimi {

template <typename value_type>
inline value_type jimi_min(value_type a, value_type b)
{
    return (a < b ? a : b);
}

template <typename value_type>
inline value_type jimi_max(value_type a, value_type b)
{
    return (a > b ? a : b);
}

}  /* namespace jimi */

// For c++ const_cast
#ifndef REMOVE_CONST
#define REMOVE_CONST(ptr, constType, newType) \
    reinterpret_cast<newType>(const_cast<constType>(ptr))
#endif

// Force convert/change a const type?
#ifndef CHANGE_CONST_CAST
#define CHANGE_CONST_CAST(ptr, constType, newConstType) \
    const_cast<const newConstType>(reinterpret_cast<newConstType>(const_cast<constType>(ptr)))
#endif

/**
 * for precompiled macro to string
 */
#define JIMI_STRING_ESCAPE(x)   #x
#define JIMI_TO_STRING(x)       JIMI_STRING_ESCAPE(x)

#define JIMI_MAKE_STRING(x)     { #x, JIMI_STRING_ESCAPE(x) }

typedef struct JIMI_MACRO_T
{
    const char *name;
    const char *value;
} JIMI_MACRO_T;

/* Need size_t and ptrdiff_t */
#include <cstddef>

//#include <iostream>
//#include <string>

//using namespace std;

#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable: 4996)
#endif  /* _MSC_VER */

// warning C4290: 忽略 C++ 异常规范，但指示函数不是 __declspec(nothrow)
#pragma warning(disable : 4290)

/**
 * for tbb's itune support
 */
#define ITT_SYNC_CREATE(obj, type, name)

namespace jimi {

/**
 * @cond INTERNAL
 * @brief Identifiers declared inside namespace internal should never be used directly by client code.
 */
namespace internal {

//! Base class for types that should not be assigned.
class no_assign {
    //! Deny assignment
    void operator = (const no_assign &);
public:
#if __GNUC__
    //! Explicitly define default construction, because otherwise gcc issues gratuitous warning.
    no_assign() {}
#endif /* __GNUC__ */
};

//! Base class for types that should not be copied or assigned.
class no_copy: no_assign {
    //! Deny copy construction
    no_copy(const no_copy &);
public:
    //! Allow default construction
    no_copy() {}
};

} // internal
//! @endcond

}  /* namespace jimi */

#endif  /* _JIMI_BASIC_STDDEF_H_ */
