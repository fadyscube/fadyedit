#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
//#include <CodeEditor>
#include <QSplitter>
#include <QTreeView>

#include "headers/linenumberarea.h"
#include "headers/codeeditor.h"

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

    void on_actionOpen_folder_triggered();

    void openTreeViewFile(QModelIndex index);

    void openNewTabTreeViewFile(QModelIndex index);

    void openTabFile(QString filePath);

    CodeEditor* currentTextEdit();

    QLabel* currentStatus();

    void on_actionZoom_in_triggered();

    void on_actionZoom_out_triggered();

    void textEditChanged();

    void updateStatus();

private:
    Ui::MainWindow *ui;

    QSplitter *window = new QSplitter(this);
    QFileSystemModel *dirModel = new QFileSystemModel(this);
    QTreeView *treeView = new QTreeView;

    QTabWidget *tabsWidget = new QTabWidget(this);
};
#endif // MAINWINDOW_H
