#include "debugwidget.h"

DebugWidget::DebugWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *mLayout = new QHBoxLayout();

    mLayout->setContentsMargins(5, 5, 5, 5);

    outputDbgText = new QPlainTextEdit(this);
    outputDbgText->setMaximumBlockCount(6000);
    mLayout->addWidget(outputDbgText);
    inputDbgText = new QPlainTextEdit();
    mLayout->addWidget(inputDbgText);
    sendTextButton = new QPushButton(this);
    sendTextButton->setText("发送命令");
    mLayout->addWidget(sendTextButton);

    this->setLayout(mLayout);

   // connect(sendTextButton, SIGNAL(clicked()), this, SLOT(sendTextSlot()));
}

void DebugWidget::print(const QString &str)
{
    QDateTime currDate = QDateTime::currentDateTime();
    QString currDateStr = currDate.toString("yyyy/MM/dd hh:mm:ss");

    outputDbgText->appendPlainText(currDateStr + "=>" + str);
}
