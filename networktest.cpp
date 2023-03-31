#include "networktest.h"
#include <QDesktopWidget>
#include <QNetworkInterface>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

NetworkTest::NetworkTest()
{
    QString thisStyle =\
                "QPushButton:hover {\
                  background-color: #BDBDBD;\
                  border-radius: 10px;\
                 }"\
                "QPushButton:pressed {\
                  background-color: #8F8F8F;\
                  border-radius: 10px;\
                 }";
    thisStyle += "QPushButton {\
                  background-color: transparent;\
                  height:30px;\
                 }";
    thisStyle += "QLineEdit {\
                  border: 1px solid #63B8FF;\
                  border-radius: 4px;\
                  height: 30px;\
                 }";
    thisStyle += "QComboBox {\
                  border: 1px solid #63B8FF;\
                  border-radius: 4px;\
                  height: 30px;\
                 }";
    thisStyle +="QDialog {\
                  background-color: #E0E0E0;\
                 }";
    thisStyle += "QPlainTextEdit {\
                  border: 2px solid #FFFFFF;\
                  border-radius: 8px;\
                 }";

    process = new QProcess(this);
    this->setStyleSheet(thisStyle);
    QDesktopWidget dw;
    this->resize(QSize(dw.width() / 4, (dw.height() / 3) * 2));
    QGridLayout *ndLayout = new QGridLayout();
    this->setLayout(ndLayout);

    QRegExp regExp("((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}");
    QValidator *validator = new QRegExpValidator(regExp, this);

    QLabel *interfaceLabel = new QLabel("网络接口:");
    ndLayout->addWidget(interfaceLabel, 0, 0);
    interfaceBox = new QComboBox();
    ndLayout->addWidget(interfaceBox, 0, 1);

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.count(); i++){
        QNetworkInterface var = interfaces[i];
        interfaceBox->addItem(var.humanReadableName());
    }
    interfaceBox->setCurrentText("以太网");

    QLabel *srcipLabel = new QLabel("本机地址:");
    ndLayout->addWidget(srcipLabel, 1, 0);
    localHostAddrEdit = new QLineEdit();
    localHostAddrEdit->setValidator(validator);
    ndLayout->addWidget(localHostAddrEdit, 1, 1);

    QLabel *dstipLabel = new QLabel("目标地址:");
    ndLayout->addWidget(dstipLabel, 2, 0);
    targetHostAddrEdit = new QLineEdit();
    targetHostAddrEdit->setValidator(validator);
    ndLayout->addWidget(targetHostAddrEdit, 2, 1);

    QLabel *maskLabel = new QLabel("地址掩码:");
    ndLayout->addWidget(maskLabel, 3, 0);
    networkMaskEdit = new QLineEdit();
    networkMaskEdit->setValidator(validator);
    ndLayout->addWidget(networkMaskEdit, 3, 1);

    testInfoDisplay = new QPlainTextEdit();
    ndLayout->addWidget(testInfoDisplay, 4, 0, 1, 2);
    connect(process, &QProcess::readyReadStandardOutput, this, [this] {
        testInfoDisplay->appendPlainText(\
                    QTextCodec::codecForName("GBK")->toUnicode(\
                        process->readAllStandardOutput()).toStdString().c_str());
    });

    QHBoxLayout *btnLayout = new QHBoxLayout();
    ndLayout->addLayout(btnLayout, 5, 0, 1, 2);
    QPushButton *startTestBtn = new QPushButton("开始测试");
    startTestBtn->setIcon(QIcon(":/icon/run.png"));
    btnLayout->addWidget(startTestBtn);
    QPushButton *closeTestBtn = new QPushButton("关闭测试");
    closeTestBtn->setIcon(QIcon(":/icon/stop.png"));
    btnLayout->addWidget(closeTestBtn);

    connect(startTestBtn, &QPushButton::clicked, this, [this] {
        QJsonObject addIpObject = QJsonObject {
            {"interface", interfaceBox->currentText()},
            {"address", localHostAddrEdit->text()},
            {"netmask", networkMaskEdit->text()}
        };
        QString addIpInfo = QString(QJsonDocument(addIpObject).toJson(QJsonDocument::Compact));;

        process->close();
        if (!hostAddrList.contains(addIpInfo)) {
            addAddress(interfaceBox->currentText(), localHostAddrEdit->text(), networkMaskEdit->text());
            hostAddrList << addIpInfo;
        }

        process->start(QString("ping -S %1 %2").arg(localHostAddrEdit->text(), targetHostAddrEdit->text()));
    });

    connect(closeTestBtn, &QPushButton::clicked, this, [this]{
        this->close();
    });
}

NetworkTest::~NetworkTest()
{
    qDebug() << "~NetworkTest()";
    closeTest();
}

void NetworkTest::setTargetAddr(const QString &addr)
{
    targetHostAddrEdit->setText(addr);

    if (!networkMaskEdit->text().size()) {
        return ;
    }

    QPair<QHostAddress, int> pair = \
            QHostAddress::parseSubnet(targetHostAddrEdit->text() + "/" + networkMaskEdit->text());
    localHostAddrEdit->setText(QHostAddress(pair.first.toIPv4Address() + 2).toString());
}

void NetworkTest::setTargetMask(const QString &mask)
{
    networkMaskEdit->setText(mask);

    if (!targetHostAddrEdit->text().size()) {
        return ;
    }

    QPair<QHostAddress, int> pair = \
            QHostAddress::parseSubnet(targetHostAddrEdit->text() + "/" + networkMaskEdit->text());
    localHostAddrEdit->setText(QHostAddress(pair.first.toIPv4Address() + 2).toString());
}

void NetworkTest::closeTest()
{
    QJsonParseError err;
    process->close();

    for (int index = 0; index < hostAddrList.size(); index++) {
        QString addIpInfo = hostAddrList.at(index);
        QJsonObject addIpObj = QJsonDocument::fromJson(addIpInfo.toUtf8(), &err).object();
        if (err.error) {
            continue;
        }

        QString delIpCmd = QString("netsh interface ip delete address name=\"%1\" addr=%2 ").\
                arg(addIpObj["interface"].toString(), addIpObj["address"].toString());

        qDebug() << delIpCmd;
        process->start(delIpCmd);
        process->waitForFinished();
    }
    hostAddrList.clear();
    testInfoDisplay->clear();
}

void NetworkTest::closeEvent(QCloseEvent *event)
{
    if (!event) {
        qWarning() << "close event is null";
    }
    closeTest();
}

bool NetworkTest::addAddress(const QString &interface, const QString &ip, const QString &netmask)
{
    QProcess process;

    QString addIpCmd = QString("netsh interface ip add address name=\"%1\" addr=%2 mask=%3").\
            arg(interface, ip, netmask);

    process.start(addIpCmd);
    process.waitForFinished();

    return true;
}

bool NetworkTest::delAddress(const QString &interface, const QString &ip)
{
    QProcess process;

    QString delIpCmd = QString("netsh interface ip delete address name=\"%1\" addr=%2 ").\
            arg(interface, ip);

    process.start(delIpCmd);
    process.waitForFinished();

    return true;
}
