
#ifndef _JIMI_LANG_STRING_CORE_H_
#define _JIMI_LANG_STRING_CORE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimi/thread/Atomic.h>
#include <jimi/lang/Char_Traits.h>
#include <jimi/lang/RefCounted.h>
#include <jimi/lang/StringCore.h>
#include <jimic/string/jm_strings.h>
#include <jimic/string/jmf_strings.h>

#pragma warning(disable : 4995)         // ��ֹ����_s�����ķ���warning��Ϣ
#pragma warning(disable : 4996)

#include <string>
using namespace std;

#define JIMI_STRING_PERVERSE
#define JIMI_STRING_CONSERVATIVE


NS_JIMI_BEGIN

/// <comment>
///
/// ��׼C������string��Copy-On-Write������һ��
///
/// Reference:
///    http://haoel.blog.51cto.com/313033/124638
///
/// ����Facebook��ԴC++��folly(1)��string������
///
/// Reference:
///    http://www.cnblogs.com/promise6522/archive/2012/06/05/2535530.html
///
/// </comment>

#define STRING_SMALL_SIZE       32

#define STRING_MEDIUM_SIZE      256

#define STRING_NULL_CHAR        '\0'

typedef enum StringTypeMask
{
    STRING_TYPE_SMALL       = (sizeof(size_t) == 4) ? 0x01000000 : 0x0100000000000000,
    STRING_TYPE_MEDIUM      = (sizeof(size_t) == 4) ? 0x02000000 : 0x0200000000000000,
    STRING_TYPE_LARGE       = (sizeof(size_t) == 4) ? 0x04000000 : 0x0400000000000000,
    STRING_TYPE_CONSTANT    = (sizeof(size_t) == 4) ? 0x08000000 : 0x0800000000000000,
    STRING_TYPE_MASK        = (STRING_TYPE_SMALL
        | STRING_TYPE_MEDIUM | STRING_TYPE_LARGE | STRING_TYPE_CONSTANT),
} StringTypeMask;

typedef enum StringTypeMaskX
{
    STRING_TYPE_SMALL_X     = 0x01,
    STRING_TYPE_MEDIUM_X    = 0x02,
    STRING_TYPE_LARGE_X     = 0x04,
    STRING_TYPE_CONSTANT_X  = 0x08,
    STRING_TYPE_MASK_X      = (STRING_TYPE_SMALL_X
        | STRING_TYPE_MEDIUM_X | STRING_TYPE_LARGE_X | STRING_TYPE_CONSTANT_X),
} StringTypeMaskX;

#define STRING_TYPE_IS(type, mask)      ((type & mask) != 0)

#define STRING_TYPE_IS_SMALL(type)      ((type & STRING_TYPE_SMALL)     != 0)
#define STRING_TYPE_IS_MEDIUM(type)     ((type & STRING_TYPE_MEDIUM)    != 0)
#define STRING_TYPE_IS_LARGE(type)      ((type & STRING_TYPE_LARGE)     != 0)
#define STRING_TYPE_IS_CONSTANT(type)   ((type & STRING_TYPE_CONSTANT)  != 0)

#define STRING_TYPE_NOT_IS_SMALL(type)   \
    ((type & (STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT))) != 0)

#define STRING_TYPE_IS_MEDIUM_OR_LARGE(type)   \
    ((type & (STRING_TYPE_MEDIUM | STRING_TYPE_LARGE)) != 0)

#define STRING_CORE_CLASSES    \
    class _CharT, class _RefCount
#define STRING_CORE            \
    string_core<_CharT, _RefCount>

template <class _CharT, class _RefCount = refcounted<_CharT, size_t> >
class JIMI_API string_core
{
public:
    // Types
    typedef _CharT              char_type;
    typedef _RefCount           refcount_type;
    typedef size_t              size_type;
    typedef size_t              flag_type;
    typedef char_traits<_CharT> traits_type;
    
    struct small_info_t;
    struct small_t;
    struct core_data_t;
    struct medium_large;
    
    typedef struct small_info_t small_info_t;
    typedef struct small_t      small_t;
    typedef struct core_data_t  core_data_t;
    typedef struct medium_large medium_large; 

    // Constant
    static const flag_type      kIsSmall    =  STRING_TYPE_SMALL;
    static const flag_type      kIsMedium   =  STRING_TYPE_MEDIUM;
    static const flag_type      kIsLarge    =  STRING_TYPE_LARGE;
    static const flag_type      kIsConstant =  STRING_TYPE_CONSTANT;
    static const flag_type      kTypeMask   =  STRING_TYPE_MASK;
    static const flag_type      kSizeMask   = ~STRING_TYPE_MASK;

    static const flag_type      kIsMediumOrLargeMask = STRING_TYPE_MEDIUM | STRING_TYPE_LARGE;
    static const flag_type      kNotIsSmallMask      = STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT);

    static const size_type      kMaxSmallSize   = (STRING_SMALL_SIZE - sizeof(small_info_t)) / sizeof(char_type);
    static const size_type      kMaxMediumSize  = STRING_MEDIUM_SIZE;

public:
    // Contructor
    string_core();
    string_core(const string_core &src);
    string_core(const char *src);
    string_core(const char * const src, const size_t size);
    string_core(const wchar_t * const src, const size_t size);
    ~string_core();

    void destroy();

    void retail();
    void release();

    bool equals(const string_core &rhs) const;

    int compare(const string_core &rhs) const;
    int compare(const char_type *rhs) const;

    flag_type getType() const       { return (_ml.type & kTypeMask); }
    flag_type getTypeValue() const  { return _ml.type;               }
    flag_type getTypeMask() const   { return kTypeMask;              }

    bool is_small() const       { return STRING_TYPE_IS_SMALL(_ml.type);    }
    bool is_medium() const      { return STRING_TYPE_IS_MEDIUM(_ml.type);   }
    bool is_large() const       { return STRING_TYPE_IS_LARGE(_ml.type);    }
    bool is_constant() const    { return STRING_TYPE_IS_CONSTANT(_ml.type); }

    bool is_medium_or_large() const { return STRING_TYPE_IS_MEDIUM_OR_LARGE(_ml.type); }

    bool not_is_small() const { return STRING_TYPE_NOT_IS_SMALL(_ml.type); }

    bool is_shared() const {
        return (is_large() && (refcount_type::refs(_ml.data) > 1));
    }

    static bool is_small(const flag_type type)      { return STRING_TYPE_IS_SMALL(type);    }
    static bool is_medium(const flag_type type)     { return STRING_TYPE_IS_MEDIUM(type);   }
    static bool is_large(const flag_type type)      { return STRING_TYPE_IS_LARGE(type);    }
    static bool is_constant(const flag_type type)   { return STRING_TYPE_IS_CONSTANT(type); }

    static bool is_medium_or_large(const flag_type type) { return STRING_TYPE_IS_MEDIUM_OR_LARGE(type); }

    static bool not_is_small(const flag_type type) { return STRING_TYPE_NOT_IS_SMALL(type); }

    static bool is_shared(const medium_large &ml) {
        return (is_large(ml.type) && (refcount_type::refs(ml.data) > 1));
    } 

    const char_type *data() const { return c_str(); }
    const char_type *c_str() const;

    size_type size() const;
    size_type capacity() const;

    // swap below doesn't test whether &rhs == this (and instead
    // potentially does extra work) on the premise that the rarity of
    // that situation actually makes the check more expensive than is
    // worth.
    void swap(string_core &rhs);

    // Don't disabled
    string_core & operator = (const string_core &rhs);

    void reserve(size_type minCapacity);

    void expandTo(const size_type newSize);
    void shrinkTo(const size_type newSize);

    void writeNull();
    
protected:
    size_t calc_capacity(size_t src_len);

private:

    void small_clone(small_t &dest, const small_t &src) {
        dest.info.lastShort = src.info.lastShort;
        traits_type::strncpy_u(dest.buf, src.buf, src.info.size);
        dest.buf[src.info.size] = '\0';
    }

    void ml_clone(medium_large &dest, const medium_large &src) {
        dest.data = src.data;
        dest.size = src.size;
        dest.capacity = src.capacity;
        dest.type = src.type;
    }

private:
    /* lastShort ��ʱ������ṩЧ�� */
    struct small_info_t {
        union {
            struct {
                unsigned char size;
                unsigned char type;
            };
            unsigned short    lastShort;
        };
    };

    /* small object buffer */
    struct small_t {
        union {
            struct {
                /* (dummyֻ��ռλ��, δʹ��) */
                unsigned char dummy[(STRING_SMALL_SIZE - sizeof(small_info_t)) / sizeof(char)];
                /* size and type */
                small_info_t  info;
            };
            char_type buf[(STRING_SMALL_SIZE - sizeof(small_info_t)) / sizeof(char_type)];
        };
    };

    /* ����ṹֻ��Ϊ�˶�̬����buf��size�����ڵ� */
    struct core_data_t {
        char_type  *data;
        size_type   size;
        size_type   capacity;
        flag_type   type;
    };

    /* medium �� large ����һ���Ľṹ */
    struct medium_large {
        union {
            struct {
                /* (dummyֻ��ռλ��, δʹ��) */
                unsigned char dummy[(STRING_SMALL_SIZE - sizeof(core_data_t)) / sizeof(char)];

                /* ����Ķ�������core_data�Ľṹһ�� */
                char_type *data;
                size_type  size;
                size_type  capacity;
                flag_type  type;
            };

            char_type buf[(STRING_SMALL_SIZE - sizeof(core_data_t)) / sizeof(char_type)];
        };
    };

private:
    /* ����һ��union����, ��small_t���ͺ�medium_large���͹�����ͬһ���ڴ�ṹ */
    union {
        /* mutable���η������const���ε�, ��ʹ���ڱ�const���ι��ĳ�Ա������, */
        /* Ҳ���Ըı䱻mutable���ι��ĳ�Ա���� */
        /* Reference: http://blog.csdn.net/wuliming_sc/article/details/3717017 */
        mutable small_t      _small;
        mutable medium_large _ml;
    };
};

template <STRING_CORE_CLASSES>
STRING_CORE::string_core()
{
#if 1
    _ml.type = STRING_TYPE_SMALL;
    (*(size_t *)(&_small.buf[0])) = 0;
#else
    _ml.data = NULL;
    _ml.size = 0;
    _ml.capacity = 0;
    _ml.type = 0;

    // init sso buffer
    (*(size_t *)(&_ml.buf[0])) = 0;
#endif
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const string_core &src)
{
    jimi_assert(&src != this);
    flag_type type = src.getType();
    /* small object */
    if (is_small(type)) {
#if 0
        _small.info.type = src._small.info.type;
        _small.info.size = src._small.info.size;
#else
        _small.info.lastShort = src._small.info.lastShort;
#endif
        jimi_assert(src._small.info.size < STRING_SMALL_SIZE);
        if (_small.info.size < STRING_SMALL_SIZE) {
            char_traits<char>::strlcpy(&_small.buf[0], STRING_SMALL_SIZE, &src._small.buf[0], _small.info.size);
        }
        else {
            sLog.error("string_core(const string_core &src): data = %s, type = 0x%04X, size = %d.",
                &_small.buf[0], _small.info.type, _small.info.size);
        }
    }
    /* eager copy */
    else if (is_medium(type)) {
        jimi_assert(src._ml.capacity == STRING_MEDIUM_SIZE);
        _ml.size = src._ml.size;
        _ml.capacity = STRING_MEDIUM_SIZE;
        _ml.type = src._ml.type;
        _ml.data = char_traits<char>::assign(STRING_MEDIUM_SIZE);
        char_traits<char>::strlcpy(_ml.data, STRING_MEDIUM_SIZE, src._ml.data, src._ml.size);
    }
    /* copy-on-write */
    else if (is_large(type)) {
        _ml.data     = src._ml.data;
        _ml.capacity = src._ml.capacity;
        const_cast<string_core &>(src).retail();
    }
    /* unknown type */
    else {
        sLog.error("string_core(const string_core &src): type = 0x%04X, data = %08X, size() = %d.", getType(), _ml.data, _ml.size);
    }
}

#if 1

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char *src)
{
#if 0
    size_t src_len = ::strnlen(src, kMaxMediumSize);
#else
    size_t src_len = ::strlen(src);
#endif
    /* small object */
    if (src_len < kMaxSmallSize) {
        _small.info.type = STRING_TYPE_SMALL_X;
        _small.info.size = src_len;
#if 1
        char_traits<char>::strncpy_u(_small.buf, src, src_len);
        _small.buf[src_len] = '\0';
#else
        ::strcpy(_small.buf, src);
#endif
    }
#if 1
    /* eager copy */
    else if (src_len < kMaxMediumSize) {
        _ml.size = src_len;
        _ml.capacity = calc_capacity(src_len);
        _ml.type = STRING_TYPE_MEDIUM;
        _ml.data = char_traits<char>::assign(_ml.capacity);
        if (_ml.data) {
            char_traits<char>::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
    /* copy-on-write or unknown type */
    else {
        size_t src_left;
        src_left = ::strlen(src + src_len);
        src_len += src_left;
        _ml.capacity = calc_capacity(src_len);
        _ml.data = char_traits<char>::assign(_ml.capacity);
        _ml.size = src_len;
        _ml.type = STRING_TYPE_LARGE;
        if (_ml.data) {
            char_traits<char>::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
#endif
}

#endif

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char * const src, const size_t size)
{
    size_t src_len = size;
    /* small object */
    if (src_len < kMaxSmallSize) {
        _small.info.type = STRING_TYPE_SMALL_X;
        _small.info.size = src_len;
#if 1
        char_traits<char>::strncpy_u(_small.buf, src, src_len);
        _small.buf[src_len] = '\0';
#else
        strcpy(_small.buf, src);
#endif
    }
    /* eager copy */
    else if (src_len < kMaxMediumSize) {
        _ml.size = src_len;
        _ml.capacity = calc_capacity(src_len);
        _ml.type = STRING_TYPE_MEDIUM;
        _ml.data = char_traits<char>::assign(_ml.capacity);
        if (_ml.data) {
            char_traits<char>::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
    /* copy-on-write or unknown type */
    else {
        _ml.capacity = calc_capacity(src_len);
        _ml.data = char_traits<char>::assign(_ml.capacity);
        _ml.size = src_len;
        _ml.type = STRING_TYPE_LARGE;
        if (_ml.data) {
            char_traits<char>::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const wchar_t * const src, const size_t size)
{
    size_t src_len = size;
    /* small object */
    if (src_len < STRING_SMALL_SIZE) {
        _small.info.size = src_len;     
        _small.info.type = STRING_TYPE_SMALL;
        char_traits<wchar_t>::strncpy(_ml.buf, jm_countof(_ml.buf), src, src_len);
    }
    /* eager copy */
    else if (src_len < STRING_MEDIUM_SIZE) {
        _ml.size = src_len;
        _ml.capacity = calc_capacity(src_len);
        _ml.type = STRING_TYPE_MEDIUM;
        _ml.data = char_traits<wchar_t>::assign(_ml.capacity);
        if (_ml.data)
            char_traits<wchar_t>::strncpy(_ml.data, _ml.capacity, src, src_len);
    }
    /* copy-on-write or unknown type */
    else {
        _ml.capacity = calc_capacity(src_len);
        _ml.data = char_traits<wchar_t>::assign(_ml.capacity);
        _ml.size = src_len;
        _ml.type = STRING_TYPE_LARGE;
        if (_ml.data)
            char_traits<wchar_t>::strncpy(_ml.data, _ml.capacity, src, src_len);
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::~string_core()
{
    destroy();
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::destroy()
{
    if (is_medium()) {
        if (_ml.data != NULL) {
            delete _ml.data;
            _ml.data = NULL;
        }
    }
    else if (is_large()) {
        if (_ml.data != NULL) {
            refcount_type::release(_ml.data);
            _ml.data = NULL;
        }
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::retail()
{
    if (is_large())
        _ml.type = STRING_TYPE_LARGE;
    refcount_type::retail(_ml.data);
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::release()
{
    if (is_medium()) {
        if (_ml.data != NULL) {
            delete _ml.data;
            _ml.data = NULL;
        }
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
    }
    else if (is_large()) {
        if (_ml.data != NULL) {
            refcount_type::release(_ml.data);
            _ml.data = NULL;
        }
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE & STRING_CORE::operator = (const STRING_CORE &rhs) {
    flag_type type = rhs.getType();
    if (type == kIsSmall)
        small_clone(_small, rhs._small);
    else
        ml_clone(_ml, rhs._ml);
    return *this;
}

// swap below doesn't test whether &rhs == this (and instead
// potentially does extra work) on the premise that the rarity of
// that situation actually makes the check more expensive than is
// worth.

template <STRING_CORE_CLASSES>
inline void STRING_CORE::swap(STRING_CORE &rhs)
{
#if 1
        // �ڲ�ͬ��type��, _ml����Щ�����ǲ��ظ��Ƶ�
        flag_type type = rhs.getType();
        if (type == kIsSmall) {
            small_t t;
            small_clone(t, _small);
            small_clone(_small, rhs._small);
            small_clone(rhs._small, t);
        }
        else {
            medium_large t;
            ml_clone(t, _ml);
            ml_clone(_ml, rhs._ml);
            ml_clone(rhs._ml, t);
        }
#else
        // ��ȫֱ�Ӹ���_ml, ��Щ���ƿ����Ƕ����
        const medium_large t = _ml;
        _ml = rhs._ml;
        rhs._ml = t;
#endif
}

template <STRING_CORE_CLASSES>
inline typename STRING_CORE::size_type STRING_CORE::size() const
{
    if (is_small())
        return _small.info.size;
    else
        return _ml.size;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::capacity() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return kMaxSmallSize - 1;
    else if (type == kIsMedium)
        return kMaxMediumSize - 1;
    else
        return _ml.capacity;
}

template <STRING_CORE_CLASSES>
inline const typename STRING_CORE::char_type *STRING_CORE::c_str() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return &_small.buf[0];
    else
        return _ml.data;
}

template <STRING_CORE_CLASSES>
inline size_t STRING_CORE::calc_capacity(size_t src_len)
{
    return src_len + 1;
}

template <STRING_CORE_CLASSES>
inline bool STRING_CORE::equals(const STRING_CORE &rhs) const
{
    return (&rhs == this);
}

template <STRING_CORE_CLASSES>
inline int STRING_CORE::compare(const STRING_CORE &rhs) const
{
    if (&rhs == this)
        return 0;

    int equal = -1;
    char_type *rhs_data;

    if (_ml.size == rhs._ml.size) {
        if (getType() == rhs.getType()) {
            if (is_small()) {
                equal = traits_type::strncmp(&_ml.buf[0], &rhs._ml.buf[0], _ml.size);
            }
            else if (not_is_small()) {
                rhs_data = rhs._ml.data;
                if (_ml.data == rhs_data)
                    equal = 0;
                else
                    equal = traits_type::strncmp(_ml.data, rhs_data, _ml.size);
            }
            else {
                // Unknown string type
                sLog.error("string_core::compare(const string_core &rhs) - Error: Unknown string type.");
            }
        }
        else {
            //
        }
    }
    else {
        // Is not same size
        sLog.info("string_core::compare(const string_core &rhs), size() = %d, rhs.size() = %d", size(), rhs.size());
    }
    return equal;
}

template <STRING_CORE_CLASSES>
inline int STRING_CORE::compare(const char_type *rhs) const
{
    int equal = -1;
    if (is_small()) {
        if (_ml.buf == rhs)
            equal = 0;
        else
            equal = traits_type::strncmp(_ml.buf, rhs, _ml.size);
    }
    else if (not_is_small()) {
        if (_ml.data == rhs)
            equal = 0;
        else
            equal = traits_type::strncmp(_ml.data, rhs, _ml.size);
    }
    else {
        // Unknown string type
        sLog.error("STRING_CORE::compare(const char_type *rhs) - Error: Unknown string type.");
    }
    return equal;
}

template <STRING_CORE_CLASSES>
inline int operator == (const STRING_CORE &lhs, const STRING_CORE &rhs)
{
    return lhs.compare(rhs);
}

template <STRING_CORE_CLASSES>
inline int operator == (const STRING_CORE &lhs, const _CharT *rhs)
{
    return lhs.compare(rhs);
}

template <STRING_CORE_CLASSES>
inline int operator == (const _CharT *lhs, const STRING_CORE &rhs)
{
    return rhs.compare(lhs);
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::reserve(size_t minCapacity)
{
    //
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::expandTo(const size_t newSize)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    size_t type = getType();
    if (type == kIsLarge) {
        //size_type _capacity = _ml.capacity;
        if (newSize > _ml.capacity)
            reserve(newSize);
    }
    else if (type == kIsMedium) {
        //size_type _capacity = kMaxMediumSize - 1;
        if (newSize > kMaxMediumSize - 1)
            reserve(newSize);
    }
    else if (type == kIsSmall) {
        //size_type _capacity = kMaxSmallSize - 1;
        if (newSize <= kMaxSmallSize - 1) {
            _small.info.size = newSize;
            _small.buf[newSize] = STRING_NULL_CHAR;
            return;
        }
        reserve(newSize);
    }
    /* is constant string or unknown type */
    else {
        return;
    }

    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);

    _ml.size = newSize;
    _ml.data[newSize] = STRING_NULL_CHAR;

    jimi_assert(size() == newSize);
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::shrinkTo(const size_t newSize)
{
    //
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::writeNull()
{
#if defined(JIMI_STRING_PERVERSE) || defined(JIMI_STRING_CONSERVATIVE)
    if (getType() == isSmall) {
        const size_type _size = _small.info.size;
        if (_size < kMaxSmallSize)
            _small.buf[_size] = STRING_NULL_CHAR;
    }
    else {
        _ml.data_[_ml.size_] = STRING_NULL_CHAR;
    }
#endif
}


NS_JIMI_END

#pragma warning(default : 4995)         // �ָ�Ĭ�ϵ�warning
#pragma warning(default : 4996)

#endif  /* _JIMI_LANG_STRING_CORE_H_ */