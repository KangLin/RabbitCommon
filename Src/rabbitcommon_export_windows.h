
#ifndef RABBITCOMMON_EXPORT_WINDOWS_H
#define RABBITCOMMON_EXPORT_WINDOWS_H

#ifdef RABBITCOMMON_STATIC_DEFINE
#  define RABBITCOMMON_EXPORT
#  define RABBITCOMMON_NO_EXPORT
#else
#  ifndef RABBITCOMMON_EXPORT
#    ifdef RabbitCommon_EXPORTS
        /* We are building this library */
#      define RABBITCOMMON_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define RABBITCOMMON_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef RABBITCOMMON_NO_EXPORT
#    define RABBITCOMMON_NO_EXPORT 
#  endif
#endif

#ifndef RABBITCOMMON_DEPRECATED
#  define RABBITCOMMON_DEPRECATED __declspec(deprecated)
#endif

#ifndef RABBITCOMMON_DEPRECATED_EXPORT
#  define RABBITCOMMON_DEPRECATED_EXPORT RABBITCOMMON_EXPORT RABBITCOMMON_DEPRECATED
#endif

#ifndef RABBITCOMMON_DEPRECATED_NO_EXPORT
#  define RABBITCOMMON_DEPRECATED_NO_EXPORT RABBITCOMMON_NO_EXPORT RABBITCOMMON_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef RABBITCOMMON_NO_DEPRECATED
#    define RABBITCOMMON_NO_DEPRECATED
#  endif
#endif

#endif /* RABBITCOMMON_EXPORT_WINDOWS_H */
