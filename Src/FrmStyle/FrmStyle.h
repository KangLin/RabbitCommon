#ifndef CFRMSTYLE_H
#define CFRMSTYLE_H

#include "rabbitcommon_export.h"
#include <QWidget>

namespace Ui {
class CFrmStyle;
}

/*!
 * \~chinese 设置样式和图标主题窗体。
 * \details
 * - 如果你的工程有自己的图标主题。那么请在 CMakeLists 中调用 INSTALL_ICON_THEME() 安装。
 * \code
 * # in CMakeLists.txt
 *
 * # Install QIcon theme
 * # param SOURCES: Default is ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons/
 * # param DESTINATION: Default is ${CMAKE_INSTALL_PREFIX}/data/icons
 * INSTALL_ICON_THEME()
 * \endcode
 *
 * - 使用：
 *  - 初始化。在程序开始处加入下面代码：
 *  \code
 *  RabbitCommon::CTools::Instance()->Init();
 *  \endcode
 *
 *  - 设置样式。在程序需要打开设置样式对话框：
 *   \code
 *   CFrmStyle* s = new CFrmStyle();
 *   s->show();
 *   \endcode
 *
 * \~english
 * \brief Set style and icon theme form
 * \details
 *
 * Usge:
 * -Initialize. Add the following code at the beginning of the program:
 *  \code
 *  RabbitCommon::CTools::Instance()->Init();
 *  \endcode
 *
 * - Set the style. In the program you need to open the Set Style dialog box:
 *   \code
 *   CFrmStyle* s = new CFrmStyle();
 *   s->show();
 *   \endcode
 *
 * If you has your icon theme.
 * then call INSTALL_ICON_THEME() in CMakeLists.txt
 * \code
 * # in CMakeLists.txt
 * # Install QIcon theme
 * # param SOURCES: Default is ${CMAKE_CURRENT_SOURCE_DIR}/Resource/icons/
 * # param DESTINATION: Default is ${CMAKE_INSTALL_PREFIX}/data/icons
 * INSTALL_ICON_THEME()
 * \endcode
 *
 * \~
 * \see CStyle INSTALL_ICON_THEME()
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CFrmStyle : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmStyle(QWidget *parent = nullptr);
    ~CFrmStyle();
    
private Q_SLOTS:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    void on_pbBrower_clicked();
    void on_pbDefault_clicked();

private:
    Ui::CFrmStyle *ui;
};

#endif // CFRMSTYLE_H
