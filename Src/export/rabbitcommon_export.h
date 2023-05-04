#ifndef RABBITCOMMON_EXPORT_H
#define RABBITCOMMON_EXPORT_H

#include <qglobal.h>

#if defined (Q_OS_WIN)
	#include "rabbitcommon_export_windows.h"
#endif

#if defined(Q_OS_LINUX) || defiend(Q_OS_MACOS)
    #include "rabbitcommon_export_linux.h"
#endif

#endif //RABBITCOMMON_EXPORT_H
