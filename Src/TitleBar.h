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
 * 
 * \~chinese
 * \brief 自定义窗口标题栏
 */
class RABBITCOMMON_EXPORT CTitleBar : public QWidget
{
    Q_OBJECT
    
public:
    /*!
     * \param pLstWidget: Add the list of QWidget* to the title bar
     * \param bAppend:
     *           - true: Keep the default QWidgets.
     *           - false: Disable the default QWidgets.
     * \param parent
     */
    explicit CTitleBar(QWidget *parent);

    /* The default button */
    int SetTitle(const QString& szTitle);
    int VisibleTitleButton(bool bVisible);
    int VisibleMaximizeButton(bool bVisible);
    int VisibleMinimizeButton(bool bVisible);
    int VisibleFloatButton(bool bVisible);
    int VisibleCloseButton(bool bVisible);
    
    //! Set up user-defined buttons
    int SetWidgets(QList<QWidget*> pLstWidget);
    
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
