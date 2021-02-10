#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindowPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slMouseUnderPlane(bool contains);
    void slRadioButtonsChanged(bool toggled);
    void slPbClip();
    void slChangeShader();

private:
    void itemsSelectionChanged();

private:
    Ui::MainWindow *ui;

    MainWindowPrivate * const d_ptr;
};
#endif // MAINWINDOW_H
