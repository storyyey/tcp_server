#include "remarkdialog.h"
#include "mainwindow.h"

RemarkDialog::RemarkDialog()
{
    QString lineEditStyle = "QLineEdit {\
                              border: 2px solid #D4D4D4;\
                              border-radius: 4px;\
                              height: 30px;\
                             }\
                             QLineEdit:focus{\
                              border: 3px solid #63B8FF;\
                             }";

    this->setStyleSheet(lineEditStyle);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    this->setWindowTitle("备注");
    this->setLayout(mainLayout);

    snLineEdit = new QLineEdit();
    snLineEdit->setEnabled(false);
    mainLayout->addWidget(snLineEdit);

    remarkLineEdit = new QLineEdit();
    remarkLineEdit->setMinimumWidth(320);
    mainLayout->addWidget(remarkLineEdit);
    connect(remarkLineEdit, SIGNAL(textEdited(QString)), this, SLOT(remarkChangeSlot(QString)));
}

void RemarkDialog::popUp()
{
    windowConsole("弹出备注窗口");
    this->exec();
    windowConsole("关闭备注窗口");
}

void RemarkDialog::setDefaultRemark(const QString &text)
{
    remarkLineEdit->setText(text);
}

void RemarkDialog::setSn(const QString &sn)
{
    snLineEdit->setText(sn);
}

void RemarkDialog::remarkChangeSlot(const QString &text)
{
    emit remarkEdited(text);
}
