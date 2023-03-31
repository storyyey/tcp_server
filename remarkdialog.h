#ifndef REMARKDIALOG_H
#define REMARKDIALOG_H

#include <QDialog>
#include <QLayout>
#include <QLineEdit>


class RemarkDialog : public QDialog
{
    Q_OBJECT
public:
    RemarkDialog();
    void popUp();
    void setDefaultRemark(const QString &text);
    void setSn(const QString &sn);
private:
    QLineEdit *remarkLineEdit;
    QLineEdit *snLineEdit;
    QString sn;
signals:
   void remarkEdited(const QString &val);

private slots:
    void remarkChangeSlot(const QString &text);
};

#endif // REMARKDIALOG_H
