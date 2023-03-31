#include "tables.h"
#include "mainwindow.h"

Tables::Tables(QWidget *parent) : QWidget(parent)
{
    widgetInit();
}

void Tables::widgetInit()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    tabWidget->setStyleSheet("QTabBar::tab:selected {\
                               width: 170px; \
                               height: 40px;\
                               color: #FFFFFF;\
                               background-color: #4D4D4D;\
                              } \
                              QTabBar::tab:!selected {\
                               width: 170px;\
                               height: 40px;\
                               color: #FFFFFF;\
                               background-color: #1F1F1F;\
                              }"\
                              "QTabWidget::pane {\
                                  border: 0px;\
                              }");
    mainLayout->addWidget(tabWidget);

    this->setLayout(mainLayout);
    devicesTab = new DevicesTable();
    tabWidget->addTab(devicesTab, "设备列表");
    tabWidget->tabBar()->setTabTextColor(tabWidget->count() - 1, QColor(255, 255, 255));
    tabWidget->setTabIcon(tabWidget->count() - 1, QIcon(":/icon/device.png"));

    urTable = new UpgradeRecordTable();
    tabWidget->addTab(urTable, "升级记录");
    tabWidget->tabBar()->setTabTextColor(tabWidget->count() - 1, QColor(255, 255, 255));
    tabWidget->setTabIcon(tabWidget->count() - 1, QIcon(":/icon/uptab.png"));

    icTable = new IpConfigRecordTable();
    tabWidget->addTab(icTable, "IP配置记录");
    tabWidget->tabBar()->setTabTextColor(tabWidget->count() - 1, QColor(255, 255, 255));
    tabWidget->setTabIcon(tabWidget->count() - 1, QIcon(":/icon/networkset.png"));

    connect(MainWindow::devices, SIGNAL(infoUpdate(Devices::devInfo &)),\
            devicesTab, SLOT(deviceInfoChange(Devices::devInfo &)));

    connect(MainWindow::devices, SIGNAL(newIpconfigRecord(QString &, QString &, QString &, QString &, QString &)),\
            icTable, SLOT(newIpconfigRecordSlot(QString &, QString &, QString &, QString &, QString &)));

    connect(MainWindow::devices, SIGNAL(newUpgradeRecord(QString &, Devices::deviceState, QString &, QString &, QString &, QString &)),\
            urTable, SLOT(newUpgradeRecordSlot(QString &, Devices::deviceState, QString &, QString &, QString &, QString &)));
}
