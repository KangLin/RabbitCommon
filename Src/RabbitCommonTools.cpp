#include "RabbitCommonTools.h"
#include "RabbitCommonGlobalDir.h"
#include <QApplication>
#include <QDir>

CRabbitCommonTools::CRabbitCommonTools()
{   
}

CRabbitCommonTools::~CRabbitCommonTools()
{
}

CRabbitCommonTools* CRabbitCommonTools::Instance()
{
    static CRabbitCommonTools* pTools = nullptr;
    if(nullptr == pTools)
        pTools = new CRabbitCommonTools();
    return pTools;
}

void CRabbitCommonTools::Init()
{
    InitResource();
    InitTranslator();
}

void CRabbitCommonTools::InitTranslator()
{
    m_Translator.load(CRabbitCommonGlobalDir::Instance()->GetDirTranslations()
                      + "/RabbitCommon_" + QLocale::system().name() + ".qm");
    qApp->installTranslator(&m_Translator);
}

void CRabbitCommonTools::CleanTranslator()
{
    qApp->removeTranslator(&m_Translator);    
}

void CRabbitCommonTools::InitResource()
{
    Q_INIT_RESOURCE(ResourceRabbitCommon);
#if defined(Q_OS_ANDROID) || _DEBUG
    Q_INIT_RESOURCE(translations_RabbitCommon);
#endif
}

void CRabbitCommonTools::CleanResource()
{
    Q_CLEANUP_RESOURCE(ResourceRabbitCommon);
#if defined(Q_OS_ANDROID) || _DEBUG
    Q_CLEANUP_RESOURCE(translations_RabbitCommon);
#endif
}
