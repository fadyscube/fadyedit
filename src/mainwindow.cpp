#include "headers/mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QModelIndex>
#include <QFontMetrics>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Fadyedit");

    this->treeView->setMaximumWidth(0);
    this->treeView->setMinimumWidth(0);

    QFile treeviewFile(":/styles/treeview.css");
    treeviewFile.open(QFile::ReadOnly);
    QString treeviewStyle = QLatin1String(treeviewFile.readAll());
    this->treeView->setStyleSheet(treeviewStyle);

    this->window->addWidget(this->treeView);
    this->window->addWidget(this->tabsWidget);

    this->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeCustomMenu(QPoint)));

    connect(this->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(openTreeViewFile(QModelIndex)));

    setCentralWidget(this->window);

    this->tabsWidget->setTabsClosable(true);
    connect(this->tabsWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    this->tabsWidget->setMovable(true);

    QFile tabsFile(":/styles/tabs.css");
    tabsFile.open(QFile::ReadOnly);
    QString tabsStyle = QLatin1String(tabsFile.readAll());
    this->tabsWidget->setStyleSheet(tabsStyle);

    QFile menubarFile(":/styles/menubar.css");
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
    QVBoxLayout *tabsLayout = new QVBoxLayout(tabFrame);

    QPlainTextEdit *fileEdit = new QPlainTextEdit();
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
    status->setText("Line 0, Column 0");
    status->setObjectName("status");

    QFile statusFile(":/styles/status.css");
    statusFile.open(QFile::ReadOnly);
    QString statusStyle = QLatin1String(statusFile.readAll());
    status->setStyleSheet(statusStyle);

    tabsLayout->addWidget(fileEdit);
    tabsLayout->addWidget(status);

//    tabFrame->setLayout(tabsLayout);

    int tab = this->tabsWidget->addTab(tabFrame, "Untitled");
    this->tabsWidget->setCurrentIndex(tab);

    this->tabsWidget->setTabToolTip(tab, "Untitled");

    connect(MainWindow::currentTextEdit(), SIGNAL(textChanged()), this, SLOT(textEditChanged()));
    connect(MainWindow::currentTextEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(updateStatus()));
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow::createTab();
}

void MainWindow::on_actionSave_as_triggered()
{
    if (this->tabsWidget->count() == 0) {
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
    MainWindow::closeTab(this->tabsWidget->currentIndex());
}


void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open the file");

    MainWindow::createTab();
    MainWindow::openTabFile(filePath);
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = this->tabsWidget->tabToolTip(this->tabsWidget->currentIndex());

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

    this->tabsWidget->setTabText(this->tabsWidget->currentIndex(), this->tabsWidget->tabText(this->tabsWidget->currentIndex()).remove(0, 1));
}

void MainWindow::closeTab(int index)
{
    if (this->tabsWidget->tabText(this->tabsWidget->currentIndex()).at(0) != "*"){
        this->tabsWidget->removeTab(index);
    } else {
        QMessageBox msgBox;
        msgBox.setText("<h4>Do you want to save ?</h4>");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::Yes:
                if (this->tabsWidget->tabToolTip(this->tabsWidget->currentIndex()) == "Untitled"){
                    MainWindow::on_actionSave_as_triggered();
                } else {
                    MainWindow::on_actionSave_triggered();
                }
                this->tabsWidget->removeTab(index);
                break;
            case QMessageBox::No:
                this->tabsWidget->removeTab(index);
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
    QUrl dirPath = QFileDialog::getExistingDirectory(this, "Open the file", "/home/fadyscube", QFileDialog::ShowDirsOnly);

    this->dirModel->setRootPath(dirPath.toString());

    this->treeView->setModel(this->dirModel);
    this->treeView->setRootIndex(this->dirModel->index(dirPath.toString()));
    this->treeView->hideColumn(1);
    this->treeView->hideColumn(2);
    this->treeView->hideColumn(3);

    this->treeView->setMinimumWidth(this->width() * 20 / 100);
    this->treeView->setMaximumWidth(this->width() * 30 / 100);
}

void MainWindow::openTreeViewFile(QModelIndex index)
{
    MainWindow::createTab();

    QString filePath = this->dirModel->fileInfo(index).absoluteFilePath();

    MainWindow::openTabFile(filePath);
}

void MainWindow::openNewTabTreeViewFile(QModelIndex index)
{
    MainWindow::createTab();

    QString filePath = this->dirModel->fileInfo(index).absoluteFilePath();

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

    this->tabsWidget->setTabToolTip(this->tabsWidget->currentIndex(), filePath);

    setWindowTitle("Fadyedit | " + filePath);

    QTextStream in(&file);
    QString text = in.readAll();

    QTextStream out(stdout);

    MainWindow::currentTextEdit()->setPlainText(text);

    file.close();

    this->tabsWidget->setTabText(this->tabsWidget->currentIndex(), fileName.fileName());
}

QPlainTextEdit* MainWindow::currentTextEdit()
{
    QList<QPlainTextEdit *> fileEditList = this->tabsWidget->findChildren<QPlainTextEdit *>("fileEdit");
    for (int i=0; i<fileEditList.count(); ++i) {
        if (this->tabsWidget->indexOf(fileEditList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            return fileEditList[i];
        }
    }

    return new QPlainTextEdit;
}

QLabel* MainWindow::currentStatus()
{
    QList<QLabel *> statusList = this->tabsWidget->findChildren<QLabel *>("status");
    for (int i=0; i<statusList.count(); ++i) {
        if (this->tabsWidget->indexOf(statusList[i]->parentWidget()) == this->tabsWidget->currentIndex()) {
            return statusList[i];
        }
    }

    return new QLabel;
}

void MainWindow::on_actionZoom_in_triggered()
{
    MainWindow::currentTextEdit()->zoomIn(2);
}

void MainWindow::on_actionZoom_out_triggered()
{
    MainWindow::currentTextEdit()->zoomOut(2);
}

void MainWindow::textEditChanged ()
{
    if (this->tabsWidget->tabText(this->tabsWidget->currentIndex()).at(0) != "*"){
        this->tabsWidget->setTabText(this->tabsWidget->currentIndex(), "*"+this->tabsWidget->tabText(this->tabsWidget->currentIndex()));
    }
}

void MainWindow::treeCustomMenu (const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QFile contextmenuFile(":/styles/menubar.css");
    contextmenuFile.open(QFile::ReadOnly);
    QString contextmenuStyle = QLatin1String(contextmenuFile.readAll());

    contextMenu.setStyleSheet(contextmenuStyle);

    QAction action1("Remove Data Point", this);
    contextMenu.addAction(&action1);

    QAction action2("Remove Data Point", this);
    contextMenu.addAction(&action2);

    QAction action3("Remove Data Point", this);
    contextMenu.addAction(&action3);

    QAction action4("Remove Data Point", this);
    contextMenu.addAction(&action4);

    contextMenu.exec(mapToGlobal(pos));
}

void MainWindow::updateStatus()
{
    QString line = QString::number(MainWindow::currentTextEdit()->textCursor().blockNumber());
    QString column = QString::number(MainWindow::currentTextEdit()->textCursor().columnNumber());

    QString status = "Line " + line + ", Column " + column;
    MainWindow::currentStatus()->setText(status);
}
