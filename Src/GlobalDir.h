#ifndef CGLOBALDIR_H
#define CGLOBALDIR_H

#include <QString>

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

#endif // CGLOBALDIR_H
