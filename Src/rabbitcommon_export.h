#ifndef LUNARCALENDAR_EXPORT_H
#define LUNARCALENDAR_EXPORT_H

#if defined (Q_OS_WIN)
	#include "rabbitcommon_export_windows.h"
#endif

#if defined(Q_OS_LINUX)
    #include "rabbitcommon_export_linux.h"
#endif

#endif //LUNARCALENDAR_EXPORT_H
