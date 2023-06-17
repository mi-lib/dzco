/* dzco_export.h */
/* This file was automatically generated. */
/* 2023年  5月 22日 月曜日 08:02:27 JST by zhidao */
#ifndef __DZCO_EXPORT_H__
#define __DZCO_EXPORT_H__
#include <zeda/zeda_compat.h>
#if defined(__WINDOWS__) && !defined(__CYGWIN__)
# if defined(__DZCO_BUILD_DLL__)
#  define __DZCO_EXPORT extern __declspec(dllexport)
#  define __DZCO_CLASS_EXPORT  __declspec(dllexport)
# else
#  define __DZCO_EXPORT extern __declspec(dllimport)
#  define __DZCO_CLASS_EXPORT  __declspec(dllimport)
# endif
#else
# define __DZCO_EXPORT __EXPORT
# define __DZCO_CLASS_EXPORT
#endif
#endif /* __DZCO_EXPORT_H__ */
