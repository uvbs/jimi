
#ifndef _JIMI_EXPORT_H_
#define _JIMI_EXPORT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>
#include <jimi/platform/jimi_compiler_config.h>

#if defined(BUILD_DLL) || defined(JIMI_BUILD_DLL) || defined(_USRDLL) \
    || defined(JIMI_EXPORT) || defined(JIMI_DECLARE_EXPORT) || defined(DECLARE_EXPORT) \
    || defined(JIMI_SHARED) || defined(JIMI_ENABLE_SHARED) || defined(ENABLE_SHARED)
    /* is a dll library */
    #undef JIMI_DECLARE_STATIC
    #undef JIMI_DECLARE_IMPORT
    #ifndef JIMI_DECLARE_EXPORT
        #define JIMI_DECLARE_EXPORT
    #endif
#elif defined(USE_DLL) || defined(JIMI_USE_DLL) || defined(JIMI_HAS_DLL) \
    || defined(JIMI_IMPORT) || defined(JIMI_DECLARE_IMPORT) || defined(DECLARE_IMPORT) \
    || defined(JIMI_USE_SHARED) || defined(USE_SHARED)
    /* use a dll library */
    #undef JIMI_DECLARE_STATIC
    #undef JIMI_DECLARE_EXPORT
    #ifndef JIMI_DECLARE_IMPORT
        #define JIMI_DECLARE_IMPORT
    #endif
#else /* defined(_LIB) || defined(JIMI_LIB) || defined(JIMI_IS_LIB) \
         || defined(JIMI_USE_LIB) || defined(JIMI_USE_STATIC) \
         || defined(JIMI_STATIC) || defined(JIMI_DECLARE_STATIC) || defined(DECLARE_STATIC) \
         || defined(JIMI_ENABLED_STATIC) || defined(ENABLED_STATIC) */
    /* is a static library or use a static library */
    #undef JIMI_DECLARE_EXPORT
    #undef JIMI_DECLARE_IMPORT
    #ifndef JIMI_DECLARE_STATIC
        #define JIMI_DECLARE_STATIC
    #endif
#endif

/**************************************************************************************
   Dynamic Library import / export / static control conditional
   (Define JIMI_DECLARE_EXPORT to export symbols, else they are imported or static)
**************************************************************************************/

#ifdef JIMI_DLL
    #undef JIMI_DLL
#endif

#if defined(JIMI_IS_WINDOWS) && (JIMI_IS_WINDOWS != 0)  /* is windows os? */
    #if defined(JIMI_DECLARE_EXPORT)              /* is a dll library */
        #define JIMI_DLL                __declspec(dllexport)
        #define JIMI_API                __declspec(dllexport)
        #define JIMI_API_TPL            __declspec(dllexport)
        #define JIMI_PRIVATE
        #define JIMI_EXPIMP_TEMPLATE
    #elif defined(JIMI_DECLARE_IMPORT)            /* use a dll library */
        #define JIMI_DLL                __declspec(dllimport)
        #define JIMI_API                __declspec(dllimport)
        #define JIMI_API_TPL            __declspec(dllimport)   // or don't defined it!
        #define JIMI_PRIVATE
        #define JIMI_EXPIMP_TEMPLATE    extern
    #else /* defined(JIMI_DECLARE_STATIC) */      /* is a static library or use static library */
        #define JIMI_DLL
        #define JIMI_API
        #define JIMI_API_TPL
        #define JIMI_PRIVATE
        #define JIMI_EXPIMP_TEMPLATE
    #endif
#elif defined(JIMI_GNUC_)   
    #define JIMI_DLL                    __attribute__ ((visibility ("default")))
    #define JIMI_API                    __attribute__ ((visibility ("default")))
    #define JIMI_API_TPL                __attribute__ ((visibility ("default")))
    #define JIMI_PRIVATE                __attribute__ ((visibility ("default")))
    #if defined(JIMI_DECLARE_IMPORT)
        #define JIMI_EXPIMP_TEMPLATE    extern
    #else
        #define JIMI_EXPIMP_TEMPLATE
    #endif
#else  /* not is windows os or gunc! */
    #define JIMI_DLL
    #define JIMI_API
    #define JIMI_API_TPL
    #define JIMI_PRIVATE
    #if defined(JIMI_DECLARE_IMPORT)
        #define JIMI_EXPIMP_TEMPLATE    extern
    #else
        #define JIMI_EXPIMP_TEMPLATE
    #endif
#endif

#endif  /* _JIMI_EXPORT_H_ */
