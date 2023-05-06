/*! \copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  \author Kang Lin <kl222@126.com>
 *  \abstract Title bar
 */

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include "rabbitcommon_export.h"

namespace RabbitCommon {

/*!
 * \~english
 * \brief The cursom title bar for QWidget, QDockWidget etc.
 *        - The default:
 *          - title label
 *          - minimized button
 *          - maximized button
 *          - float button
 *          - close button
 *        - Add custom buttons with AddWidgets.
 *        - Usage:
 *          - If the parent is QDockWidget. use QDockWidget::setTitleBarWidget settings
 *            - ag: \snippet Src/DockFolderBrowser/DockFolderBrowser.cpp Use CTitleBar
 *
 * \~chinese
 * \brief 自定义窗口标题栏。例如： QWidget, QDockWidget 等
 *        - 默认有:
 *          - 标题栏
 *          - 最小化按钮
 *          - 最大化按钮
 *          - 浮动按钮
 *          - 关闭按钮
 *        - 用 AddWidgets 增加自定义按钮。
 *        - 用法：
 *          - 如果要设置的父窗口是 QDockWidget，调用 QDockWidget::setTitleBarWidget 设置
 *            - 例子：\snippet Src/DockFolderBrowser/DockFolderBrowser.cpp Use CTitleBar
 *
 * \~
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CTitleBar(QWidget *parent);

    /* The default button */
    int VisibleTitleButton(bool bVisible);
    int VisibleMaximizeButton(bool bVisible);
    int VisibleMinimizeButton(bool bVisible);
    int VisibleFloatButton(bool bVisible);
    int VisibleCloseButton(bool bVisible);

    //! Add user-defined buttons
    int AddWidgets(QList<QWidget*> pLstWidget);

    static QPushButton* CreateSmallPushButton(QIcon icon, QWidget *parent);

private Q_SLOTS:
    void slotMinimize();
    void slotMaximize();
    void slotFloat();
    void slotTopLevelChanged(bool topLevel);

private:
    QLabel *m_pTitle;
    QPushButton *m_pCloseButton, *m_pFloatButton, *m_pMaxButton, *m_pMinButton;
    QList<QWidget*> m_pLstWdiget;    
};

} // namespace RabbitCommon

#endif // TITLEBAR_H
