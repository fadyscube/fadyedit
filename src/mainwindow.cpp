#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

#include <headers/linenumberarea.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Fadyedit");

    treeView->setMaximumWidth(0);
    treeView->setMinimumWidth(0);

    QFile treeviewFile(":/styles/treeview.css");
    treeviewFile.open(QFile::ReadOnly);
    QString treeviewStyle = QLatin1String(treeviewFile.readAll());
    treeView->setStyleSheet(treeviewStyle);

    window->addWidget(treeView);
    window->addWidget(tabsWidget);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(openTreeViewFile(QModelIndex)));

    setCentralWidget(window);

    tabsWidget->setTabsClosable(true);
    connect(tabsWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    tabsWidget->setMovable(true);

    QFile tabsFile(":/styles/tabs.css");
    tabsFile.open(QFile::ReadOnly);
    QString tabsStyle = QLatin1String(tabsFile.readAll());
    tabsWidget->setStyleSheet(tabsStyle);

    QFile menubarFile(":/styles/menubar.css");
    menubarFile.open(QFile::ReadOnly);
    QString menubarStyle = QLatin1String(menubarFile.readAll());
    ui->menubar->setStyleSheet(menubarStyle);

    QFontDatabase::addApplicationFont(":/fonts/SourceCodePro-Regular.ttf");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createTab()
{
    QFrame *tabFrame = new QFrame(this);
    QVBoxLayout *tabsLayout = new QVBoxLayout(tabFrame);

    CodeEditor *fileEdit = new CodeEditor();
    QFont font = fileEdit->document()->defaultFont();
    font.setFamily("Source Code Pro");
    fileEdit->setFont(font);
    fileEdit->setObjectName("fileEdit");
    fileEdit->setTabStopDistance(QFontMetricsF(fileEdit->font()).horizontalAdvance(' ') * 4);

    QFile texteditFile(":/styles/textedit.css");
    texteditFile.open(QFile::ReadOnly);
    QString texteditStyle = QLatin1String(texteditFile.readAll());
    fileEdit->setStyleSheet(texteditStyle);

    QLabel *status = new QLabel(this);
    status->setText("Line 1, Column 1");
    status->setObjectName("status");

    QFile statusFile(":/styles/status.css");
    statusFile.open(QFile::ReadOnly);
    QString statusStyle = QLatin1String(statusFile.readAll());
    status->setStyleSheet(statusStyle);

    tabsLayout->addWidget(fileEdit);
    tabsLayout->addWidget(status);

//    tabFrame->setLayout(tabsLayout);

    int tab = tabsWidget->addTab(tabFrame, "Untitled");
    tabsWidget->setCurrentIndex(tab);

    tabsWidget->setTabToolTip(tab, "Untitled");

    connect(MainWindow::currentTextEdit(), SIGNAL(textChanged()), this, SLOT(textEditChanged()));
    connect(MainWindow::currentTextEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(updateStatus()));
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow::createTab();
}

void MainWindow::on_actionSave_as_triggered()
{
    if (tabsWidget->count() == 0) {
        QMessageBox::warning(this, "Warning", "Cannot save file !");
        return;
    }
    QString filePath = QFileDialog::getSaveFileName(this, "Save as ...");
    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());

        return;
    }

    setWindowTitle("Fadyedit | " + filePath);

    QTextStream out(&file);

    QString text = MainWindow::currentTextEdit()->toPlainText();
    out << text;

    file.close();

    MainWindow::currentTextEdit()->setPlainText(text);
    MainWindow::openTabFile(filePath);
}

void MainWindow::on_actionCut_triggered()
{
    MainWindow::currentTextEdit()->cut();
}


void MainWindow::on_actionCopy_triggered()
{
    MainWindow::currentTextEdit()->copy();
}


void MainWindow::on_actionPaste_triggered()
{
    MainWindow::currentTextEdit()->paste();
}


void MainWindow::on_actionUndo_triggered()
{
    MainWindow::currentTextEdit()->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    MainWindow::currentTextEdit()->redo();
}


void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionClose_file_triggered()
{
    MainWindow::closeTab(tabsWidget->currentIndex());
}


void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open the file");

    MainWindow::createTab();
    MainWindow::openTabFile(filePath);
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = tabsWidget->tabToolTip(tabsWidget->currentIndex());

    if (fileName == "Untitled") {
        MainWindow::on_actionSave_as_triggered();
        return;
    }
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());

        return;
    }

    QTextStream out(&file);

    QString text;

    text = MainWindow::currentTextEdit()->toPlainText();

    out << text;

    file.close();

    QString newTabText = tabsWidget->tabText(tabsWidget->currentIndex()).remove(0, 1);
    tabsWidget->setTabText(tabsWidget->currentIndex(), newTabText);
}

void MainWindow::closeTab(int index)
{
    if (tabsWidget->tabText(tabsWidget->currentIndex()).at(0) != "*"){
        tabsWidget->removeTab(index);
    } else {
        QMessageBox msgBox;
        msgBox.setText("<h4>Do you want to save ?</h4>");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::Yes:
                if (tabsWidget->tabToolTip(tabsWidget->currentIndex()) == "Untitled"){
                    MainWindow::on_actionSave_as_triggered();
                } else {
                    MainWindow::on_actionSave_triggered();
                }
                tabsWidget->removeTab(index);
                break;
            case QMessageBox::No:
                tabsWidget->removeTab(index);
                break;
        }
    }
}

void MainWindow::on_actionAbout_Fadyedit_triggered()
{
    QFile abouboxFile(":/styles/aboutbox.css");
    abouboxFile.open(QFile::ReadOnly);
    QString aboutboxStyle = QLatin1String(abouboxFile.readAll());

    QMessageBox msgBox;
    msgBox.setText("<h3>About Fadyedit</h3>");
    msgBox.setInformativeText("Fadyedit version 0.1 created by Fady's Cube in 2021");
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);

    msgBox.setStyleSheet(aboutboxStyle);
    msgBox.exec();
}

void MainWindow::on_actionOpen_folder_triggered()
{
    QUrl dirPath = QFileDialog::getExistingDirectory(this, "Open the file", "/", QFileDialog::ShowDirsOnly);

    dirModel->setRootPath(dirPath.toString());

    treeView->setModel(dirModel);
    treeView->setRootIndex(dirModel->index(dirPath.toString()));
    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    treeView->setMinimumWidth(width() * 20 / 100);
    treeView->setMaximumWidth(width() * 30 / 100);
}

void MainWindow::openTreeViewFile(QModelIndex index)
{
    MainWindow::createTab();

    QString filePath = dirModel->fileInfo(index).absoluteFilePath();

    MainWindow::openTabFile(filePath);
}

void MainWindow::openNewTabTreeViewFile(QModelIndex index)
{
    MainWindow::createTab();

    QString filePath = dirModel->fileInfo(index).absoluteFilePath();

    MainWindow::openTabFile(filePath);
}

void MainWindow::openTabFile(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileName(filePath);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());

        return;
    }

    tabsWidget->setTabToolTip(tabsWidget->currentIndex(), filePath);

    setWindowTitle("Fadyedit | " + filePath);

    QTextStream in(&file);
    QString text = in.readAll();

    QTextStream out(stdout);

    MainWindow::currentTextEdit()->setPlainText(text);

    file.close();

    tabsWidget->setTabText(tabsWidget->currentIndex(), fileName.fileName());
}

CodeEditor* MainWindow::currentTextEdit()
{
    QList<CodeEditor *> fileEditList = tabsWidget->findChildren<CodeEditor *>("fileEdit");
    for (int i=0; i<fileEditList.count(); ++i) {
        if (tabsWidget->indexOf(fileEditList[i]->parentWidget()) == tabsWidget->currentIndex()) {
            return fileEditList[i];
        }
    }

    return new CodeEditor;
}

QLabel* MainWindow::currentStatus()
{
    QList<QLabel *> statusList = tabsWidget->findChildren<QLabel *>("status");
    for (int i=0; i<statusList.count(); ++i) {
        if (tabsWidget->indexOf(statusList[i]->parentWidget()) == tabsWidget->currentIndex()) {
            return statusList[i];
        }
    }

    return new QLabel;
}

void MainWindow::textEditChanged ()
{
    QString tabName = tabsWidget->tabText(tabsWidget->currentIndex());

    if (tabName.at(0) != "*"){
        tabsWidget->setTabText(tabsWidget->currentIndex(), "*"+tabName);
    }
}

void MainWindow::updateStatus()
{
    QString line = QString::number(MainWindow::currentTextEdit()->textCursor().blockNumber() + 1);
    QString column = QString::number(MainWindow::currentTextEdit()->textCursor().columnNumber() + 1);

    QString status = "Line " + line + ", Column " + column;
    MainWindow::currentStatus()->setText(status);
}
