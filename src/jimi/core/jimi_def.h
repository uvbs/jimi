
#ifndef _JIMI_DEF_H_
#define _JIMI_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_platform_def.h>
#include <jimi/core/jimi_compiler_def.h>

#if JIMI_IS_WINDOWS
    #include <jimi/core/win32/jimi_targetver.h>
#endif

#include <jimi/core/jimi_config.h>
#include <jimi/core/jimi_export.h>

// Define type size_t
#ifndef _SIZE_T_DEFINED
#include <stddef.h>
#endif

#ifndef __has_feature
#define __has_feature(X)        (0)
#endif

// Is noexcept supported?
#if defined(__clang__) && __has_feature(cxx_noexcept) || \
    defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
    defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 180021114 && _MSC_FULL_VER >= 190021114)
    // support noexcept
    #define NOEXCEPT        noexcept
#else  /* ! Is noexcept supported? */

    #if defined(_MSC_VER)
      #include <yvals.h>
      #ifndef NOEXCEPT
        #define NOEXCEPT    _NOEXCEPT
        //#define NOEXCEPT
      #endif
    #endif  /* _MSC_VER */

#endif  /* Is noexcept supported? */

// linux maybe need #include <sys/types.h>

// Define integer types with known size: int32_t, uint32_t, int64_t, uint64_t.
// If this doesn't work then insert compiler-specific definitions here:
// (stdint.h defined from visual studio 2010 (_MSC_VER == 1600))
#if defined(JIMI_IS_GNUC) || (defined(JIMI_MSC_VER) && JIMI_MSC_VER >= 1600)
  // Compilers supporting C99 or C++0x have stdint.h defining these integer types
  #include <stdint.h>
  #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
#elif defined(JIMI_MSC_VER)
  #include <jimi/core/win32/vs_stdint.h>
#else
  #ifndef _STDINT
    #define _STDINT
    // This works with most compilers
    typedef signed   short int  int16_t;
    typedef unsigned short int uint16_t;
    typedef signed   int        int32_t;
    typedef unsigned int       uint32_t;
    typedef long long           int64_t;
    typedef unsigned long long uint64_t;
    #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
  #endif
#endif

#include <jimi/core/jimi_common.h>

#define USING_NS_JIMI           using namespace jimi;

// namespace jimi
#define NS_JIMI_BEGIN           namespace jimi {
#define NS_JIMI_END             }

#define USING_NS_JIMI_CORE      using namespace jimi::core;
#define USING_NS_JIMI_SYSTEM    using namespace jimi::system;
#define USING_NS_JIMI_IOCP      using namespace jimi::iocp;
#define USING_NS_JIMI_FS        using namespace jimi::fs;
#define USING_NS_JIMI_IO        using namespace jimi::io;

#define USING_NS_JIMI_SUB(NS)   using namespace jimi::##NS##;

// namespace jimi::internal
#define NS_JIMI_INTERNAL_BEGIN  namespace internal {
#define NS_JIMI_INTERNAL_END    }

// namespace jimi::core
#define NS_JIMI_CORE_BEGIN      namespace core {
#define NS_JIMI_CORE_END        }

// namespace jimi::system
#define NS_JIMI_SYSTEM_BEGIN    namespace system {
#define NS_JIMI_SYSTEM_END      }

// namespace jimi::ui
#define NS_JIMI_UI_BEGIN        namespace ui {
#define NS_JIMI_UI_END          }

// namespace jimi::util
#define NS_JIMI_UTIL_BEGIN      namespace util {
#define NS_JIMI_UTIL_END        }

// namespace jimi::iocp
#define NS_JIMI_IOCP_BEGIN      namespace iocp {
#define NS_JIMI_IOCP_END        }

// namespace jimi::fs
#define NS_JIMI_FS_BEGIN        namespace fs {
#define NS_JIMI_FS_END          }

// namespace jimi::io
#define NS_JIMI_IO_BEGIN        namespace io {
#define NS_JIMI_IO_END          }

/**
 * for assert defines
 */
#include <jimi/core/jimi_assert.h>

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

#define NOMINMAX

#define JIMI_MIN(a, b)          ((a) < (b) ? (a) : (b))
#define JIMI_MAX(a, b)          ((a) > (b) ? (a) : (b))

#if defined(_WIN64) || defined(_MX64)
#define JIMI_SIZE_T_SIZEOF      8
#else
#define JIMI_SIZE_T_SIZEOF      4
#endif

/**
 * macro for address aligned to n bytes
 */
#define JIMI_ALIGNED_TO(n, alignment)   \
    (((n) + ((alignment) - 1)) & ~size_t((alignment) - 1))

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

NS_JIMI_BEGIN

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

NS_JIMI_END

/**
 * for c++ const_cast
 */
#define CONST_CAST(_Val, _TypeVal, _TypeNew) \
    reinterpret_cast<_TypeNew>(const_cast<_TypeVal>(_Val))

#define CONST_CAST_CONST(_Val, _TypeVal, _TypeNew) \
    const_cast<const _TypeNew>(reinterpret_cast<_TypeNew>(const_cast<_TypeVal>(_Val)))

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

NS_JIMI_BEGIN

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

NS_JIMI_END

#endif  /* _JIMI_DEF_H_ */
