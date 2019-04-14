#include "RabbitCommonTools.h"

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

void CRabbitCommonTools::InitTranslator()
{
    QString szPre;    
#if defined(Q_OS_ANDROID) || _DEBUG
    szPre = ":/Translations";
#else
    szPre = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + "translations";
#endif
    m_Translator.load(szPre + "/RabbitCommon_" + QLocale::system().name() + ".qm");
    qApp->installTranslator(&m_Translator);
}

void CRabbitCommonTools::CleanTranslator()
{
    qApp->removeTranslator(&m_Translator);    
}
