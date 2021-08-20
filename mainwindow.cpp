#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Plain text edit with font
    MainWindow::createTab();
    setCentralWidget(this->tabWidget);
    this->tabWidget->setTabsClosable(true);
    connect(this->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    this->tabWidget->setMovable(true);

    QFile file(":/tabs.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    this->tabWidget->setStyleSheet(styleSheet);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createTab()
{
    QVBoxLayout *layout = new QVBoxLayout(this->tabWidget->currentWidget());

    // Plain text edit with font
    QPlainTextEdit *fileEdit = new QPlainTextEdit();
    QFont font = fileEdit->document()->defaultFont();
    font.setFamily("Source Code Pro");
    fileEdit->setFont(font);
    fileEdit->setObjectName("fileEdit");
    QFile file(":/textedit.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    fileEdit->setStyleSheet(styleSheet);
//    fileEdit->setStyleSheet("border: 0;");

    QLabel *status = new QLabel(this);
    status->setText("No file opened.");
    status->setObjectName("status");

    QFrame *frame = new QFrame(this);

    layout->addWidget(fileEdit);
    layout->addWidget(status);

    frame->setLayout(layout);

    int tab = this->tabWidget->addTab(frame, "Untitled");
    this->tabWidget->setCurrentIndex(tab);

}

void MainWindow::on_actionNew_triggered()
{
    // Plain text edit with font
    MainWindow::createTab();
}

void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannots save file : " + file.errorString());
        return;
    }

    setWindowTitle(fileName);
    QTextStream out(&file);
    QList<QPlainTextEdit *> fileEdit = this->tabWidget->findChildren<QPlainTextEdit *>("fileEdit");

    for (int i=0; i<fileEdit.count(); ++i) {
        if (this->tabWidget->indexOf(fileEdit[i]->parentWidget()) == this->tabWidget->currentIndex()) {
            QString text = fileEdit[i]->toPlainText();
            out << text;

            setWindowTitle("Found!!!");
            break;
        }
    }

    file.close();
}

void MainWindow::on_actionCut_triggered()
{
    QPlainTextEdit *fileEdit = this->tabWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->cut();
}


void MainWindow::on_actionCopy_triggered()
{
    QPlainTextEdit *fileEdit = this->tabWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->copy();
}


void MainWindow::on_actionPaste_triggered()
{
    QPlainTextEdit *fileEdit = this->tabWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->paste();
}


void MainWindow::on_actionUndo_triggered()
{
    QPlainTextEdit *fileEdit = this->tabWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    QPlainTextEdit *fileEdit = this->tabWidget->findChild<QPlainTextEdit *>("fileEdit");
    fileEdit->redo();
}


void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionClose_file_triggered()
{
    if (this->tabWidget->count() == 1) {
        QApplication::quit();
    } else {
        this->tabWidget->removeTab(this->tabWidget->currentIndex());
    }
}


void MainWindow::on_actionOpen_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    QFileInfo fi(fileName);


    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());
    }


    QList<QLabel *> status = this->tabWidget->findChildren<QLabel *>("status");
    for (int i=0; i<status.count(); ++i) {
        if (this->tabWidget->indexOf(status[i]->parentWidget()) == this->tabWidget->currentIndex()) {
            status[i]->setText(fileName);
            break;
        }
    }

    this->tabWidget->setTabText(this->tabWidget->currentIndex(), fi.fileName());

    QTextStream in(&file);
    QString text = in.readAll();

    QTextStream out(stdout);

    QList<QPlainTextEdit *> fileEdit = this->tabWidget->findChildren<QPlainTextEdit *>("fileEdit");
    for (int i=0; i<fileEdit.count(); ++i) {
        if (this->tabWidget->indexOf(fileEdit[i]->parentWidget()) == this->tabWidget->currentIndex()) {
            fileEdit[i]->setPlainText(text);
            break;
        }
    }

    file.close();
}


void MainWindow::on_actionSave_triggered()
{
    QString fileName;
    QList<QLabel *> status = this->tabWidget->findChildren<QLabel *>("status");
    for (int i=0; i<status.count(); ++i) {
        if (this->tabWidget->indexOf(status[i]->parentWidget()) == this->tabWidget->currentIndex()) {
            fileName = status[i]->text();
            break;
        }
    }

    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannots save file : " + file.errorString());
        return;
    }

    setWindowTitle(fileName);
    QTextStream out(&file);
    QList<QPlainTextEdit *> fileEdit = this->tabWidget->findChildren<QPlainTextEdit *>("fileEdit");

    QString text;




    for (int i=0; i<fileEdit.count(); ++i) {
        if (this->tabWidget->indexOf(fileEdit[i]->parentWidget()) == this->tabWidget->currentIndex()) {
            text = fileEdit[i]->toPlainText();
            break;
        }
    }




    out << text;


    file.close();
}

void MainWindow::closeTab(int index)
{
    if (this->tabWidget->count() == 1) {
        QApplication::quit();
    } else {
        this->tabWidget->removeTab(index);
    }
}

void MainWindow::on_actionAbout_Fadyedit_triggered()
{
    QFile file(":/aboutbox.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    QMessageBox msgBox;
    msgBox.setText("<h3>About Fadyedit</h3>");
    msgBox.setInformativeText("Fadyedit version 0.1 created by Fady's Cube in 2021");
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);

    msgBox.setStyleSheet(styleSheet);
    msgBox.exec();
}
