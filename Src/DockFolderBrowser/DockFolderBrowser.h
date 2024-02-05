#ifndef FOLDERBROWSER_H
#define FOLDERBROWSER_H

#include <QDockWidget>

#include "rabbitcommon_export.h"

namespace Ui {
class CDockFolderBrowser;
}

class QFileSystemModel;

/*!
 * \~english
 * \brief Folder browser. \n
 *        Usage: \snippet App/MainWindow.cpp Use CDockFolderBrowser
 * \~chinese
 * \brief 文件夹浏览器。 \n
 *        用法：\snippet App/MainWindow.cpp Use CDockFolderBrowser
 * \~
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CDockFolderBrowser : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit CDockFolderBrowser(const QString &title, QWidget *parent = nullptr,
                           Qt::WindowFlags flags = Qt::WindowFlags());
    explicit CDockFolderBrowser(QWidget *parent = nullptr,
                           Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~CDockFolderBrowser();

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
    Ui::CDockFolderBrowser *ui;
    
    QFileSystemModel *m_pModel;

    bool m_bDetails;
};

#endif // FOLDERBROWSER_H
