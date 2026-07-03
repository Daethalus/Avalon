#pragma once


typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef unsigned long int  ul32;

typedef signed char      i8;
typedef signed short     i16;
typedef signed int       i32;
typedef signed long long i64;

typedef float  f32;
typedef double f64;

typedef void*               VoidPtr;
typedef const void*         ConstPtr;
typedef char*               CharPtr;
typedef const char*         ConstChr;
typedef decltype(sizeof(0)) usize;


#if _WIN64
	#define AV_WIN 1
	#define AV_API __declspec(dllexport)
#endif