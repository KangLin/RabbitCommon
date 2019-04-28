#ifndef RABBITCOMM_CGLOBALDIR_H
#define RABBITCOMM_CGLOBALDIR_H

#include <QString>

namespace RabbitCommon {

class CGlobalDir
{
public:
    CGlobalDir();
    
    static CGlobalDir* Instance();
     
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

} //namespace RabbitCommon
#endif // RABBITCOMM_CGLOBALDIR_H
