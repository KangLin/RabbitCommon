#ifndef FOLDERBROWSER_H
#define FOLDERBROWSER_H

#include <QDockWidget>

#include "rabbitcommon_export.h"

namespace Ui {
class CFolderBrowser;
}

class QFileSystemModel;

/*!
 * \~english
 * \brief Folder browser. \n
 *        Usage: \snippet Tests/MainWindow.cpp Use CFolderBrowser
 * \~chinese
 * \brief 文件夹浏览器。 \n
 *        用法：\snippet Tests/MainWindow.cpp Use CFolderBrowser
 * \~
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CFolderBrowser : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit CFolderBrowser(const QString &title, QWidget *parent = nullptr,
                           Qt::WindowFlags flags = Qt::WindowFlags());
    explicit CFolderBrowser(QWidget *parent = nullptr,
                           Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~CFolderBrowser();

    //! Set root path
    void setRootPath(const QString dir);
    //! Get root path
    QString rootPath() const;

Q_SIGNALS:
    //! emit when Double clicked
    //! \param szItem: item of double clicked
    //! \param bDir:
    //!          - true: item is directory
    //!          - false: item is folder
    void sigDoubleClicked(const QString &szItem, bool bDir);

private:
    Ui::CFolderBrowser *ui;
    
    QFileSystemModel *m_pModel;

    bool m_bDetails;
};

#endif // FOLDERBROWSER_H
