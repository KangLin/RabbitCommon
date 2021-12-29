#ifndef CGENERATEUPDATEFILE_H
#define CGENERATEUPDATEFILE_H

#include <QCommandLineParser>

class CGenerateUpdateFile
{
public:
    CGenerateUpdateFile();
    
    /**
     * @brief Update XML file used only to generate programs
     * @return 
     */
    int GenerateUpdateXml();
    int GenerateUpdateXml(QCommandLineParser &parser);
    
    int SetVersion(const QString &szVersion);
    int SetArch(const QString &szArch);
    
private:
    struct INFO{
        QString szVerion;
        QString szInfomation;
        QString szTime;
        bool bForce;
        QString szSystem;
        QString szPlatform;
        QString szArchitecture;
        QString szUrl;
        QString szUrlHome;
        QString szMd5sum;
        QString szMinUpdateVersion;
    } m_Info;
    
    int GenerateUpdateXmlFile(const QString &szFile, const INFO &info);
    
    QString m_szCurrentVersion;
    QString m_szCurrentArch;
    QString m_szPlatform;
};

#endif // CGENERATEUPDATEFILE_H
