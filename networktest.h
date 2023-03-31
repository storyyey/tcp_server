#ifndef NETWORKTEST_H
#define NETWORKTEST_H

#include <QDialog>
#include <QToolBox>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QLayout>
#include <QProcess>
#include <QLabel>
#include <QLineEdit>
#include <QTextCodec>
#include <QPushButton>

class NetworkTest : public QDialog
{
    Q_OBJECT
public:
    explicit NetworkTest();
    ~NetworkTest();
    void setTargetAddr(const QString &addr);
    void setTargetMask(const QString &mask);
    void closeTest();
    void closeEvent(QCloseEvent *event);
    static bool addAddress(const QString &interface, const QString &ip, const QString &netmask);
    static bool delAddress(const QString &interface, const QString &ip);
private:
    QProcess *process;
    QToolBox *testItemBox;
    QComboBox *interfaceBox;
    QLineEdit *localHostAddrEdit;
    QLineEdit *targetHostAddrEdit;
    QLineEdit *networkMaskEdit;
    QStringList hostAddrList;
    QPlainTextEdit *testInfoDisplay;
};

#endif // NETWORKTEST_H
