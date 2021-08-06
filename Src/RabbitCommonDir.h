/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Directory operator
 */

#ifndef RABBITCOMM_CGLOBALDIR_H
#define RABBITCOMM_CGLOBALDIR_H

#pragma once

#include <QString>
#include <QFileDialog>
#include "rabbitcommon_export.h"

namespace RabbitCommon {

/**
 * 
 * Default directory:
 * \code
 * ApplicationInstallRoot                          GetDirApplicationInstallRoot()
 *       |- bin                                    GetDirApplication()
 *       |- etc                                    GetDirConfig()
 *       |   |- xml                                GetDirApplicationXml()
 *       |   |- applicationName.conf               GetApplicationConfigureFile()
 *       |- translations                           GetDirTranslations()
 *       |- data                                   GetDirData()
 *       |   |- db                                 GetDirDatabase()
 *       |       |- database.db                    GetDirDatabaseFile()
 *       |- plugins                                GetDirPlugins()
 *
 * DocumentRoot/Rabbit/applicationName             GetDirUserDocument()
 *       |- applicationName.conf                   GetFileUserConfigure()
 *       |- etc                                    GetDirUserConfig()
 *       |- data                                   GetDirUserData()
 *       |    |- image                             GetDirUserImage()
 *       |    |- db                                GetDirUserDatabase()
 *       |       |- database.db                    GetDirUserDatabaseFile()
 *       |    |- xml                               GetDirUserXml()
 *
 *
 * Android:
 *    assets                                       GetDirApplicationInstallRoot()  (Only read)
 *       |- etc                                    GetDirConfig()
 *       |   |- xml                                GetDirApplicationXml()
 *       |   |- applicationName.conf               GetApplicationConfigureFile()
 *       |- translations                           GetDirTranslations()
 *       |- data                                   GetDirData()
 *       |   |- db                                 GetDirDatabase()
 *       |       |- database.db                    GetDirDatabaseFile()
  
 *    DocumentRoot/Rabbit/applicationName          (Write and read)
 *       |- root                                 
 *       |    |- etc                               GetDirConfig()
 *       |    |   |- xml                           GetDirApplicationXml()
 *       |    |   |- applicationName.conf          GetApplicationConfigureFile() 
 *       |    |- data                              GetDirData()
 *       |    |   |- db                            GetDirDatabase()
 *       |        |- database.db                   GetDirDatabaseFile()
 *
 * \endcode
 * 
 * NOTE: In android, copy contents to DocumentRoot/Rabbit/applicationName from assets
 * 
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CDir
{
public:
    CDir();
    
    static CDir* Instance();
     
    QString GetDirApplication();
    int SetDirApplication(const QString &szPath);
    QString GetDirApplicationInstallRoot();
    int SetDirApplicationInstallRoot(const QString &szPath);
    QString GetDirConfig(bool bReadOnly = false);
    QString GetDirData(bool bReadOnly = false);
    QString GetDirDatabase(bool bReadOnly = false);
    QString GetDirDatabaseFile(const QString &szFile = QString(), bool bReadOnly = false);
    QString GetDirApplicationXml(bool bReadOnly = false);
    QString GetDirTranslations();
    QString GetDirPlugins();
    QString GetFileApplicationConfigure(bool bReadOnly = false);
    
    QString GetDirUserDocument();
    int SetDirUserDocument(QString szPath);
    QString GetDirUserConfig();
    QString GetDirUserData();
    QString GetDirUserDatabase();
    QString GetDirUserDatabaseFile(const QString &szFile = QString());
    QString GetDirUserXml();
    QString GetDirUserImage();
    QString GetFileUserConfigure();
    
    static int CopyDirectory(const QString &fromDir,
                      const QString &toDir,
                      bool bCoverIfFileExists = true);
    static QString GetOpenDirectory(QWidget *parent = nullptr,
                      const QString &caption = QString(),
                      const QString &dir = QString(),
                      QFileDialog::Options options = QFileDialog::Options());
    static QString GetOpenFileName(QWidget *parent = nullptr,
                      const QString &caption = QString(),
                      const QString &dir = QString(),
                      const QString &filter = QString(),
                      QFileDialog::Options options = QFileDialog::Options());
    static QString GetSaveFileName(QWidget *parent = nullptr,
                      const QString &caption = QString(),
                      const QString &dir = QString(),
                      const QString &filter = QString(),
                      QFileDialog::Options options = QFileDialog::Options());
private:
    QString m_szDocumentPath;
    QString m_szApplicationDir;
    QString m_szApplicationRootDir;

};

} //namespace RabbitCommon

#endif // RABBITCOMM_CGLOBALDIR_H
