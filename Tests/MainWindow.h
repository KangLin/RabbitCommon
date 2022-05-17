/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Main window
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_actionStype_triggered();
    void on_pbEncrypt_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
