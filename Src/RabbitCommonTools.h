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
#include <QCoreApplication>
#include "rabbitcommon_export.h"
#include <QLocale>
#include <QSharedPointer>
#include <QObject>
#ifdef HAVE_RABBITCOMMON_GUI
    #include <QMenu>
    #include <QMainWindow>
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
class RABBITCOMMON_EXPORT CTools : QObject
{
    Q_OBJECT

public:
    static CTools* Instance();

    /*!
     * \~chinese
     * 初始化资源，只在程序开始时调用一次。
     * \note
     * - 在 main() 函数的开始处 QApplication a(argc, argv) 之后调用：
     * \code
     * int main(int argc, char* argv[]) {
     *     QApplication a(argc, argv);
     *     a.setApplicationName(......);
     *     RabbitCommon::CTools::Instance->Init();
     *     ......
     * }
     * \endcode
     * - 如果要放到 QApplication a(argc, argv) 之前调用，
     *   - 则在调用此函数之前需要先调用:
     *     - QCoreApplication::setApplicationName()
     *     - CDir::Instance()->SetDirApplication()
     *     - CDir::Instance()->SetDirApplicationInstallRoot()
     *   \code
     *   int main(int argc, char* argv[]) {
     *       QCoreApplication::setApplicationName(...);
     *       CDir::Instance()->SetDirApplication(...);
     *       CDir::Instance()->SetDirApplicationInstallRoot(...);
     *       RabbitCommon::CTools::Instance->Init();
     *       QApplication a(argc, argv);
     *       ......
     *   }
     *   \endcode
     *   - 带参数调用：
     *   \code
     *   int main(int argc, char* argv[]) {
     *       RabbitCommon::CTools::Instance->Init(szName, szPath, szInstallRoot);
     *       QApplication a(argc, argv);
     *       ......
     *   }
     *   \endcode
     *
     * \~english
     * Initialize the resource,
     * which is called only once at the beginning of the program,
     * \note
     * - Be called after QApplication a(argc, argv)
     *   at the beginning of the main() function
     *   \code
     *   int main(int argc, char* argv[]) {
     *       QApplication a(argc, argv);
     *       a.setApplicationName(......);
     *       RabbitCommon::CTools::Instance->Init();
     *       ......
     *   }
     *   \endcode
     * 
     * - If you are called before QApplication a(argc, argv)
     *   - you need to call:
     *     - QCoreApplication::setApplicationName()
     *     - CDir::Instance()->SetDirApplication()
     *     - CDir::Instance()->SetDirApplicationInstallRoot()
     *   \code
     *   int main(int argc, char* argv[]) {
     *       QCoreApplication::setApplicationName(...);
     *       CDir::Instance()->SetDirApplication(...);
     *       CDir::Instance()->SetDirApplicationInstallRoot(...);
     *       RabbitCommon::CTools::Instance->Init();
     *       QApplication a(argc, argv);
     *       ......
     *   }
     *   \endcode
     *  - Be called with the parameters
     *   \code
     *   int main(int argc, char* argv[]) {
     *       RabbitCommon::CTools::Instance->Init(szName, szPath, szInstallRoot);
     *       QApplication a(argc, argv);
     *       ......
     *   }
     *   \endcode
     *
     *\~
     *\see Init(int argc, char* argv[], QString szApplicationName)
     */
    void Init(QString szApplicationName = QCoreApplication::applicationName(),
              QString szApplicationDirPath = QCoreApplication::applicationDirPath(),
              QString szApplicationInstallRoot = QString(),
              const QString szLanguage = GetLanguage());
    /*!
     * \brief Init. It can called before QApplication a(argc, argv);
     *        It is parse the application name and path from command line parameters.
     * \param szApplicationName: Set if the application name is not same the program.
     * \see void Init()
     */
    void Init(int argc, char* argv[], QString szApplicationName = QString());

    /*! 
     * \~chinese
     * \note 释放资源，只在程序结束前调用一次
     * \~english
     * \note Free up the resource and call it only once before the program ends
     */
    void Clean();

    static int SetLanguage(const QString szLanguage);
    /*!
     * \~chinese
     * \brief 得到语言
     *   获取顺序：
     *     - 环境变量(LANG)
     *     - SetLanguage() 设置的值
     *     - QLocale::system().name()
     * \~english Get language
     *   Acquisition order:
     *     - Environment Variable (LANG)
     *     - The value set by SetLanguage().
     *     - QLocale::system().name()
     */
    [[nodiscard]] static QString GetLanguage();

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
        const QString szLanguage = GetLanguage());
    int RemoveTranslator(QSharedPointer<QTranslator> translator);

    //! RabbitCommon version
    [[nodiscard]] static QString Version();
    //! RabbitCommon information
    [[nodiscard]] static QString Information();

    struct VersionInfo {
        bool isValid = false;
        int major = 0;
        int minor = 0;
        int patch = 0;
        QString preRelease;
        QString build;
        // 新增辅助方法
        [[nodiscard]] QString toString() const {
            QString result = QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
            if (!preRelease.isEmpty())
                result += "-" + preRelease;
            if (!build.isEmpty())
                result += "+" + build;
            return result;
        }

        // 版本比较
        [[nodiscard]] bool isGreaterThan(const VersionInfo& other) const {
            if (major != other.major) return major > other.major;
            if (minor != other.minor) return minor > other.minor;
            if (patch != other.patch) return patch > other.patch;
            return false;  // 简化实现
        }
    };
    /*!
     * \~chinese
     * \brief 获取版本信息
     * \param szVersion 版本字符串，例如 "v1.2.3", "24.04", "1", "v1.2.3-alpha+build"
     * \param bSemantic 是否严格遵循 Semantic Versioning (需要 major.minor.patch)
     * \return VersionInfo 结构体，包含解析后的版本信息
     *
     * 简化模式（bSemantic=false）支持：
     *   - "24.04"     → major=24, minor=4
     *   - "1"         → major=1
     *   - "1.0"       → major=1, minor=0
     *   - "1.2.3"     → major=1, minor=2, patch=3
     *   - "v24.04"    → major=24, minor=4
     *
     * 严格模式（bSemantic=true）只支持完整的 major.minor.patch：
     *   - "1.2.3"     → major=1, minor=2, patch=3
     *   - "v1.2.3"    → major=1, minor=2, patch=3
     *   - "1"         → ❌ 无效
     *
     * \~english
     * \brief Get version information
     * \param szVersion: version string. eg: "v1.2.3", "24.04", "1", "v1.2.3-alpha+build"
     * \param bSemantic: Whether to strictly follow Semantic Versioning (need major.minor.patch)
     * \return VersionInfo: A struct that contains the parsed version information
     *
     * Simplified Mode（bSemantic=false）support：
     *   - "24.04"     → major=24, minor=4
     *   - "1"         → major=1
     *   - "1.0"       → major=1, minor=0
     *   - "1.2.3"     → major=1, minor=2, patch=3
     *   - "v24.04"    → major=24, minor=4
     *
     * Strictly Mode（bSemantic=true）Only supports the full version major.minor.patch：
     *   - "1.2.3"     → major=1, minor=2, patch=3
     *   - "v1.2.3"    → major=1, minor=2, patch=3
     *   - "1"         → ❌ Invalid
     *
     * \~
     * \see [Semantic Versioning](https://semver.org)
     * \see [Calendar Versioning](https://calver.org/)
     * \since 2.4.0
     */
    [[nodiscard]] static VersionInfo GetVersion(const QString& szVersion, bool bSemantic = false);
    //! \since 2.4.0
    [[nodiscard]] static bool VersionValid(const QString& szVersion);
    /*!
     * \brief Compare version
     * \return > 0: ver1 > ver2
     *         = 0: same
     *         < 0  ver1 < ver2
     * \see [Semantic Versioning](https://semver.org)
     * \~
     * \since 2.4.0
     */
    [[nodiscard]] static int VersionCompare(const QString &ver1, const QString &ver2);
    struct GitVersionInfo {
        QString baseVersion;
        QString commitCount;
        QString commitHash;
        bool isDevelopment;
        bool isValid;
    };
    /*!
     * \note For testing only
     * \~ince 2.4.0
     */
    [[nodiscard]] static GitVersionInfo GetGitDevelopmentVersion(const QString szVersion);
    /*!
     * \note For testing only
     * \~ince 2.4.0
     */
    [[nodiscard]] static bool IsGitDevelopmentVersion(const QString szVersion);

    /*!
     * \brief Enable core dump
     * \param bPrompt: prompt
     * \note It must be called first in the main function
     */
    Q_DECL_DEPRECATED_X("Setting in log configure file. Will be move to private in v3")
    static bool EnableCoreDump(bool bPrompt = true);

    /*!
     * \brief Has administrator privilege.
     * \since v2.3.0
     */
    [[nodiscard]] static bool HasAdministratorPrivilege();
    /*!
     * \brief executeByRoot: Run with administrator privileges
     * \param program
     * \param arguments
     */
    Q_DECL_DEPRECATED_X("Will be move to private in v3, with executeWithAdministratorPrivilege")
    static bool executeByRoot(const QString &program,
                              const QStringList &arguments = QStringList());
    /*!
     * \brief Run the program as an administrator
     * \param program: the program path
     * \param arguments: the parameter of the program
     * \param bDetached: true: detached; false: no detached
     * \return
     */
    static bool ExecuteWithAdministratorPrivilege(
        const QString &program,
        const QStringList &arguments = QStringList(),
        bool bDetached = true);
    /*!
     * \brief Start the self program with administrator privilege
     * \param bQuitOld: true, quit the old program
     * \since v2.3.3
     */
    static bool StartWithAdministratorPrivilege(bool bQuitOld = false);

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

    [[nodiscard]] static QString GetCurrentUser();
    [[nodiscard]] static QString GetHostName();

    [[nodiscard]] static QString MarkDownToHtml(const QString &szText);

    /*!
     * \brief GenerateDesktopFile: Generate desktop file
     * \param szPath: the path of desktop file.
     * \param szName: the name of desktop file.
     *            Default: "org.Rabbit." + qApp->applicationName() + ".desktop"
     */
    static int GenerateDesktopFile(const QString &szPath = QString(),
                                   const QString &szName = QString());

    /*!
     * \brief Invert color
     * \since 2.4.0
     */
    [[nodiscard]] static QColor InvertColor(const QColor& color);
    /*!
     * \brief Bytes convert to string
     * \param bytes
     * \since 2.4.0
     */
    [[nodiscard]] static QString BytesToString(quint64 bytes);

    /*!
     * \brief Get Random Number
     * \since 2.4.0
     */
    [[nodiscard]] static int GetRandomNumber(int min, int max);

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
     * \param parentMainWindow: QMainWindow pointer isn't nullptr, then add log QDockWidget
     * \return 
     */
    [[nodiscard]] static QMenu* GetLogMenu(QWidget *parentMainWindow = nullptr);

    static int RestoreWidget(QWidget* pWidget);
    static int SaveWidget(QWidget* pWidget);

    /*!
     * \brief Fit the platform to display the dialog or widget. Use macro RC_SHOW_WINDOW
     * \param pWin
     * \return if pWin is dialog, return exec()
     */
    int ShowWidget(QWidget *pWin);

    void ShowCoreDialog(QString szTitle, QString szContent,
                        QString szDetail, QString szCoreDumpFile);
    /*!
     * \~chinese
     * \brief 用系统文件浏览器打开文件所在目录，并选中文件
     * \param szFile： 要打开的文件
     * \~english Open the directory where the file is located using the system file browser and select the file.
     * \~
     * \since v2.3.4
     */
    static bool LocateFileWithExplorer(const QString szFile);
    [[nodiscard]] static QMainWindow* GetMainWindow();
#endif //#ifdef HAVE_RABBITCOMMON_GUI

    static int AndroidRequestPermission(const QStringList& permissions);
    static int AndroidRequestPermission(const QString& permission);

private:
    CTools();
    virtual ~CTools();

    void InitResource();
    void CleanResource();

    QSharedPointer<QTranslator> InstallTranslatorFile(const QString szFile);
    QVector<QSharedPointer<QTranslator> > m_Translator;

    bool m_bShowMaxWindow;

    /*!
     * \brief Compare PreRelease
     * \param ver1
     * \param ver2
     * \since 2.4.0
     */
    static int ComparePreRelease(const QString &ver1, const QString &ver2);
};

//! Fit the platform to display the dialog or widget.
#define RC_SHOW_WINDOW(win) RabbitCommon::CTools::Instance()->ShowWidget(win)

} //namespace RabbitCommon

#endif // RABBITCOMMON_TOOLS_H
