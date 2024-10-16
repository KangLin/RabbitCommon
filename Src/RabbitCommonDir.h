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
 *       |- share                                  GetDirData()
 *       |   |- translations                       GetDirTranslations()
 *       |   |- doc                                GetDirDocument()
 *       |   |   |- ${ProjectName}                 GetDirDocument(QCoreApplication::applicationName())
 *       |   |- icons                              GetDirIcons()
 *       |   |- db                                 GetDirDatabase()
 *       |       |- database.db                    GetDirDatabaseFile()
 *       |- plugins                                GetDirPlugins()
 *       |   |- translations                       GetDirPluginsTranslation()
 *       |   |- szDir                              GetDirPlugins("plugins/szDir")
 *       |       | - translations                  GetDirPluginsTranslation("plugins/szDir")
 *
 *
 * The follow folder is write and read:
 * ${DocumentRoot}/Rabbit/${ApplicationName}       GetDirUserDocument()
 *       |- applicationName.conf                   GetFileUserConfigure()
 *       |- etc                                    GetDirUserConfig()
 *       |- share                                  GetDirUserData()
 *       |    |- image                             GetDirUserImage()
 *       |    |- db                                GetDirUserDatabase()
 *       |    |   |- database.db                   GetDirUserDatabaseFile()
 *       |    |- xml                               GetDirUserXml()
 *
 * System temp folder
 *       |- log
 *       |   |- ${ApplicationName}                 GetDirLog()
 *
 *
 * Android:
 *    assets (The folder is only read)             GetDirApplicationInstallRoot()
 *       |- etc                                    GetDirConfig()
 *       |   |- xml                                GetDirApplicationXml()
 *       |   |- applicationName.conf               GetFileApplicationConfigure()
 *       |- share                                  GetDirData()
 *       |   |- translations                       GetDirTranslations()
 *       |   |- icons                              GetDirIcons()
 *       |   |- db                                 GetDirDatabase()
 *       |       |- database.db                    GetDirDatabaseFile()
 *       |- plugins
 *       |   |- translations                       GetDirPluginsTranslation()
 *       |   |- szDir
 *       |       | - translations                  GetDirPluginsTranslation("plugins/szDir")
 *       
 *    ${DocumentRoot}/Rabbit/${applicationName}    (Write and read)
 *       |- root
 *       |    |- etc                               GetDirConfig(true)
 *       |    |   |- xml                           GetDirApplicationXml(true)
 *       |    |   |- applicationName.conf          GetFileApplicationConfigure(true)
 *       |    |- log                               GetDirLog()
 *       |    |- share                             GetDirData(true)
 *       |    |   |- db                            GetDirDatabase(true)
 *       |           |- database.db                GetDirDatabaseFile(true)
 *
 * \endcode
 *
 * \note In android, copy contents to DocumentRoot/Rabbit/applicationName from assets
 *
 * \see
 * - [GNU installation Directories in cmake](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html#module:GNUInstallDirs)
 * - [GNU installation Directories](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html)
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
    /*!
     * Get data directory
     * \param bReadOnly
     * \return data directory
     * \note the data directory is ${CMAKE_INSTALL_DATADIR} in CMakeLists.txt
     * \see
     *   - [GNU installation Directories in cmake](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html#module:GNUInstallDirs)
     *   - [GNU installation Directories](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html)
     */
    QString GetDirData(bool bReadOnly = false);
    /*!
     * \brief GetDirDocument
     * \param szProjectName: project name
     * \param bReadOnly
     * \return the document directory
     * \note the data directory is ${CMAKE_INSTALL_DOCDIR} in CMakeLists.txt
     * \see
     *   - [GNU installation Directories in cmake](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html#module:GNUInstallDirs)
     *   - [GNU installation Directories](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html)
     */
    QString GetDirDocument(QString szProjectName = QString(),
                           bool bReadOnly = false);
    QString GetDirDatabase(bool bReadOnly = false);
    QString GetDirDatabaseFile(const QString &szFile = QString(),
                               bool bReadOnly = false);
    QString GetDirApplicationXml(bool bReadOnly = false);
    QString GetDirIcons(bool bReadOnly = false);
    /*!
     * \see cmake/Translations.cmake
     * \see CTools::InstallTranslator
     */
    QString GetDirTranslations(QString szPrefix = QString());
    QString GetDirPluginsTranslation(QString szDir = "plugins");
    QString GetDirPlugins(const QString &szDir = "");
    QString GetFileApplicationConfigure(bool bReadOnly = false);

    QString GetDirUserDocument();
    /*!
     * \brief Set user document directory
     * \param szPath: If is empty, set system user document
     */
    int SetDirUserDocument(QString szPath = QString());
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
