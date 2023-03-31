#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDateTime>

class DebugWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DebugWidget(QWidget *parent = nullptr);
    void print(const QString &str);

private:
    QPlainTextEdit *outputDbgText;
    QPlainTextEdit *inputDbgText;
    QPushButton *sendTextButton;

signals:

};

#endif // DEBUGWIDGET_H
