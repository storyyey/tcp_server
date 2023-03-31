#include "actionbar.h"
#include "mainwindow.h"
#include <QGroupBox>
#include <WinSock2.h>
#include <QApplication>

static inline QString host(const QHttpServerRequest &request)
{
    return request.headers()[QStringLiteral("Host")].toString();
}

ActionBar::ActionBar(QWidget *parent) : QWidget(parent)
{

    QString thisStyle\
            = "QPushButton:hover {\
                 background-color: #BDBDBD;\
                 border-radius: 8px;\
                }"\
                "QPushButton:pressed {\
                  background-color: #8F8F8F;\
                  border-radius: 8px;\
                 }";
    thisStyle += "QPushButton {\
                  background-color: transparent;\
                  height:30px;\
                 }";
    thisStyle += "QLineEdit {\
                  border: 2px solid #D4D4D4;\
                  border-radius: 4px;\
                  height: 30px;\
                  }\
                  QLineEdit:focus{\
                     border: 3px solid #63B8FF;\
                  }";
    thisStyle += "QComboBox {\
                   border: 2px solid #D4D4D4;\
                   border-radius: 4px;\
                   height: 30px;\
                 }\
                 QComboBox:focus{\
                    border: 3px solid #63B8FF;\
                 }";
    thisStyle += "QToolBoxButton {\
                  background-color: #FFFFFF;\
                  color: #000000;\
                  min-height: 40px;\
                  border: 0px solid #4D4D4D;\
                  text-align: center;\
                  border-radius: 8px;\
                  font-size: 16px;\
                  font-weight: bold;\
                 }"\
                 "QToolBox {\
                   border: 6px solid transparent;\
                   border-radius: 8px;\
                  }";
   thisStyle += "QPlainTextEdit {\
                  border: 2px solid #FFFFFF;\
                  border-radius: 4px;\
                 }";
   thisStyle += ".QWidget {\
                  background-color: #FFFFFF;\
                  border-radius: 8px;\
                 }";
   thisStyle += "QTabWidget::pane {\
                  border: 0px;\
                 }";

    this->setStyleSheet(thisStyle);
    mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);
    QWidget *toolBoxWidget = new QWidget();
    QVBoxLayout *toolBoxLayout = new QVBoxLayout();
    toolBoxWidget->setLayout(toolBoxLayout);
    toolBox = new QTabWidget();
    toolBoxLayout->addWidget(toolBox);
    mainLayout->addWidget(toolBoxWidget);
    shortcutWidgetInit();
    httpServerWidgetInit();
    dhcpServerWidgetInit();
    networkTestWidgetInit();
    messageNotifyWidgetInit();
}

ActionBar::~ActionBar()
{
    if (httpServer) {
        delete httpServer;
    }
}

void ActionBar::shortcutWidgetInit()
{
    static int shortcutWidgetIndex;
    shortcutWidgetIndex = toolBox->count();

    QWidget *shortcutWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    shortcutWidget->setLayout(mainLayout);

    QGroupBox *baseBox = new QGroupBox();
    baseBox->setTitle("基础设置");
    mainLayout->addWidget(baseBox);
    QVBoxLayout *baseLayout = new QVBoxLayout();
    baseBox->setLayout(baseLayout);
    QHBoxLayout *proLayout = new QHBoxLayout();
    baseLayout->addLayout(proLayout);
    QLabel *proLabel = new QLabel("项目类型:");
    proLayout->addWidget(proLabel);
    QComboBox *projectBox = new QComboBox();
    projectBox->setFixedWidth(250);
    shortCutWidgets.append(projectBox);
    projectBox->addItems(QStringList() << "幼儿园" << "其他");
    projectBox->setCurrentText(SetOption::value("ShortcutFunction", "projectType").toString());
    connect(projectBox, &QComboBox::currentTextChanged, this, [](const QString &text) {
        SetOption::setValue("ShortcutFunction", "projectType", text);
    });
    proLayout->addWidget(projectBox);
    proLayout->addStretch();

    QHBoxLayout *wanLayout = new QHBoxLayout();
    baseLayout->addLayout(wanLayout);
    QLabel *wanlabel = new QLabel("网络接口:");
    wanLayout->addWidget(wanlabel);
    QComboBox *interfaceBox = new QComboBox();
    interfaceBox->setFixedWidth(250);
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.count(); i++){
        QNetworkInterface var = interfaces[i];
        interfaceBox->addItem(var.humanReadableName());
    }
    interfaceBox->setCurrentText("以太网");
    shortCutWidgets.append(interfaceBox);
    interfaceBox->setCurrentText(SetOption::value("ShortcutFunction", "interface").toString());
    connect(interfaceBox, &QComboBox::currentTextChanged, this, [](const QString &text) {
        SetOption::setValue("ShortcutFunction", "interface", text);
    });
    wanLayout->addWidget(interfaceBox);
    wanLayout->addStretch();

    QGroupBox *autoUpgradeBox = new QGroupBox();
    autoUpgradeBox->setTitle("升级设置");
    mainLayout->addWidget(autoUpgradeBox);
    QGridLayout *autoUpgradeLayout = new QGridLayout();
    autoUpgradeBox->setLayout(autoUpgradeLayout);
    autoUpgradeCehck = new QCheckBox();
    autoUpgradeCehck->setText("自动升级");
    autoUpgradeCehck->setChecked(SetOption::value("ShortcutFunction", "autoUpgradeState").toBool());
    connect(autoUpgradeCehck, &QCheckBox::stateChanged, this, [](int state) {
        bool val = state == Qt::CheckState::Checked ? true : false;
        SetOption::setValue("ShortcutFunction", "autoUpgradeState", val);
    });
    //shortCutWidgets.append(autoUpgradeCehck);
    autoUpgradeLayout->addWidget(autoUpgradeCehck, 0, 0);
    QHBoxLayout *devTypeLayout = new QHBoxLayout();
    autoUpgradeLayout->addLayout(devTypeLayout, 1, 0);
    QLabel *devTypeLabel = new QLabel("网关类型:");
    devTypeLayout->addWidget(devTypeLabel);
    QComboBox *devTypeComBox = new QComboBox();
    devTypeComBox->setFixedWidth(250);
    shortCutWidgets.append(devTypeComBox);
    devTypeComBox->addItem("所有");
    devTypeComBox->setCurrentText("所有");
    devTypeComBox->addItems(Devices::devTypeList());
    devTypeComBox->setCurrentText(SetOption::value("ShortcutFunction", "deviceType").toString());
    connect(devTypeComBox, &QComboBox::currentTextChanged, this, [](const QString &text) {
        SetOption::setValue("ShortcutFunction", "deviceType", text);
    });
    devTypeLayout->addWidget(devTypeComBox);
    devTypeLayout->addStretch();

    QHBoxLayout *currVerLayout = new QHBoxLayout();
    autoUpgradeLayout->addLayout(currVerLayout, 2, 0);
    QLabel *currVerLabel = new QLabel("当前版本:");
    currVerLayout->addWidget(currVerLabel);
    autoUpgradeCurrVerEdit = new QLineEdit();
    autoUpgradeCurrVerEdit->setFixedWidth(250);
    shortCutWidgets.append(autoUpgradeCurrVerEdit);
    autoUpgradeCurrVerEdit->setText(SetOption::value("ShortcutFunction", "autoUpgradeCurrVer").toString());
    connect(autoUpgradeCurrVerEdit, &QLineEdit::textChanged, this, [](const QString &text) {
        SetOption::setValue("ShortcutFunction", "autoUpgradeCurrVer", text);
    });
    currVerLayout->addWidget(autoUpgradeCurrVerEdit);
    currVerLayout->addStretch();

    QHBoxLayout *targetVerLayout = new QHBoxLayout();
    autoUpgradeLayout->addLayout(targetVerLayout, 3, 0);
    QLabel *targetVerLabel = new QLabel("目标版本:");
    targetVerLayout->addWidget(targetVerLabel);
    autoUpgradeTargetEdit = new QLineEdit();
    autoUpgradeTargetEdit->setFixedWidth(250);
    shortCutWidgets.append(autoUpgradeTargetEdit);
    autoUpgradeTargetEdit->setText(SetOption::value("ShortcutFunction", "autoUpgradeTargetVer").toString());
    connect(autoUpgradeTargetEdit, &QLineEdit::textChanged, this, [](const QString &text) {
        SetOption::setValue("ShortcutFunction", "autoUpgradeTargetVer", text);
    });
    targetVerLayout->addWidget(autoUpgradeTargetEdit);
    targetVerLayout->addStretch();

    QGroupBox *autoIpConfigBox = new QGroupBox();
    autoIpConfigBox->setTitle("地址设置");
    mainLayout->addWidget(autoIpConfigBox);
    QGridLayout *autoIpConfigLayout = new QGridLayout();
    autoIpConfigBox->setLayout(autoIpConfigLayout);
    QCheckBox *autoIpConfigCehck = new QCheckBox();
    autoIpConfigCehck->setText("自动配置地址");
    shortCutWidgets.append(autoIpConfigCehck);
    autoIpConfigCehck->setChecked(SetOption::value("ShortcutFunction", "autoIpconfigState").toBool());
    connect(autoIpConfigCehck, &QCheckBox::stateChanged, this, [](int state) {
        bool val = state == Qt::CheckState::Checked ? true : false;
        SetOption::setValue("ShortcutFunction", "autoIpconfigState", val);
    });
    autoIpConfigLayout->addWidget(autoIpConfigCehck, 0, 0);
    QHBoxLayout *presetLayout = new QHBoxLayout();
    autoIpConfigLayout->addLayout(presetLayout, 1, 0);
    QLabel *presetIpLabel  = new QLabel("预设地址:");
    presetLayout->addWidget(presetIpLabel);
    QComboBox *presetIpComBox = new QComboBox();
    presetIpComBox->setFixedWidth(250);
    shortCutWidgets.append(presetIpComBox);
    presetLayout->addWidget(presetIpComBox);
    presetLayout->addStretch();

    QGroupBox *autoRemarkBox = new QGroupBox();
    autoRemarkBox->setTitle("备注设置");
    mainLayout->addWidget(autoRemarkBox);
    QHBoxLayout *remarkLayout = new QHBoxLayout();
    autoRemarkBox->setLayout(remarkLayout);
    QLabel *remarkLabel = new QLabel("备注信息:");
    remarkLayout->addWidget(remarkLabel);
    autoRemarkEdit = new QLineEdit();
    autoRemarkEdit->setFixedWidth(250);
    shortCutWidgets.append(autoRemarkEdit);
    remarkLayout->addWidget(autoRemarkEdit);
    remarkLayout->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    mainLayout->addLayout(btnLayout);
    QPushButton *startBtn = new QPushButton("启动");
    btnLayout->addWidget(startBtn);
    startBtn->setEnabled(true);
    startBtn->setIcon(QIcon(runIcon));
    QPushButton *stopBtn = new QPushButton("停止");
    btnLayout->addWidget(stopBtn);
    stopBtn->setEnabled(false);
    stopBtn->setIcon(QIcon(stopIcon));

    QTimer *delayTimer = new QTimer();

    connect(startBtn, &QPushButton::clicked, this, [this, projectBox, interfaceBox, startBtn, stopBtn, delayTimer] {
        QProcess process;
        QString cmdformat("netsh interface ip set address \"%1\" static %2 %3 %4");
        QString cmd;
        QString ip;
        QString port = 0;

        if (projectBox->currentText() == "幼儿园") {
            cmd = QString(cmdformat).arg(interfaceBox->currentText(),\
                                           "10.200.67.222", "255.255.255.0", "10.200.67.1");
            ip = "10.200.67.222";
            port = "4998";

        }
        else {
            cmd = QString(cmdformat).arg(interfaceBox->currentText(),\
                                            "14.29.249.101", "255.255.255.0", "14.29.249.1");
            ip = "14.29.249.101";
            port = "4998";
        }
        qDebug() << cmd;
        process.start(cmd);
        process.waitForFinished();

        delayTimer->setInterval(5000);
        delayTimer->start();
        connect(delayTimer, &QTimer::timeout, this, [this, port, ip, delayTimer](){
            refreshComboBox(ifipBox);
            ifipBox->addItem(anyAddress);
            ifipBox->setCurrentText(anyAddress);
            listenportEdit->setText(port);
            httpRunBtn->click();

            refreshComboBox(dhcpSrvIp);
            dhcpSrvIp->setCurrentText(ip);
            dhcpAutoConfig();
            dhcpRunBtn->click();
            delayTimer->stop();
        });

        startBtn->setEnabled(false);
        stopBtn->setEnabled(true);
        setEnabled(shortCutWidgets, false);
        toolBox->setTabIcon(shortcutWidgetIndex, QIcon(runIcon));
    });

    connect(stopBtn, &QPushButton::clicked, this, [this, startBtn, stopBtn] {
        httpStopBtn->click();
        dhcpStopBtn->click();
        startBtn->setEnabled(true);
        stopBtn->setEnabled(false);
        setEnabled(shortCutWidgets, true);
        toolBox->setTabIcon(shortcutWidgetIndex, QIcon(stopIcon));
    });

    mainLayout->addStretch();
    toolBox->addTab(shortcutWidget, "快捷功能");
    toolBox->setTabIcon(shortcutWidgetIndex, QIcon(stopIcon));
}

void ActionBar::httpServerWidgetInit()
{
    static int httpWidgetIndex;
    httpWidgetIndex = toolBox->count();

    QVBoxLayout *mvlayout = new QVBoxLayout();
    QGridLayout *mLayout = new QGridLayout();
    mvlayout->addLayout(mLayout);
    httpSrvWidget = new QWidget();
    httpSrvWidget->setLayout(mvlayout);
    toolBox->addTab(httpSrvWidget, "HTTP");
    toolBox->setTabIcon(httpWidgetIndex, QIcon(stopIcon));

    QLabel *interLabel = new QLabel("网口:");
    mLayout->addWidget(interLabel, 0, 0);
    httpInterfaces = new QComboBox();
    mLayout->addWidget(httpInterfaces, 0, 1);
    interfaceListInit();
    connect(httpInterfaces, SIGNAL(currentIndexChanged(int)), this, SLOT(interfaceSelectChange(int)));

    QLabel *ipLabel = new QLabel("IP:");
    mLayout->addWidget(ipLabel, 1, 0);
    ifipBox = new QComboBox();
    refreshComboBox(ifipBox);
    mLayout->addWidget(ifipBox, 1, 1);

    QLabel *portLabel = new QLabel("端口:");
    mLayout->addWidget(portLabel, 2, 0);
    listenportEdit = new QLineEdit();
    listenportEdit->setText("4998");
    mLayout->addWidget(listenportEdit, 2, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    mLayout->addLayout(btnLayout, 3, 0, 1, 2);

    httpRunBtn = new QPushButton();
    httpRunBtn->setText("启动");
    httpRunBtn->setIcon(QIcon(runIcon));
    httpRunBtn->setIconSize(QSize(50, 20));
    httpRunBtn->setDisabled(false);
    btnLayout->addWidget(httpRunBtn);

    httpStopBtn = new QPushButton();
    httpStopBtn->setText("停止");
    httpStopBtn->setIcon(QIcon(stopIcon));
    httpStopBtn->setIconSize(QSize(50, 20));
    httpStopBtn->setDisabled(true);
    btnLayout->addWidget(httpStopBtn);


    connect(httpRunBtn, &QPushButton::clicked, this, [this] {
        if (httpServerRun()) {
            toolBox->setTabIcon(httpWidgetIndex, QIcon(runIcon));
        }
    });

    connect(httpStopBtn, &QPushButton::clicked, this, [this] {
        if (httpServerStop()) {
            toolBox->setTabIcon(httpWidgetIndex, QIcon(stopIcon));
        }
    });
    mvlayout->addStretch();
}

void ActionBar::dhcpServerWidgetInit()
{
    static int dhcpWidgetIndex;
    dhcpWidgetIndex = toolBox->count();

    dhcpsrv = new DhcpServer();
    QVBoxLayout *mLayout = new QVBoxLayout();
    dhcpSrvWidget = new QWidget();
    dhcpSrvWidget->setLayout(mLayout);
    toolBox->addTab(dhcpSrvWidget, "DHCP");
    toolBox->setTabIcon(dhcpWidgetIndex, QIcon(stopIcon));

    QRegExp regExp("((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}");
    QValidator *validator = new QRegExpValidator(regExp, this);

    QGridLayout *ipconfigLayout = new QGridLayout();
    QLabel *srvipLabel = new QLabel("服务器IP:");
    ipconfigLayout->addWidget(srvipLabel, 0, 0);
    mLayout->addLayout(ipconfigLayout);
    dhcpSrvIp = new QComboBox();
    ipconfigLayout->addWidget(dhcpSrvIp, 0, 1);
    refreshComboBox(dhcpSrvIp);
    connect(dhcpSrvIp, SIGNAL(currentIndexChanged(int)), this, SLOT(dhcpServerIpChange(int)));

    QLabel *startIPlabel = new QLabel("起始IP:");
    ipconfigLayout->addWidget(startIPlabel, 1, 0);
    dhcpStartIP = new QLineEdit();
    dhcpStartIP->setValidator(validator);
    ipconfigLayout->addWidget(dhcpStartIP, 1, 1);

    QLabel *endIPlabel = new QLabel("结束IP:");
    ipconfigLayout->addWidget(endIPlabel, 2, 0);
    dhcpEndIP = new QLineEdit();
    dhcpEndIP->setValidator(validator);
    ipconfigLayout->addWidget(dhcpEndIP, 2, 1);

    QLabel *netmasklabel = new QLabel("子网掩码:");
    ipconfigLayout->addWidget(netmasklabel, 3, 0);
    dhcpNetMask = new QLineEdit();
    dhcpNetMask->setValidator(validator);
    ipconfigLayout->addWidget(dhcpNetMask, 3, 1);
    connect(dhcpNetMask, SIGNAL(textEdited(QString)), this, SLOT(dhcpserverMaskChange(QString)));

    QLabel *gatewaylabel = new QLabel("网关:");
    ipconfigLayout->addWidget(gatewaylabel, 4, 0);
    dhcpGatewawy = new QLineEdit();
    dhcpGatewawy->setValidator(validator);
    ipconfigLayout->addWidget(dhcpGatewawy, 4, 1);

    QLabel *routerlabel = new QLabel("路由:");
    ipconfigLayout->addWidget(routerlabel, 5, 0);
    dhcpRouter = new QLineEdit();
    dhcpRouter->setValidator(validator);
    ipconfigLayout->addWidget(dhcpRouter, 5, 1);

    QLabel *broadcastlabel = new QLabel("广播地址:");
    ipconfigLayout->addWidget(broadcastlabel, 6, 0);
    dhcpBroadcast = new QLineEdit();
    dhcpBroadcast->setValidator(validator);
    ipconfigLayout->addWidget(dhcpBroadcast, 6, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    mLayout->addLayout(btnLayout);
    dhcpRunBtn = new QPushButton();
    dhcpRunBtn->setText("启动");
    dhcpRunBtn->setIcon(QIcon(runIcon));
    dhcpRunBtn->setIconSize(QSize(50, 20));
    dhcpRunBtn->setDisabled(false);
    btnLayout->addWidget(dhcpRunBtn);

    dhcpStopBtn = new QPushButton();
    dhcpStopBtn->setText("停止");
    dhcpStopBtn->setIcon(QIcon(stopIcon));
    dhcpStopBtn->setIconSize(QSize(50, 20));
    dhcpStopBtn->setDisabled(true);
    btnLayout->addWidget(dhcpStopBtn);

    dhcpAutoConfig();
    connect(dhcpRunBtn, &QPushButton::clicked, this, [this] {
        if (dhcpServerRun()) {
            toolBox->setTabIcon(dhcpWidgetIndex, QIcon(runIcon));
        }
    });
    connect(dhcpStopBtn, &QPushButton::clicked, this, [this] {
        if (dhcpServerStop()) {
            toolBox->setTabIcon(dhcpWidgetIndex, QIcon(stopIcon));
        }
    });
    mLayout->addStretch();
}

void ActionBar::networkTestWidgetInit()
{
    static int networTestWidgetIndex;
    networTestWidgetIndex = toolBox->count();

    QProcess *process = new QProcess(this);
    QRegExp regExp("((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}");
    QValidator *validator = new QRegExpValidator(regExp, this);

    QGridLayout *mainLayout = new QGridLayout();
    QWidget *netTestWidget = new QWidget();
    netTestWidget->setLayout(mainLayout);
    toolBox->addTab(netTestWidget, "网络测试");
    toolBox->setTabIcon(networTestWidgetIndex, QIcon(stopIcon));

    QLabel *interfaceLabel = new QLabel("网络接口");
    mainLayout->addWidget(interfaceLabel, 0, 0);
    QComboBox *interfaceBox = new QComboBox();
    mainLayout->addWidget(interfaceBox, 0, 1);

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.count(); i++){
        QNetworkInterface var = interfaces[i];
        interfaceBox->addItem(var.humanReadableName());
    }
    interfaceBox->setCurrentText("以太网");

    QLabel *hostAddrLabel = new QLabel("本机地址:");
    mainLayout->addWidget(hostAddrLabel, 1, 0);
    QComboBox *hostAddrBox = new QComboBox();
    hostAddrBox->setEditable(true);
    QLineEdit *hostAddrEdit = hostAddrBox->lineEdit();
    hostAddrEdit->setValidator(validator);
    mainLayout->addWidget(hostAddrBox, 1, 1);

    QLabel *hostMaskLabel = new QLabel("本机掩码:");
    mainLayout->addWidget(hostMaskLabel, 2, 0);
    QLineEdit *hostMaskEdit = new QLineEdit();
    hostMaskEdit->setValidator(validator);
    mainLayout->addWidget(hostMaskEdit, 2, 1);

    QLabel *broadcastAddrLabel = new QLabel("网关地址:");
    mainLayout->addWidget(broadcastAddrLabel, 3, 0);
    QLineEdit *broadcastAddrEdit = new QLineEdit();
    broadcastAddrEdit->setValidator(validator);
    mainLayout->addWidget(broadcastAddrEdit, 3, 1);

    QLabel *targetAddrLabel = new QLabel("目标地址:");
    mainLayout->addWidget(targetAddrLabel, 4, 0);
    QComboBox *targetAddrBox = new QComboBox();
    targetAddrBox->setEditable(true);
    QLineEdit *targetAddrEdit = targetAddrBox->lineEdit();
    targetAddrEdit->setValidator(validator);
    mainLayout->addWidget(targetAddrBox, 4, 1);
    targetAddrBox->addItems(QStringList() << "10.200.67.222" << "14.29.249.101");
    targetAddrEdit->setText("10.200.67.222");

    QPlainTextEdit *testInfoDisplay = new QPlainTextEdit();
    mainLayout->addWidget(testInfoDisplay, 5, 0, 1, 2);
    connect(process, &QProcess::readyReadStandardOutput, this, [process, testInfoDisplay] {
        testInfoDisplay->appendPlainText(\
                    QTextCodec::codecForName("GBK")->toUnicode(\
                        process->readAllStandardOutput()).toStdString().c_str());
    });

    QHBoxLayout *btnLayout = new QHBoxLayout();
    mainLayout->addLayout(btnLayout, 6, 0, 1, 2);

    QPushButton *addAddressBtn = new QPushButton("添加地址");
    btnLayout->addWidget(addAddressBtn);
    addAddressBtn->setIcon(QIcon(":/icon/add.png"));
    connect(addAddressBtn, &QPushButton::clicked, this, [process, interfaceBox,\
            hostAddrEdit, hostMaskEdit, broadcastAddrEdit, testInfoDisplay] {
        QString addIpCmd = QString("netsh interface ip set address \"%1\" static %2 %3 %4").\
                arg(interfaceBox->currentText(), hostAddrEdit->text(), hostMaskEdit->text(), broadcastAddrEdit->text());

        qDebug() << addIpCmd;
        testInfoDisplay->clear();
        process->close();
        process->start(addIpCmd);
        process->waitForFinished();
    });

    QPushButton *startTestBtn = new QPushButton("开始测试");
    startTestBtn->setIcon(QIcon(runIcon));
    connect(startTestBtn, &QPushButton::clicked, this, [this, process,\
            hostAddrEdit, targetAddrEdit] {
        process->start(QString("ping -S %1 %2").arg(hostAddrEdit->text(), targetAddrEdit->text()));
        toolBox->setTabIcon(networTestWidgetIndex, QIcon(runIcon));
    });

    btnLayout->addWidget(startTestBtn);
    QPushButton *stopTestBtn = new QPushButton("停止测试");
    stopTestBtn->setIcon(QIcon(stopIcon));
    btnLayout->addWidget(stopTestBtn);
    connect(stopTestBtn, &QPushButton::clicked, this, [this, process] {
        process->close();
        toolBox->setTabIcon(networTestWidgetIndex, QIcon(stopIcon));
    });
}
void ActionBar::messageNotifyWidgetInit()
{
    notifyLayout = new QVBoxLayout();
    notifyLayout->setContentsMargins(0, 0, 0, 0);
    notifyWidget = new QWidget();
    mainLayout->addWidget(notifyWidget);
    notifyWidget->setStyleSheet(".QWidget {\
                                  border: 0px;\
                                  border-radius: 8px;\
                                  background-color: #EDEDED;\
                                }");
    notifyWidget->setLayout(notifyLayout);
    msgwidget = new QWidget();
    msgwidget->setStyleSheet(".QWidget {\
                               border: 0px;\
                               border-radius: 8px;\
                               background-color: #7FFFD4;\
                              }");
    QVBoxLayout *msgLayout = new QVBoxLayout();
    msgwidget->setLayout(msgLayout);
    notifyLayout->addWidget(msgwidget);
    msgLabel = new QLabel("message notify");
    msgLayout->addWidget(msgLabel);
    msgClearTimer = new QTimer();
    msgClearTimer->setInterval(15000);
    msgClearTimer->start();
    connect(msgClearTimer, SIGNAL(timeout()), this, SLOT(messageAutoClear()));

    notifyList = new QListWidget();
    notifyLayout->addWidget(notifyList);
    notifyList->setStyleSheet("QListWidget {\
                                border: 0px;\
                                background-color: #EDEDED;\
                               }\
                               QScrollBar {\
                                width: 0px;\
                                height: 0px;\
                               }"\
                               "QListWidget::item {\
                                 border-radius: 8px;\
                                 height: 40px;\
                                 border: 1px solid #FCFCFC;\
                                }");
    QString cntStyle = "QPushButton {\
                         background-color: transparent;\
                         border: 0px;\
                        }";
    QHBoxLayout *notifyTotalLayout = new QHBoxLayout();
    notifyLayout->addLayout(notifyTotalLayout);
    normalNotifyCnt = new QPushButton();
    normalNotifyCnt->hide();
    normalNotifyCnt->setIcon(QIcon(":/icon/notify.png"));
    normalNotifyCnt->setStyleSheet(cntStyle);
    notifyTotalLayout->addWidget(normalNotifyCnt);
    warnNotifyCnt = new QPushButton();
    notifyTotalLayout->addWidget(warnNotifyCnt);
    warnNotifyCnt->setStyleSheet(cntStyle);
    errorNotifyCnt = new QPushButton();
    notifyTotalLayout->addWidget(errorNotifyCnt);
    errorNotifyCnt->setStyleSheet(cntStyle);
    QPushButton *clearAllMsgBtn = new QPushButton();
    clearAllMsgBtn->setIcon(QIcon(":/icon/clear.png"));
    notifyTotalLayout->addWidget(clearAllMsgBtn);

    mainAnimation = new QPropertyAnimation();
    mainAnimation->setTargetObject(msgwidget);
    mainAnimation->setPropertyName("geometry");

    connect(clearAllMsgBtn, SIGNAL(clicked()), this, SLOT(clearAllNotify()));
    //connect(testButton, SIGNAL(clicked()), this, SLOT(messagePopUp()));
    connect(notifyList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(listWidgetItemClickd(QListWidgetItem *)));
}

void ActionBar::addMessageItem(const QString &text)
{
    if (msgClearTimer->isActive()) {
        msgClearTimer->setInterval(15000);
    }
    else {
        msgClearTimer->start();
    }
    QListWidgetItem  *item = new QListWidgetItem();

    item->setIcon(QIcon(":/icon/notify.png"));
    msgLabel->setText(text);
    msgLabel->setToolTip(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    notifyList->insertItem(0, item);
    notifyList->setItemWidget(item, new QLabel(text));
    item->setToolTip(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    if (notifyList->count() > notifyRecordMax) {
        QListWidgetItem *rmitem = notifyList->item(notifyRecordMax);
        notifyList->removeItemWidget(rmitem);
        rmitem = NULL;
    }
    normalNotifyCnt->show();
    normalNotifyCnt->setText(QString(" %1").arg(notifyList->count()));

    QPoint pos;
    pos = msgwidget->pos();
    QRect rect = msgwidget->contentsRect();

    if (pos.rx() || pos.ry()) {
        return ;
    }
    mainAnimation->setDuration(500);
    mainAnimation->setStartValue(QRect(pos.rx(), pos.ry() - rect.height(), rect.width(), rect.height()));
    mainAnimation->setEndValue(QRect(pos.rx(), pos.ry(), rect.width(), rect.height()));
    mainAnimation->start();
}

void ActionBar::autoScript(Devices::devInfo &devInfo)
{
    autoUpgrade(devInfo);
    autoIpconfig(devInfo);
    autoRemark(devInfo);
}

void ActionBar::autoUpgrade(Devices::devInfo &devInfo)
{
    if (!autoUpgradeCehck->isChecked()) {
        return ;
    }

    if (devInfo.version != autoUpgradeCurrVerEdit->text()) {
        return ;
    }

    for (int i = 0; DeviceDB::getFileStore().size(); i++) {
        const Devices::fileStore &file = DeviceDB::getFileStore().at(i);
        if (file.version == autoUpgradeTargetEdit->text() &&\
            file.model == devInfo.model) {
            windowConsole(devInfo.sn + "开始自动升级软件版本:" + file.version);
            Devices::upgradeRecord updata;

            updata.file = file.name;
            updata.md5 = file.md5;
            updata.remark = QString("升级文件为:" + file.name + "[" + file.md5 + "]");
            updata.sn = devInfo.sn;
            updata.model = devInfo.model;
            updata.old_ver = devInfo.version;
            updata.new_ver = file.version;
            updata.state = Devices::statusText(Devices::upgradeWait);
            updata.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            MainWindow::devices->setUpgrade(updata);
            break;
        }
    }
}

void ActionBar::autoIpconfig(Devices::devInfo &devInfo)
{

}

void ActionBar::autoRemark(Devices::devInfo &devInfo)
{
    if (!autoRemarkEdit->text().size()) {
        return ;
    }

    if (!devInfo.remark.size()) {
        MainWindow::devices->setRemark(devInfo.sn, autoRemarkEdit->text());
    }
}
void ActionBar::deviceStatusChange(Devices::devInfo &devInfo)
{
    switch (devInfo.state) {
        case Devices::upgradeInProgress: {
                uint32_t useTime = Devices::UpgradeTimerValue(devInfo.sn);
                if (useTime < upgradeThresholdFloor &&\
                    useTime < devInfo.interval) {
                    /* 升级过程中短时间内收到的数据忽略 */
                    break;
                }

                QString tv;
                Devices::getTargetVer(devInfo.sn, tv);
                if (tv != devInfo.version || \
                    useTime > upgradeThresholdUpper ||\
                    useTime < upgradeThresholdFloor) {
                    devInfo.state = Devices::upgradeFailed;
                    addMessageItem(QString("<font color = red><u>%1</u></font>升级至版本(%2)失败").arg(devInfo.sn, tv));
                }
                else {
                    devInfo.state = Devices::upgradeSuccess;
                    addMessageItem(QString("<font color = green><u>%1</u></font>升级至版本(%2)成功").arg(devInfo.sn, tv));
                }
                MainWindow::devices->addUpgradeRecord(devInfo.sn, devInfo.state, devInfo.version, tv, devInfo.remark);
            }
            break;
        case Devices::upgradeSuccess:
            devInfo.state = Devices::upgradeSuccess;
            break;
        case Devices::upgradeWait:
            /* devInfo.state = Devices::upgradeFailed; */
            break;
        case Devices::waitReboot:
            devInfo.state = Devices::rebootInProgress;
            break;
        case Devices::rebootInProgress:
        case Devices::upgradeFinish:
        case Devices::offline:
            devInfo.state = Devices::online;
            break;
        case Devices::online:
        case Devices::upgradeFailed:
        case Devices::deviceStatusEnd:
            break;
    }
    MainWindow::devices->setStatus(devInfo.sn, devInfo.state);
}

void ActionBar::refreshComboBox(QComboBox *box)
{
    int count = box->count();
    for (int n = 0; n < count; n++) {
        box->removeItem(0);
    }

    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for (int n = 0; n < ipList.count(); n++) {
        if (ipList[n].protocol() == QAbstractSocket::IPv4Protocol) {
            box->addItem(ipList[n].toString());
        }
    }
}
void ActionBar::addKeepLiveRouter()
{
    if (!httpServer)
        return ;

    httpServer->route("/gateway/upgrade", [this](const QHttpServerRequest &request) {
        windowConsole(request.body().data());
        windowConsole(QString("%1").arg(host(request)));

        QJsonParseError err;

        QJsonObject gateway = QJsonDocument::fromJson(request.body(), &err).object();
        if (err.error)
            return QString();

        if (gateway["command"].toString() == "keepalive") {
            Devices::devInfo devInfo;
            QJsonObject dataObj = gateway["data"].toObject();

            if (dataObj.isEmpty() == false) {
                quint32  addr_origin = request.remoteAddress().toIPv4Address();
                QHostAddress addr_host = QHostAddress(addr_origin);
                QDateTime currDate = QDateTime::currentDateTime();
                QString currDateStr = currDate.toString("yyyy-MM-dd hh:mm:ss");

                /* 解析并更新设备数据 */
                devInfo.state = Devices::online;
                devInfo.sn = dataObj["gateway_id"].toString();
                if (!Devices::getInfo(devInfo.sn, devInfo)) {
                    QApplication::alert(this);

                    Devices::devInfo dbdata;
                    DeviceDB::select(devInfo.sn, dbdata);
                    devInfo.remark = dbdata.remark;
                }
                devInfo.time = currDateStr;
                devInfo.ip = addr_host.toString();
                devInfo.version = dataObj["version"].toString();
                devInfo.model = dataObj["model"].toString();
                devInfo.interval = dataObj["interval"].toInt();
                deviceStatusChange(devInfo);

                /* 保存最新的设备数据 */
                MainWindow::devices->addInfo(devInfo.sn, devInfo);
                addMessageItem(QString("SN:<font color = #EE0000><u>%1</u></font>\
                                        IP:<font color = blue><u>%2</u></font>").arg(devInfo.sn, devInfo.ip));

                windowConsole(QString("网关ID %1 \n 版本 %2 型号 %3 \n")\
                              .arg(devInfo.sn)\
                              .arg(devInfo.version)\
                              .arg(devInfo.model));

                Devices::setFileDownUrl(fileDownUrl.arg(host(request)));
                /* */
                autoScript(devInfo);

                QString msg = Devices::popOperatorCmd(devInfo.sn);
                if (msg.length()) {
                    windowConsole("发送消息给设备" + QString(msg));
                    return msg;
                }
            }
        }

        return QString();
    });
}

void ActionBar::addUpgradeRouter()
{
    httpServer->route("/gateway/upgrade/file=<arg>", [this] (const QUrl &url, const QHttpServerRequest &request) {
        Devices::devInfo devInfo;
        QString tv;

        uint32_t cip = request.remoteAddress().toIPv4Address();
        MainWindow::devices->setStatus(cip, Devices::upgradeInProgress);
        Devices::getInfo(cip, devInfo);
        Devices::getTargetVer(devInfo.sn, tv);
        MainWindow::devices->addUpgradeRecord(devInfo.sn, devInfo.state, devInfo.version, tv, devInfo.remark);

        QByteArray data;
        if (!DeviceDB::getFile(url.path(), data)) {
            windowConsole(QString("未发现MD5为:%1的文件").arg(url.path()));
        }
        else {
            windowConsole(QString("开始发送文件MD5为:%1到服务器").arg(url.path()));
        }

        return data;
    });
}

void ActionBar::interfaceListInit()
{
    interfaceList = QNetworkInterface::allInterfaces();

    for (int i = 0; i < interfaceList.count(); i++){
        QNetworkInterface var = interfaceList[i];
        httpInterfaces->addItem(var.humanReadableName());
        httpInterfaces->setCurrentText("以太网");
    }
}

void ActionBar::dhcpAutoConfig()
{
    QHostAddress srvaddr(dhcpSrvIp->currentText());
    uint8_t netmasklen = 0;
    uint32_t broadcastip = 0;
    uint32_t subip = 0;
    uint32_t startip = 0;
    uint32_t endip = 0;

    if (dhcpNetMask->text().length() == 0) {
        dhcpNetMask->setText("255.255.255.0");
    }

    QPair<QHostAddress, int> pair = QHostAddress::parseSubnet(dhcpSrvIp->currentText() + "/" + dhcpNetMask->text());
    netmasklen = pair.second;
    subip = pair.first.toIPv4Address();
    startip = subip + 2;
    broadcastip = subip;
    for (int n = 0; n < 32 - netmasklen; n++) {
        broadcastip |= 0x01 << n;
    }
    endip = broadcastip - 1;
    dhcpStartIP->setText(QHostAddress(startip).toString());
    dhcpEndIP->setText(QHostAddress(endip).toString());
    dhcpGatewawy->setText(QHostAddress(subip + 1).toString());
    dhcpRouter->setText(srvaddr.toString());
    dhcpBroadcast->setText(QHostAddress(broadcastip).toString());
}

bool ActionBar::httpServerRun()
{
    if (httpServer)
        return false;

    httpServer = new QHttpServer();
    if (!httpServer)
        return false;

    windowConsole("等待启动http服务器");

    addKeepLiveRouter();
    addUpgradeRouter();

    QHostAddress listenip;
    if (ifipBox->currentText() == anyAddress) {
        listenip.setAddress(QHostAddress::Any);
    }
    else {
        listenip.setAddress(ifipBox->currentText());
    }
    uint16_t listenport = listenportEdit->text().toUShort();
    listenport = listenport ? listenport : (listenportEdit->setText("4998"), 4998);

    windowConsole(QString("启动服务器[%1:%2]").arg(listenip.toString()).arg(listenport));
    const auto port = httpServer->listen(listenip, listenport);
    if (!port) {
        windowConsole("启动http服务器失败");
        addMessageItem("<font color = red>HTTP服务启动失败</font>");
        delete httpServer;
        httpServer = NULL;
        return false;
    }
    else {
        windowConsole("启动http服务器成功");
        addMessageItem("<font color = green>HTTP服务启动成功</font>");
        httpRunBtn->setDisabled(true);
        httpStopBtn->setDisabled(false);
        httpInterfaces->setEnabled(false) ;
        ifipBox->setEnabled(false);
        listenportEdit->setDisabled(true);
        return true;
    }
}

bool ActionBar::httpServerStop()
{
    if (!httpServer)
        return false;

    delete httpServer;
    httpServer = NULL;

    httpRunBtn->setDisabled(false);
    httpStopBtn->setDisabled(true);
    httpInterfaces->setEnabled(true) ;
    ifipBox->setEnabled(true);
    listenportEdit->setDisabled(false);
    windowConsole("关闭http服务器成功");
    refreshComboBox(ifipBox);

    return true;
}

void ActionBar::interfaceSelectChange(int index)
{
    int count = 0;

    QNetworkInterface var = interfaceList[index];
    windowConsole("interface选择改变为" + var.humanReadableName());

    count = ifipBox->count();
    for (int n = 0; n < count; n++) {
        ifipBox->removeItem(0);
    }

    ifipBox->addItem(anyAddress);
    QList<QHostAddress> ipList = var.allAddresses();
    count = ipList.count();
    for (int n = 0; n < count; n++) {
        if (ipList[n].protocol() == QAbstractSocket::IPv4Protocol) {
            ifipBox->addItem(ipList[n].toString());
        }
    }
}

void ActionBar::messagePopUp()
{
    addMessageItem("message notify ");
}

void ActionBar::listWidgetItemClickd(QListWidgetItem *item)
{
    notifyList->removeItemWidget(item);
    delete item;
    normalNotifyCnt->setText(QString(" %1").arg(notifyList->count()));
    if (notifyList->count() <= 0) {
        normalNotifyCnt->hide();
    }
}

void ActionBar::messageAutoClear()
{
    msgClearTimer->stop();
    msgLabel->clear();
}

void ActionBar::clearAllNotify()
{
    normalNotifyCnt->hide();
    notifyList->clear();
}

bool ActionBar::dhcpServerRun()
{
    DhcpServer::dhcpcfg cfg;

    windowConsole("DHCP服务启动");

    cfg.srvip = QHostAddress(dhcpSrvIp->currentText()).toIPv4Address();
    cfg.startip = QHostAddress(dhcpStartIP->text()).toIPv4Address();
    cfg.endip = QHostAddress(dhcpEndIP->text()).toIPv4Address();
    cfg.netmask = QHostAddress(dhcpNetMask->text()).toIPv4Address();
    cfg.gateway = QHostAddress(dhcpGatewawy->text()).toIPv4Address();
    cfg.router = QHostAddress(dhcpRouter->text()).toIPv4Address();
    cfg.broadcast = QHostAddress(dhcpBroadcast->text()).toIPv4Address();

    if (dhcpsrv->start(cfg)) {
        dhcpRunBtn->setDisabled(true);
        dhcpStopBtn->setDisabled(false);
        dhcpSrvIp->setDisabled(true);
        dhcpStartIP->setDisabled(true);
        dhcpEndIP->setDisabled(true);
        dhcpNetMask->setDisabled(true);
        dhcpGatewawy->setDisabled(true);
        dhcpRouter->setDisabled(true);
        dhcpBroadcast->setDisabled(true);

        return true;
    }

    return false;
}

bool ActionBar::dhcpServerStop()
{
    windowConsole("DHCP服务停止");

    refreshComboBox(dhcpSrvIp);

    if (dhcpsrv->stop()) {
        dhcpRunBtn->setDisabled(false);
        dhcpStopBtn->setDisabled(true);
        dhcpSrvIp->setDisabled(false);
        dhcpStartIP->setDisabled(false);
        dhcpEndIP->setDisabled(false);
        dhcpNetMask->setDisabled(false);
        dhcpGatewawy->setDisabled(false);
        dhcpRouter->setDisabled(false);
        dhcpBroadcast->setDisabled(false);

        return true;
    }

    return false;
}

void ActionBar::setEnabled(QList<QWidget *> &widgets, bool flag)
{
    for (int i = 0; i < widgets.size(); ++i) {
        widgets.at(i)->setEnabled(flag);
    }
}

void ActionBar::dhcpServerIpChange(int index)
{
    windowConsole("当前选择服务器IP为" + dhcpSrvIp->currentText());
    dhcpAutoConfig();
}

void ActionBar::dhcpserverMaskChange(const QString &text)
{
    dhcpAutoConfig();
}
