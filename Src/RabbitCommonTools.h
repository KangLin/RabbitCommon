/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Tools
 */

#ifndef RABBITCOMMON_TOOLS_H
#define RABBITCOMMON_TOOLS_H

#pragma once

#include <QTranslator>
#include "rabbitcommon_export.h"
#include <QLocale>

namespace RabbitCommon {

/**
 * @brief Tools
 * @ingroup API
 */
class RABBITCOMMON_EXPORT CTools
{
public:
    static CTools* Instance();
    
    int SetLanguage(const QString szLanguage);
    QString GetLanguage();
    /*! NOTE: 初始化资源，只在程序开始时调用一次，一般在 main() 函数的开始处
     * QApplication a(argc, argv);
     * a.setApplicationName(......);
     * 之后
     * 如果放到 QApplication a(argc, argv); 之前，
     * 则需要先调用 CDir::Instance()->SetDirApplication()
     * 和 CDir::Instance()->SetDirApplicationInstallRoot()
     */
    void Init(const QString szLanguage = QLocale::system().name());
    //! NOTE: 释放资源，只在程序结束前调用一次
    void Clean();
    void InitTranslator(const QString szLanguage = QLocale::system().name());
    void CleanTranslator();
    void InitResource();
    void CleanResource();
    
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
     * \param szName
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
    
    static void OpenLogConfigureFile();
    static void OpenLogFile();
    static void OpenLogFolder();
    
#ifdef HAVE_GUI
    static int RestoreWidget(QWidget* pWidget);
    static int SaveWidget(QWidget* pWidget);
#endif

private:
    CTools();
    virtual ~CTools();
    
    QTranslator m_Translator, m_TranslatorQt;
    
    QString m_szLanguage;
};

} //namespace RabbitCommon

#endif // RABBITCOMMON_TOOLS_H
