/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Main window
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Download.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_actionAbout_triggered();
    void on_actionUpdate_triggered();
    void on_pushButton_clicked();
    void on_pbEncrypt_clicked();
    void on_pbGenerateCoreFile_clicked();
    void on_pbThreadCore_clicked();

    void on_actionFolder_browser_triggered();

    void on_pbAddFile_clicked();
    void on_pbDownload_clicked();
    void slotDownloadFile(const QString szFile);
    void slotDownloadError(int nErr, const QString szErr);

private:
    Ui::MainWindow *ui;
    
    RabbitCommon::CDownload* m_pDownload;
};

#endif // MAINWINDOW_H
