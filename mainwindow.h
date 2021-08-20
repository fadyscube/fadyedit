#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createTab();

private slots:

    void on_actionNew_triggered();

    void on_actionSave_as_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionQuit_triggered();

    void on_actionClose_file_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void closeTab(int index);

    void on_actionAbout_Fadyedit_triggered();

private:
    Ui::MainWindow *ui;

    QTabWidget *tabWidget = new QTabWidget(this);
};
#endif // MAINWINDOW_H
