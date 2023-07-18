/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Directory operator
 */

#ifndef RABBITCOMM_CGLOBALDIR_H
#define RABBITCOMM_CGLOBALDIR_H

#pragma once

#include <QString>
#if HAVE_RABBITCOMMON_GUI
#include <QFileDialog>
#endif
#include "rabbitcommon_export.h"

namespace RabbitCommon {

/**
 * 
 * Default directory:
 * \code
 * ApplicationInstallRoot(The folder is only read) GetDirApplicationInstallRoot()
 *       |- bin                                    GetDirApplication()
 *       |- etc                                    GetDirConfig()
 *       |   |- xml                                GetDirApplicationXml()
 *       |   |- applicationName.conf               GetFileApplicationConfigure()
 *       |- translations                           GetDirTranslations()
 *       |- log                                    GetDirLog()
 *       |- data                                   GetDirData()
 *       |   |- icons                              GetDirIcons()
 *       |   |- db                                 GetDirDatabase()
 *       |       |- database.db                    GetDirDatabaseFile()
 *       |- plugins                                GetDirPlugins()
 *       |   |- translations                       GetDirPluginsTranslation()
 *       |   |- szDir                              GetDirPlugins("plugins/szDir")
 *       |       | - translations                  GetDirPluginsTranslation("plugins/szDir")
 *
 * The follow folder is write and read:
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
 *    assets (The folder is only read)             GetDirApplicationInstallRoot()
 *       |- etc                                    GetDirConfig()
 *       |   |- xml                                GetDirApplicationXml()
 *       |   |- applicationName.conf               GetFileApplicationConfigure()
 *       |- translations                           GetDirTranslations()
 *       |- data                                   GetDirData()
 *       |   |- icons                              GetDirIcons()
 *       |   |- db                                 GetDirDatabase()
 *       |       |- database.db                    GetDirDatabaseFile()
 *       |- plugins
 *       |   |- translations                       GetDirPluginsTranslation()
 *       |   |- szDir
 *       |       | - translations                  GetDirPluginsTranslation("plugins/szDir")
 *       
 *    DocumentRoot/Rabbit/applicationName          (Write and read)
 *       |- root
 *       |    |- etc                               GetDirConfig(true)
 *       |    |   |- xml                           GetDirApplicationXml(true)
 *       |    |   |- applicationName.conf          GetFileApplicationConfigure(true)
 *       |    |- log                               GetDirLog(true)
 *       |    |- data                              GetDirData(true)
 *       |    |   |- db                            GetDirDatabase(true)
 *       |        |- database.db                   GetDirDatabaseFile(true)
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
    static CDir* Instance();
     
    QString GetDirApplication();
    int SetDirApplication(const QString &szPath);
    QString GetDirApplicationInstallRoot();
    int SetDirApplicationInstallRoot(const QString &szPath);
    QString GetDirConfig(bool bReadOnly = false);
    QString GetDirLog();
    QString GetDirData(bool bReadOnly = false);
    QString GetDirDatabase(bool bReadOnly = false);
    QString GetDirDatabaseFile(const QString &szFile = QString(), bool bReadOnly = false);
    QString GetDirApplicationXml(bool bReadOnly = false);
    QString GetDirIcons(bool bReadOnly = false);
    QString GetDirTranslations();
    QString GetDirPluginsTranslation(QString szDir = "plugins");
    QString GetDirPlugins(const QString &szDir = "");
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

private:
    CDir();

    QString m_szDocumentPath;
    QString m_szApplicationDir;
    QString m_szApplicationRootDir;

};

} //namespace RabbitCommon

#endif // RABBITCOMM_CGLOBALDIR_H
