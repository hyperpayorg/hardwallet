#ifndef JUB_SDK_COMM_H
#define JUB_SDK_COMM_H


// Generic helper definitions for shared library support
#if defined _MSC_VER || defined __CYGWIN__
    #define JUB_COINCORE_DLL_IMPORT extern "C" __declspec(dllimport)
    #define JUB_COINCORE_DLL_EXPORT extern "C" __declspec(dllexport)
    #define JUB_COINCORE_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define JUB_COINCORE_DLL_IMPORT __attribute__((visibility("default")))
        #define JUB_COINCORE_DLL_EXPORT __attribute__((visibility("default")))
        #define JUB_COINCORE_DLL_LOCAL  __attribute__((visibility("internal")))
    #else
        #define JUB_COINCORE_DLL_IMPORT
        #define JUB_COINCORE_DLL_EXPORT
        #define JUB_COINCORE_DLL_LOCAL
    #endif // #if __GNUC__ >= 4
#endif // #if defined _MSC_VER || defined __CYGWIN__

// Tag to deprecate functions and methods.
// Gives a compiler warning when they are used.
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_DEPRECATED __declspec(deprecated)
#else
    #if __GNUC__ >= 4
        #define JUB_DEPRECATED __attribute__((deprecated))
    #else
        #define JUB_DEPRECATED
    #endif // #if __GNUC__ >= 4
#endif // #if defined _MSC_VER || defined __CYGWIN__

// Avoid namespace conflict between boost::placeholders and std::placeholders.
#define BOOST_BIND_NO_PLACEHOLDERS

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

#if defined(_WIN32)
    #define HID_MODE
#elif defined(__APPLE__)
// see https://sourceforge.net/p/predef/mailman/message/34497133/
    #include <TargetConditionals.h>
    #if TARGET_OS_OSX // mac osx
        #define HID_MODE
    #elif TARGET_OS_IOS // ios
        #define BLE_MODE
    #endif // #if TARGET_OS_OSX
#elif defined(__ANDROID__)
    #define BLE_MODE
#else //other
    #define HID_MODE
#endif // #if defined(_WIN32)

#define JUBR_OK                     0x00000000UL

#define JUBR_ERROR                  0x00000001UL
#define JUBR_HOST_MEMORY            0x00000002UL
#define JUBR_ARGUMENTS_BAD          0x00000003UL
#define JUBR_IMPL_NOT_SUPPORT		0x00000004UL
#define JUBR_MEMORY_NULL_PTR		0x00000005UL

#define JUBR_INVALID_MEMORY_PTR		0x00000008UL
#define JUBR_REPEAT_MEMORY_PTR		0x00000009UL
#define JUBR_BUFFER_TOO_SMALL		0x0000000AUL

#define JUBR_MULTISIG_OK            0x0000000BUL

#define JUBR_QRC20_WRONG_FORMAT		0x0000000CUL

/* === Library typedef: === */
#ifndef IN
    #define IN
#endif // #ifndef IN

#ifndef OUT
    #define OUT
#endif // #ifndef OUT

#ifndef INOUT
    #define INOUT
#endif // #ifndef INOUT

#define JUB_TRUE    1
#define JUB_FALSE   0

#ifndef JUB_DISABLE_TRUE_FALSE
    #ifndef FALSE
        #define FALSE JUB_FALSE
    #endif // #ifndef FALSE

    #ifndef TRUE
        #define TRUE JUB_TRUE
    #endif // #ifndef TRUE
#endif // #ifndef JUB_DISABLE_TRUE_FALSE

/* an unsigned 8-bit value */
typedef unsigned char JUB_BYTE;

/* an unsigned 8-bit character */
typedef JUB_BYTE JUB_UCHAR;

/* an unsigned/signed 8-bit character, decide by complie*/
typedef char JUB_CHAR;

/* an 8-bit UTF-8 character */
typedef JUB_BYTE JUB_UTF8UCHAR;

/* a BYTE-sized Boolean flag */
typedef JUB_BYTE JUB_BBOOL;

/* an unsigned value, at least 32 bits long */
typedef unsigned long int JUB_ULONG;

/* a signed value, the same size as a JUB_ULONG */
typedef signed long int  JUB_LONG;

typedef JUB_BYTE JUB_UINT8;

typedef unsigned int JUB_UINT32;

typedef unsigned short JUB_UINT16;

/* uint64 */
typedef unsigned long long JUB_UINT64;

/* signed uint64 */
typedef signed long long JUB_INT64;

#define JUB_PTR *
typedef JUB_CHAR JUB_PTR            JUB_CHAR_PTR;
typedef JUB_CHAR_PTR JUB_PTR        JUB_CHAR_PTR_PTR;
typedef const JUB_CHAR JUB_PTR      JUB_CHAR_CPTR;
typedef const JUB_BYTE JUB_PTR      JUB_BYTE_CPTR;

typedef JUB_BYTE JUB_PTR            JUB_BYTE_PTR;
typedef JUB_UCHAR JUB_PTR           JUB_UCHAR_PTR;
typedef JUB_UTF8UCHAR JUB_PTR       JUB_UTF8UCHAR_PTR;
typedef JUB_ULONG JUB_PTR           JUB_ULONG_PTR;
typedef JUB_UINT16 JUB_PTR          JUB_UINT16_PTR;
typedef JUB_UINT32 JUB_PTR          JUB_UINT32_PTR;
typedef JUB_UINT64 JUB_PTR          JUB_UINT64_PTR;
typedef void JUB_PTR                JUB_VOID_PTR;

/* Pointer to a JUB_VOID_PTR-- i.e., pointer to pointer to void */
typedef JUB_VOID_PTR JUB_PTR JUB_VOID_PTR_PTR;

typedef JUB_ULONG JUB_RV;

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef enum {
    HEX = 0x00,
    XPUB = 0x01,
    PUB_FORMAT_NS_ITEM
} JUB_ENUM_PUB_FORMAT;

typedef struct stContextCfg {
    JUB_CHAR_PTR            mainPath;

     stContextCfg();
    virtual ~stContextCfg() {}
} CONTEXT_CONFIG;

typedef enum {
    BOOL_FALSE = 0,
    BOOL_TRUE,
    BOOL_NR_ITEMS
} JUB_ENUM_BOOL;

typedef struct stBip32Path {
	JUB_ENUM_BOOL change;
	JUB_UINT64    addressIndex;

     stBip32Path();
    ~stBip32Path() = default;
} BIP32_Path;

/*****************************************************************************
* @function name : JUB_ClearContext
* @in  param : contextID - context ID
* @out param : 
* @last change : 
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_ClearContext(IN JUB_UINT16 contextID);

/*****************************************************************************
* @function name : JUB_ShowVirtualPwd
* @in  param : contextID - context ID
* @out param : 
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_ShowVirtualPwd(IN JUB_UINT16 contextID);

/*****************************************************************************
* @function name : JUB_CancelVirtualPwd
* @in  param : contextID - context ID
* @out param : 
* @last change : 
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_CancelVirtualPwd(IN JUB_UINT16 contextID);

/*****************************************************************************
* @function name : JUB_VerifyPIN
* @in  param : contextID - context ID
*            : pinMix
* @out param : retry
* @last change : 
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_VerifyPIN(IN JUB_UINT16 contextID,
                     IN JUB_CHAR_PTR pinMix,
                     OUT JUB_ULONG_PTR pretry);

/*****************************************************************************
* @function name : JUB_FreeMemory
* @in  param : memPtr
* @out param : 
* @last change : 
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_FreeMemory(IN JUB_CHAR_CPTR memPtr);

/*****************************************************************************
* @function name : JUB_GetVersion
* @in  param :
* @out param : 
* @last change : 
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_CHAR_PTR JUB_GetVersion(void);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_COMM_H */
