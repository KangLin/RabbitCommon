/*!
 * \~english
 * \copyright Copyright (c) Kang Lin studio, All Rights Reserved
 * \author Kang Lin <kl222@126.com>
 * 
 * \~chinese
 * \copyright 康林工作室版权所有 (c)，保留所有权力。
 * \author 康　林 <kl222@126.com>
 */

#ifndef RABBITCOMMON_TOOLS_H
#define RABBITCOMMON_TOOLS_H

#pragma once

#include <QTranslator>
#include "rabbitcommon_export.h"
#include <QLocale>
#include <QSharedPointer>

#ifdef HAVE_RABBITCOMMON_GUI
    #include <QMenu>
#endif

/*!
 * \~english
 * \defgroup API Application interface
 * \defgroup INTERNAL_API Internal interface
 * 
 * \~chinese
 * \defgroup API 应用程序接口
 * \defgroup INTERNAL_API 内部使用接口
 */

namespace RabbitCommon {

/**
 * @brief Tools
 * @ingroup API
 */
class RABBITCOMMON_EXPORT CTools
{
public:
    static CTools* Instance();

    /*!
     * \~chinese
     * \note 初始化资源，只在程序开始时调用一次，一般在 main() 函数的开始处　QApplication a(argc, argv) 之后：
     * \code
     * int main(int argc, char* argv[]) {
     *     QApplication a(argc, argv);
     *     a.setApplicationName(......);
     *     RabbitCommon::CTools::Instance->Init();
     *     ......
     * }
     * \endcode
     * 如果放到 QApplication a(argc, argv) 之前，
     * 则需要先调用 CDir::Instance()->SetDirApplication()
     * 和 CDir::Instance()->SetDirApplicationInstallRoot()
     * 
     * \code
     * int main(int argc, char* argv[]) {
     *     CDir::Instance()->SetDirApplication();
     *     CDir::Instance()->SetDirApplicationInstallRoot();
     *     RabbitCommon::CTools::Instance->Init();
     *     QApplication a(argc, argv);
     *     a.setApplicationName(......);
     *     ......
     * }
     * \endcode
     * 
     * \~english
     * \note Initialize the resource,
     *       which is called only once at the beginning of the program,
     *       usually after QApplication a(argc, argv)
     *       at the beginning of the main() function
     * \code
     * int main(int argc, char* argv[]) {
     *     QApplication a(argc, argv);
     *     a.setApplicationName(......);
     *     RabbitCommon::CTools::Instance->Init();
     *     ......
     * }
     * \endcode
     * 
     * If you put it before QApplication a(argc, argv),
     * you need to call CDir::Instance()->SetDirApplication()
     * and CDir::Instance()->SetDirApplicationInstallRoot()
     * \code
     * int main(int argc, char* argv[]) {
     *     CDir::Instance()->SetDirApplication();
     *     CDir::Instance()->SetDirApplicationInstallRoot();
     *     RabbitCommon::CTools::Instance->Init();
     *     QApplication a(argc, argv);
     *     a.setApplicationName(......);
     *     ......
     * }
     * \endcode
     */
    void Init(const QString szLanguage = QLocale::system().name());
    /*! 
     * \~chinese
     * \note 释放资源，只在程序结束前调用一次
     * \~english
     * \note Free up the resource and call it only once before the program ends
     */
    void Clean();
    
    int SetLanguage(const QString szLanguage);
    QString GetLanguage();
    
    enum class TranslationType {
        Application,
        Library,
        Plugin
    };
    /*!
     * \brief Install translation
     * \param szName: translation name
     * \param szLanguage: language
     * \param type: TranslationType
     * \param szPluginDir: plugin directory
     * \return QSharedPointer<QTranslator>
     * \see CDir::GetDirTranslations cmake/Translations.cmake
     */
    QSharedPointer<QTranslator> InstallTranslator(
        const QString szName = QString(),
        TranslationType type = TranslationType::Application,
        const QString szPluginDir = "plugins",
        const QString szLanguage = QLocale::system().name());
    int RemoveTranslator(QSharedPointer<QTranslator> translator);

    //! RabbitCommon version
    static QString Version();
    //! RabbitCommon information
    static QString Information();

    /*!
     * \brief Enable core dump
     * \param bPrompt: prompt
     * \note It must be called first in the main function
     */
    static bool EnableCoreDump(bool bPrompt = true);

    /*!
     * \brief executeByRoot: Run with administrator privileges
     * \param program
     * \param arguments
     */
    static bool executeByRoot(const QString &program, const QStringList &arguments = QStringList());

    /*!
     * \brief InstallStartRun: auto run when startup
     * \param szName: The programe name
     *                - Windows: is programe name
     *                - Linux: the name of desktop file.
     * \param szPath
     * \param bAllUser:
     *          - true: all user
     *          - false: current user
     */
    static int InstallStartRun(const QString &szName = QString(),
                               const QString &szPath = QString(),
                               bool bAllUser = false);
    static int RemoveStartRun(const QString &szName = QString(),
                              bool bAllUser = false);
    static bool IsStartRun(const QString &szName = QString(),
                           bool bAllUser = false);

    static QString GetCurrentUser();
    static QString GetHostName();
    
    /*!
     * \brief GenerateDesktopFile: Generate desktop file
     * \param szPath: the path of desktop file.
     * \param szName: the name of desktop file.
     *            Default: "org.Rabbit." + qApp->applicationName() + ".desktop"
     */
    static int GenerateDesktopFile(const QString &szPath = QString(),
                                   const QString &szName = QString());
    
#ifdef HAVE_RABBITCOMMON_GUI
    /*!
     * \brief Add style menu after pMenu
     * \param pMenu
     * \param parent
     * \return 
     * 
     * \note
     * If you has your icon theme.
     * then call INSTALL_ICON_THEME() in CMakeLists.txt
     * \code
     * # in CMakeLists.txt
     * # Install QIcon theme
     * # param SOURCES: Default is ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons/
     * # param DESTINATION: Default is ${CMAKE_INSTALL_PREFIX}/data/icons
     * INSTALL_ICON_THEME()
     * \endcode
     * \see CFrmStyle
     */
    static QAction* AddStyleMenu(QMenu* pMenu, QWidget *parent = nullptr);
    /*!
     * \brief Insert style menu
     * \param pMenu
     * \param before
     * \param parent
     *
     * \note
     * If you has your icon theme.
     * then call INSTALL_ICON_THEME() in CMakeLists.txt
     * \code
     * # in CMakeLists.txt
     * # Install QIcon theme
     * # param SOURCES: Default is ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons/
     * # param DESTINATION: Default is ${CMAKE_INSTALL_PREFIX}/data/icons
     * INSTALL_ICON_THEME()
     * \endcode
     * \see CFrmStyle
     */
    static void InsertStyleMenu(QMenu* pMenu, QAction* before, QWidget *parent = nullptr);
    /*!
     * \brief Get log menu
     * \param parentMainWindow: QMainWindow pointer
     * \return 
     */
    static QMenu* GetLogMenu(QWidget *parentMainWindow = nullptr);

    static int RestoreWidget(QWidget* pWidget);
    static int SaveWidget(QWidget* pWidget);

    /*!
     * \brief Fit the platform to display the dialog or widget. Use macro RC_SHOW_WINDOW
     * \param pWin
     * \return if pWin is dialog, return exec()
     */
    int ShowWidget(QWidget *pWin);
#endif

    static int AndroidRequestPermission(const QStringList& permissions);
    static int AndroidRequestPermission(const QString& permission);

private:
    CTools();
    virtual ~CTools();

    void InitResource();
    void CleanResource();

    QSharedPointer<QTranslator> InstallTranslatorFile(const QString szFile);
    QVector<QSharedPointer<QTranslator> > m_Translator;

    QString m_szLanguage;
    bool m_Initialized;

    bool m_bShowMaxWindow;
};

//! Fit the platform to display the dialog or widget.
#define RC_SHOW_WINDOW(win) RabbitCommon::CTools::Instance()->ShowWidget(win)

} //namespace RabbitCommon

#endif // RABBITCOMMON_TOOLS_H
