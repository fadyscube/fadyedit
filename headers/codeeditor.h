#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>
#include <QRect>
#include <QPainter>
#include <QTextBlock>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPlainTextEdit>


class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
//    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};

#endif // CODEEDITOR_H
