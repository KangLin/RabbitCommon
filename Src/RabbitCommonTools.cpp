#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "AdminAuthoriser/adminauthoriser.h"

#include <QApplication>
#include <QDir>

inline void g_RabbitCommon_InitResource()
{
    Q_INIT_RESOURCE(ResourceRabbitCommon);
#if defined(Q_OS_ANDROID) || _DEBUG
    Q_INIT_RESOURCE(translations_RabbitCommon);
#endif
}

inline void g_RabbitCommon_CleanResource()
{
    Q_CLEANUP_RESOURCE(ResourceRabbitCommon);
#if defined(Q_OS_ANDROID) || _DEBUG
    Q_CLEANUP_RESOURCE(translations_RabbitCommon);
#endif
}

namespace RabbitCommon {

CTools::CTools()
{   
}

CTools::~CTools()
{
}

CTools* CTools::Instance()
{
    static CTools* pTools = nullptr;
    if(nullptr == pTools)
        pTools = new CTools();
    return pTools;
}

void CTools::Init()
{
    InitResource();
    InitTranslator();
}

void CTools::InitTranslator()
{
    m_Translator.load(CDir::Instance()->GetDirTranslations()
                      + "/RabbitCommon_" + QLocale::system().name() + ".qm");
    qApp->installTranslator(&m_Translator);
}

void CTools::CleanTranslator()
{
    qApp->removeTranslator(&m_Translator);    
}

void CTools::InitResource()
{
    g_RabbitCommon_InitResource();
}

void CTools::CleanResource()
{
    g_RabbitCommon_CleanResource();
}

bool CTools::execute(const QString &program, const QStringList &arguments)
{
    return CAdminAuthoriser::Instance()->execute(program, arguments);
}

} //namespace RabbitCommon
