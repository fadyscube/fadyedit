#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QModelIndex>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Fadyedit");

    this->treeView->setMaximumWidth(0);
    this->treeView->setMinimumWidth(0);

    this->mainLayout->addWidget(this->treeView);
    this->mainLayout->addWidget(this->tabsWidget);

//    connect(this->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openNewTabTreeViewFile(QModelIndex)));
    connect(this->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(openTreeViewFile(QModelIndex)));

    this->window->setLayout(this->mainLayout);

    setCentralWidget(this->window);
    this->setLayout(this->mainLayout);

    MainWindow::createTab();
    this->tabsWidget->setTabsClosable(true);
    connect(this->tabsWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    this->tabsWidget->setMovable(true);

    QFile tabsFile(":/tabs.css");
    tabsFile.open(QFile::ReadOnly);
    QString tabsStyle = QLatin1String(tabsFile.readAll());
    this->tabsWidget->setStyleSheet(tabsStyle);

    QFile menubarFile(":/menubar.css");
    menubarFile.open(QFile::ReadOnly);
    QString menubarStyle = QLatin1String(menubarFile.readAll());
    ui->menubar->setStyleSheet(menubarStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createTab()
{

    QFrame *tabFrame = new QFrame(this);
    QVBoxLayout *tabsLayout = new QVBoxLayout(this->tabsWidget->currentWidget());

    QPlainTextEdit *fileEdit = new QPlainTextEdit();
    QFont font = fileEdit->document()->defaultFont();
    font.setFamily("Source Code Pro");
    fileEdit->setFont(font);
    fileEdit->setObjectName("fileEdit");

    QFile texteditFile(":/textedit.css");
    texteditFile.open(QFile::ReadOnly);
    QString texteditStyle = QLatin1String(texteditFile.readAll());
    fileEdit->setStyleSheet(texteditStyle);

    QLabel *status = new QLabel(this);
    status->setText("No file opened.");
    status->setObjectName("status");

    tabsLayout->addWidget(fileEdit);
    tabsLayout->addWidget(status);

    tabFrame->setLayout(tabsLayout);

    int tab = this->tabsWidget->addTab(tabFrame, "Untitled");
    this->tabsWidget->setCurrentIndex(tab);
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow::createTab();
}

void MainWindow::on_actionSave_as_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save as ...");
    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());

        return;
    }

    setWindowTitle("Fadyedit | " + filePath);
    QTextStream out(&file);
    QList<QPlainTextEdit *> fileEditList = this->tabsWidget->findChildren<QPlainTextEdit *>("fileEdit");

    for (int i=0; i<fileEditList.count(); ++i) {
        if (this->tabsWidget->indexOf(fileEditList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            QString text = fileEditList[i]->toPlainText();
            out << text;

            break;
        }
    }

    file.close();
}

void MainWindow::on_actionCut_triggered()
{
    QPlainTextEdit *fileEdit = this->tabsWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->cut();
}


void MainWindow::on_actionCopy_triggered()
{
    QPlainTextEdit *fileEdit = this->tabsWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->copy();
}


void MainWindow::on_actionPaste_triggered()
{
    QPlainTextEdit *fileEdit = this->tabsWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->paste();
}


void MainWindow::on_actionUndo_triggered()
{
    QPlainTextEdit *fileEdit = this->tabsWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    QPlainTextEdit *fileEdit = this->tabsWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->redo();
}


void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionClose_file_triggered()
{
//    if (this->tabsWidget->count() == 1) {
//        QApplication::quit();
//    } else {
    this->tabsWidget->removeTab(this->tabsWidget->currentIndex());
//    }
}


void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(filePath);
    QFileInfo fileName(filePath);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());

        return;
    }

    QList<QLabel *> statusList = this->tabsWidget->findChildren<QLabel *>("status");
    for (int i=0; i<statusList.count(); ++i) {
        if (this->tabsWidget->indexOf(statusList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            statusList[i]->setText(filePath);

            break;
        }
    }

    this->tabsWidget->setTabText(this->tabsWidget->currentIndex(), fileName.fileName());
    setWindowTitle("Fadyedit | " + filePath);

    QTextStream in(&file);
    QString text = in.readAll();

    QTextStream out(stdout);

    QList<QPlainTextEdit *> fileEditList = this->tabsWidget->findChildren<QPlainTextEdit *>("fileEdit");
    for (int i=0; i<fileEditList.count(); ++i) {
        if (this->tabsWidget->indexOf(fileEditList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            fileEditList[i]->setPlainText(text);

            break;
        }
    }

    file.close();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName;
    QList<QLabel *> statusList = this->tabsWidget->findChildren<QLabel *>("status");
    for (int i=0; i<statusList.count(); ++i) {
        if (this->tabsWidget->indexOf(statusList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            fileName = statusList[i]->text();

            break;
        }
    }

    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());

        return;
    }

    QTextStream out(&file);
    QList<QPlainTextEdit *> fileEditList = this->tabsWidget->findChildren<QPlainTextEdit *>("fileEdit");

    QString text;

    for (int i=0; i<fileEditList.count(); ++i) {
        if (this->tabsWidget->indexOf(fileEditList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            text = fileEditList[i]->toPlainText();

            break;
        }
    }

    out << text;

    file.close();
}

void MainWindow::closeTab(int index)
{
//    if (this->tabsWidget->count() == 1) {
//        QApplication::quit();
//    } else {
    this->tabsWidget->removeTab(index);
//    }
}

void MainWindow::on_actionAbout_Fadyedit_triggered()
{
    QFile abouboxFile(":/aboutbox.css");
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
    QUrl dirPath = QFileDialog::getExistingDirectory(this, "Open the file", "/home/fadyscube", QFileDialog::ShowDirsOnly);

    this->dirModel->setRootPath(dirPath.toString());

    this->treeView->setModel(this->dirModel);
    this->treeView->setRootIndex(this->dirModel->index(dirPath.toString()));
    this->treeView->hideColumn(1);
    this->treeView->hideColumn(2);
    this->treeView->hideColumn(3);

    this->treeView->setMaximumWidth(this->width() * 40 / 100);
    this->treeView->setMinimumWidth(this->width() * 20 / 100);
}

void MainWindow::openTreeViewFile(QModelIndex index)
{
    MainWindow::createTab();

    QString filePath = this->dirModel->fileInfo(index).absoluteFilePath();

    QFile file(filePath);
    QFileInfo fileName(filePath);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());

        return;
    }

    QList<QLabel *> statusList = this->tabsWidget->findChildren<QLabel *>("status");
    for (int i=0; i<statusList.count(); ++i) {
        if (this->tabsWidget->indexOf(statusList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            statusList[i]->setText(filePath);

            break;
        }
    }

    this->tabsWidget->setTabText(this->tabsWidget->currentIndex(), fileName.fileName());
    setWindowTitle("Fadyedit | " + filePath);

    QTextStream in(&file);
    QString text = in.readAll();

    QTextStream out(stdout);

    QList<QPlainTextEdit *> fileEditList = this->tabsWidget->findChildren<QPlainTextEdit *>("fileEdit");
    for (int i=0; i<fileEditList.count(); ++i) {
        if (this->tabsWidget->indexOf(fileEditList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            fileEditList[i]->setPlainText(text);

            break;
        }
    }

    file.close();
}

void MainWindow::openNewTabTreeViewFile(QModelIndex index)
{
    MainWindow::createTab();

    QString filePath = this->dirModel->fileInfo(index).absoluteFilePath();

    QFile file(filePath);
    QFileInfo fileName(filePath);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());

        return;
    }

    QList<QLabel *> statusList = this->tabsWidget->findChildren<QLabel *>("status");
    for (int i=0; i<statusList.count(); ++i) {
        if (this->tabsWidget->indexOf(statusList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            statusList[i]->setText(filePath);

            break;
        }
    }

    this->tabsWidget->setTabText(this->tabsWidget->currentIndex(), fileName.fileName());
    setWindowTitle("Fadyedit | " + filePath);

    QTextStream in(&file);
    QString text = in.readAll();

    QTextStream out(stdout);

    QList<QPlainTextEdit *> fileEditList = this->tabsWidget->findChildren<QPlainTextEdit *>("fileEdit");
    for (int i=0; i<fileEditList.count(); ++i) {
        if (this->tabsWidget->indexOf(fileEditList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            fileEditList[i]->setPlainText(text);

            break;
        }
    }

    file.close();
}
