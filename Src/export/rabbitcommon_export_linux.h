
#ifndef RABBITCOMMON_EXPORT_LINUX_H
#define RABBITCOMMON_EXPORT_LINUX_H

#ifdef RABBITCOMMON_STATIC_DEFINE
#  define RABBITCOMMON_EXPORT
#  define RABBITCOMMON_NO_EXPORT
#else
#  ifndef RABBITCOMMON_EXPORT
#    ifdef RABBITCOMMON_EXPORTS
        /* We are building this library */
#      define RABBITCOMMON_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define RABBITCOMMON_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef RABBITCOMMON_NO_EXPORT
#    define RABBITCOMMON_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef RABBITCOMMON_DEPRECATED
#  define RABBITCOMMON_DEPRECATED __attribute__ ((__deprecated__))
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

#endif //RABBITCOMMON_EXPORT_LINUX_H
