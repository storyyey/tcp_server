#include "devicestable.h"
#include "mainwindow.h"
#include <QClipboard>
#include <QApplication>

DevicesTable::DevicesTable(QWidget *parent) : QWidget(parent)
{
    QString thisStyle\
            = "QPushButton:hover {\
                  background-color: #BDBDBD;\
                  border-radius: 10px;\
                }"\
                "QPushButton:pressed {\
                   background-color: #8F8F8F;\
                   border-radius: 10px;\
                 }";
    thisStyle += "QPushButton {\
                   background-color: transparent;\
                   color: #FFFFFF;\
                   height:30px;\
                 }";
    thisStyle += ".QWidget {\
                  background-color: #FFFFFF;\
                 }";

    thisStyle += "QTableView {\
                  border: 3px solid #FFFFFF;\
                  border-radius: 6px;\
                 }";

    this->setStyleSheet(thisStyle);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    //deviceTabVBoxLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);

    QHBoxLayout *deviceTabHboxLayout = new QHBoxLayout();
    refreshBtn = new QPushButton("刷新");
    refreshBtn->setToolTip("刷新设备数据表");
    //refreshBtn->setStyleSheet(btnStyle);
    refreshBtn->setIcon(QIcon(":/icon/refresh.png"));
    refreshBtn->setIconSize(QSize(50, 24));
    refreshBtn->setFixedSize(120, 35);
    deviceTabHboxLayout->addWidget(refreshBtn);
    connect(refreshBtn, SIGNAL(clicked()), this, SLOT(tableRefresh()));

    exportBtn = new QPushButton("导出");
    exportBtn->setToolTip("数据表导出");
    exportBtn->setIcon(QIcon(":/icon/export.png"));
    exportBtn->setIconSize(QSize(50, 24));
    exportBtn->setFixedSize(120, 35);
    deviceTabHboxLayout->addWidget(exportBtn);
    connect(exportBtn, SIGNAL(clicked()), this, SLOT(tableExport()));

    importBtn = new QPushButton("历史记录");
    importBtn->setToolTip("导入历史数据");
    importBtn->setIcon(QIcon(":/icon/import.png"));
    importBtn->setIconSize(QSize(50, 24));
    importBtn->setFixedSize(120, 35);
    deviceTabHboxLayout->addWidget(importBtn);
    connect(importBtn, SIGNAL(clicked()), this, SLOT(historyImport()));

    exportProgress = new QProgressBar();
    deviceTabHboxLayout->addWidget(exportProgress);
    exportProgress->hide();
    deviceTabHboxLayout->addStretch();

    otherBtn = new QPushButton();
    otherBtn->setIcon(QIcon(":/icon/more.png"));
    otherBtn->setToolTip("更多功能");
    otherBtn->setFixedSize(60, 35);
    deviceTabHboxLayout->addWidget(otherBtn);
    connect(otherBtn, SIGNAL(clicked()), this, SLOT(otherFunc()));
    otherDialog = new OtherDialog();

    mainLayout->addLayout(deviceTabHboxLayout);

    tabModel = new QStandardItemModel();
    tabModel->setColumnCount(tabItemTitle.length());
    tabModel->setHorizontalHeaderLabels(tabItemTitle);
    tabView = new QTableView();
    tabView->setSortingEnabled(true);
    tabView->setModel(tabModel);
    //dDevTab->resizeColumnsToContents();
    tabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //dDevTab->verticalHeader()->hide();
    tabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabView->setAlternatingRowColors(true);
    //dDevTab->setSelectionMode(QAbstractItemView::SingleSelection);
    //dDevTab->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabView->setContextMenuPolicy(Qt::CustomContextMenu);
    tabView->setAutoScroll(true);

    operMenu = new DevicesTableMenu();
    operMenu->setTargetModel(tabModel);

    snInCloumn = findSnColumn();

    mainLayout->addWidget(tabView);

    connect(tabView, &QTableView::clicked, this, [](const QModelIndex &index) {
        if (index.isValid()) {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(index.data().toString());
        }
    });
    connect(tabView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuSlot(QPoint)));
    connect(operMenu, &DevicesTableMenu::newRemarkSignal, this, &DevicesTable::newRemarkSolt);
    connect(operMenu, &DevicesTableMenu::newIpConfigSignal, this, &DevicesTable::newIpConfigSolt);
    connect(operMenu, &DevicesTableMenu::newUpgradeSignal, this, &DevicesTable::newUpgradeSolt);

    QTimer *flickerTimer = new QTimer();
    flickerTimer->setInterval(10);
    flickerTimer->start();
    connect(flickerTimer, &QTimer::timeout, this, [this]{
        while (!flickerRun.isEmpty()) {
            if (flickerRun.head().statusLevel >= 1000) {
                flickerRun.dequeue();
                flicker.dequeue();
            }
            else {
                break;
            }
        }

        if (flickerRun.isEmpty()) return;

        for (int queueIndex = 0; queueIndex < flickerRun.size(); queueIndex++) {
            for (int ListIndex = 0; ListIndex < flickerRun[queueIndex].Item.size(); ListIndex++) {
                QStandardItem *item = flickerRun[queueIndex].Item.at(ListIndex);
                if (item) {
                    item->setBackground(QBrush((flickerRun[queueIndex].statusLevel / 10 + 155) << 8));
                }
            }
            flickerRun[queueIndex].statusLevel += 10;
        }
    });
}

static inline QString deviceStatusStr(const Devices::devInfo &data)
{
    if (data.state == Devices::upgradeInProgress) {
        return QString("[%1]").arg(Devices::UpgradeTimerValue(data.sn)) + \
                Devices::statusText(data.state);
    }
    else {
        return Devices::statusText(data.state);
    }
}

static inline QString deviceModelStr(const QString &model)
{
    if (model == "LWG-5000") {
        return "室内LORA网关";
    }
    else if (model == "LWG-3000") {
        return "室外LORA网关";
    }
    else {
        return "未知型号";
    }
}

void DevicesTable::addItem(const Devices::devInfo &data)
{
    QList<QStandardItem *> idItem = tabModel->findItems(data.sn, Qt::MatchFixedString, snInCloumn);
    QList<QStandardItem *> itemList;
    int changeRow = 0;

    if (idItem.size() == 0) {
        QStandardItem *StatusItem = new QStandardItem(deviceStatusStr(data));
        StatusItem->setToolTip(deviceStatusStr(data));
        itemList.append(StatusItem);
        QStandardItem *timeItem = new QStandardItem(data.time);
        timeItem->setToolTip(data.time);
        itemList.append(timeItem);
        QStandardItem *ipItem = new QStandardItem(data.ip);
        ipItem->setToolTip(data.ip);
        itemList.append(ipItem);
        QStandardItem *snItem = new QStandardItem(data.sn);
        snItem->setToolTip(data.sn);
        QFont snFont;
        snFont.setUnderline(true);
        snItem->setFont(snFont);
        snItem->setForeground(QBrush("#0000FF"));
        itemList.append(snItem);
        QStandardItem *versionItem = new QStandardItem(data.version);
        versionItem->setToolTip(data.version);
        itemList.append(versionItem);
        QStandardItem *modelItem = new QStandardItem(data.model);
        modelItem->setToolTip(deviceModelStr(data.model));
        itemList.append(modelItem);
        QStandardItem *remarkItem = new QStandardItem(data.remark);
        remarkItem->setToolTip(data.remark);
        itemList.append(remarkItem);
        tabModel->insertRow(0, itemList);
        changeRow = 0;
    }
    else {
        int row = 0;
        foreach(QStandardItem * entry, idItem) {
            row = entry->row();
            tabModel->item(row, 0)->setText(deviceStatusStr(data));
            tabModel->item(row, 0)->setToolTip(deviceStatusStr(data));

            tabModel->item(row, 1)->setText(data.time);
            tabModel->item(row, 1)->setToolTip(data.time);

            tabModel->item(row, 2)->setText(data.ip);
            tabModel->item(row, 2)->setToolTip(data.ip);

            tabModel->item(row, snInCloumn)->setText(data.sn);
            tabModel->item(row, snInCloumn)->setToolTip(data.sn);

            tabModel->item(row, 4)->setText(data.version);
            tabModel->item(row, 4)->setToolTip(data.version);

            tabModel->item(row, 5)->setText((data.model));
            tabModel->item(row, 5)->setToolTip(deviceModelStr(data.model));

            tabModel->item(row, 6)->setText(data.remark);
            tabModel->item(row, 6)->setToolTip(data.remark);

            for (int i = 0; i < tabModel->columnCount(); i++) {
                itemList.append(tabModel->item(row, i));
            }

            changeRow = row;
            break;
        }
    }
    setStateIcon(itemList.at(0), data.state);

 #if 1
    if (data.state == Devices::online &&\
        changeRow >=  tabView->rowAt(0) &&\
        changeRow <=  tabView->rowAt(0) + 60) {
        insertFlickerItem(itemList);
    }
    else {
        for (int n = 0; n < itemList.size(); n++) {
            QStandardItem *item = itemList.at(n);
            if (item) {
                if (!item->text().size()) {
                    item->setBackground(QBrush("#FFFF00"));
                }
                else {
                    item->setBackground(QBrush(Devices::stateColor(data.state)));
                }
            }
        }
    }
#endif

    windowConsole(QString("发现新设备当前列表设备总数%1").arg(tabModel->rowCount()));
}

int DevicesTable::findSnColumn()
{
    for (int i = 0; i < tabItemTitle.size(); i++) {
        if (tabItemTitle.at(i) == "序列号") {
            return i;
        }
    }
    return 0;
}


void DevicesTable::setStateIcon(QStandardItem *item, Devices::deviceState state)
{
    if (!item) return ;

    switch (state) {
        case Devices::upgradeInProgress:
        case Devices::upgradeWait:
            item->setIcon(QIcon(":/icon/sandclock.png"));
            break;
        case Devices::upgradeFinish:
        case Devices::upgradeSuccess:
        case Devices::online:
            item->setIcon(QIcon(":/icon/online.png"));
            break;
        case Devices::upgradeFailed:
            item->setIcon(QIcon(":/icon/cancel.png"));
            break;
        case Devices::offline:
        case Devices::deviceStatusEnd:
            item->setIcon(QIcon(":/icon/offline.png"));
            break;
        case Devices::waitReboot:
        case Devices::rebootInProgress:
            item->setIcon(QIcon(":/icon/reboot.png"));
            break;
    }
}

void DevicesTable::insertFlickerItem(QList<QStandardItem *> &item)
{
    if (!flicker.contains(item)) {
        flicker.append(item);
        flickerItem fitem;

        fitem.statusLevel = 0;
        fitem.Item = item;
        flickerRun.append(fitem);
    }
}

void DevicesTable::contextMenuSlot(QPoint pos)
{
    auto index = tabView->indexAt(pos);
    if (index.isValid()) {
        selectRow = tabView->rowAt(pos.y());
        selectColumn = tabView->columnAt(pos.x());
        // windowConsole(QString("当前鼠标右键选择第%1行%2列").arg(selectRow).arg(selectColumn));
        operMenu->setSelectPos(selectRow, selectColumn);
        operMenu->exec(QCursor::pos());
    }
}

void DevicesTable::newIpConfigSolt(Devices::ipconfigRecord &val)
{
   MainWindow::devices->setAddress(val);
}

void DevicesTable::newRemarkSolt(const QString &val)
{
    QStandardItem *item = tabModel->item(selectRow, snInCloumn);
    if (item) {
        MainWindow::devices->setRemark(item->text(), val);
    }
}

void DevicesTable::newUpgradeSolt(Devices::upgradeRecord &val)
{
    MainWindow::devices->setUpgrade(val);
}

void DevicesTable::deviceInfoChange(Devices::devInfo &data)
{
     addItem(data);
}

void DevicesTable::historyImport()
{
    windowConsole("导入历史数据开始");
    QList<Devices::devInfo> devinfos;

    DeviceDB::select(devinfos);
    for (int n = 0; n < devinfos.size(); n++) {
        addItem(devinfos.at(n));
    }
    windowConsole("导入历史数据结束");
}

void DevicesTable::tableExport()
{
    OperateXlsx oxlsx;
    windowConsole("开始设备列表");
    oxlsx.modelToXlsx(*tabModel, "设备列表", exportProgress);
    windowConsole("结束设备列表");
}

void DevicesTable::tableRefresh()
{
    windowConsole("刷新数据开始");
    for (int n = 0; n < tabModel->rowCount(); n++) {
        QStandardItem *item = tabModel->item(n, snInCloumn);
        if (item) {
            Devices::devInfo data;
            if (Devices::getInfo(item->text(), data)) {
                data.state = Devices::offline;
                MainWindow::devices->addInfo(item->text(), data);
            }
        }
    }

    windowConsole("刷新数据结束");
}

void DevicesTable::otherFunc()
{
    otherDialog->exec();
}
#if 0
void DevicesTable::selectDeviceInfoImport(Devices::devInfo &data)
{
    Devices::devInfo devinfo;

    if (Devices::getInfo(data.sn, devinfo)) {
        data.state = devinfo.state;
    }
    else {
        data.state = Devices::offline;
    }
    devices->addInfo(data.sn, data);
}
#endif
