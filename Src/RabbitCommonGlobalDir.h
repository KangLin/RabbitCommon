#ifndef RABBITCOMM_CGLOBALDIR_H
#define RABBITCOMM_CGLOBALDIR_H

#include <QString>

class CRabbitCommonGlobalDir
{
public:
    CRabbitCommonGlobalDir();
    
    static CRabbitCommonGlobalDir* Instance();
     
    QString GetDirApplication();

    QString GetDirConfig();
    QString GetDirApplicationXml();
    QString GetDirDocument();
    int SetDirDocument(QString szPath);
    
    QString GetDirData();
    QString GetDirImage();
    
    QString GetDirTranslations();

    QString GetApplicationConfigureFile();
    
    QString GetUserConfigureFile();
   
private:
    QString m_szDocumentPath;
};

#endif // RABBITCOMM_CGLOBALDIR_H
