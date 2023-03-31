#ifndef ACTIONBAR_H
#define ACTIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QString>
#include <QtHttpServer/qhttpserver.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QPropertyAnimation>
#include <QNetworkInterface>
#include <QComboBox>
#include <QToolBox>
#include <QScrollArea>
#include <QListWidget>
#include <QLabel>
#include <QProcess>
#include <QCheckBox>
#include "dhcpserver.h"
#include "DeviceData.h"
#include "devicedb.h"
#include "setoption.h"

class ActionBar : public QWidget
{
    Q_OBJECT
public:
    explicit ActionBar(QWidget *parent = nullptr);
    ~ActionBar();
    void shortcutWidgetInit();
    void httpServerWidgetInit();
    void dhcpServerWidgetInit();
    void networkTestWidgetInit();
    void messageNotifyWidgetInit();
    void addMessageItem(const QString &text);
    void autoScript(Devices::devInfo &devInfo);
    void autoUpgrade(Devices::devInfo &devInfo);
    void autoIpconfig(Devices::devInfo &devInfo);
    void autoRemark(Devices::devInfo &devInfo);
    void addKeepLiveRouter();
    void addUpgradeRouter();
    void refreshComboBox(QComboBox *box);
    bool httpServerRun();
    bool httpServerStop();
    bool dhcpServerRun();
    bool dhcpServerStop();
    void setEnabled(QList<QWidget *> &widgets, bool flag);
private:
    QString runIcon = ":/icon/run.png";
    QString stopIcon = ":/icon/stop.png";
    QString anyAddress = "ANY";
    QString fileDownUrl = QString("http://%1/gateway/upgrade/");
    QTabWidget *toolBox;
    QVBoxLayout *mainLayout;
    QWidget *httpSrvWidget;
    QComboBox *ifipBox;
    QLineEdit *listenportEdit;
    QPushButton *httpRunBtn;
    QPushButton *httpStopBtn;
    QHttpServer *httpServer = NULL;
    QPropertyAnimation *mainAnimation;
    QWidget *testWidget;
    Devices *devices;
    QComboBox *httpInterfaces;
    QList<QNetworkInterface> interfaceList;

    DhcpServer *dhcpsrv;
    QWidget *dhcpSrvWidget;
    QPushButton *dhcpRunBtn;
    QPushButton *dhcpStopBtn;
    QComboBox *dhcpSrvIp;
    QLineEdit *dhcpStartIP;
    QLineEdit *dhcpEndIP;
    QLineEdit *dhcpNetMask;
    QLineEdit *dhcpGatewawy;
    QLineEdit *dhcpRouter;
    QLineEdit *dhcpBroadcast;

    int notifyRecordMax = 1000;
    QListWidget *notifyList;
    QWidget *notifyWidget;
    QPushButton *testButton;
    QWidget *msgLineWidget;
    QVBoxLayout *notifyLayout;
    QWidget *msgwidget;
    QLabel *msgLabel;
    QTimer *msgClearTimer;
    QPushButton *normalNotifyCnt;
    QPushButton *warnNotifyCnt;
    QPushButton *errorNotifyCnt;

    QList<QWidget *> shortCutWidgets;
    QCheckBox *autoUpgradeCehck;
    QLineEdit *autoUpgradeCurrVerEdit;
    QLineEdit *autoUpgradeTargetEdit;
    QLineEdit *autoRemarkEdit;

    void interfaceListInit();
    void dhcpAutoConfig();
    void deviceStatusChange(Devices::devInfo &devInfo);
private slots:
    void interfaceSelectChange(int index);
    void messagePopUp();
    void listWidgetItemClickd(QListWidgetItem *item);
    void messageAutoClear();
    void clearAllNotify();
    void dhcpServerIpChange(int index);
    void dhcpserverMaskChange(const QString &text);
signals:
};

#endif // ACTIONBAR_H
