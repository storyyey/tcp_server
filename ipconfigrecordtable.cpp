#include "ipconfigrecordtable.h"
#include "mainwindow.h"

IpConfigRecordTable::IpConfigRecordTable(QWidget *parent) : QWidget(parent)
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
    this->setLayout(mainLayout);

    QHBoxLayout *toolBarLayout = new QHBoxLayout();

    exportBtn = new QPushButton("导出");
    exportBtn->setToolTip("数据表导出");
    exportBtn->setIcon(QIcon(":/icon/export.png"));
    exportBtn->setIconSize(QSize(50, 24));
    exportBtn->setFixedSize(120, 35);
    toolBarLayout->addWidget(exportBtn);
    connect(exportBtn, SIGNAL(clicked()), this, SLOT(ipcfgRecordExportSlot()));

    importBtn = new QPushButton("历史记录");
    importBtn->setToolTip("导入历史数据");
    importBtn->setIcon(QIcon(":/icon/import.png"));
    importBtn->setIconSize(QSize(50, 24));
    importBtn->setFixedSize(120, 35);
    toolBarLayout->addWidget(importBtn);
    connect(importBtn, SIGNAL(clicked()), this, SLOT(ipcfgRecordImportSlot()));

    progress = new QProgressBar();
    toolBarLayout->addWidget(progress);
    progress->hide();
    toolBarLayout->addStretch();
    mainLayout->addLayout(toolBarLayout);
    tabModel = new QStandardItemModel();
    tabModel->setColumnCount(ipcfgItemTitle.length());
    for (int n = 0; n < ipcfgItemTitle.length(); n++) {
        tabModel->setHeaderData(n, Qt::Horizontal, ipcfgItemTitle[n]);
    }

    tabView = new QTableView();
    tabView->setSortingEnabled(true);
    tabView->setModel(tabModel);
    //dDevTab->resizeColumnsToContents();
    tabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //dDevTab->verticalHeader()->hide();
    tabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabView->setAlternatingRowColors(true);
    tabView->setContextMenuPolicy(Qt::CustomContextMenu);
    mainLayout->addWidget(tabView);

    rmenu = new QMenu();
    searchDialog = new SearchDialog();
    searchDialog->setSearchModel(tabModel);
    QAction *search = new QAction("查找");
    search->setIcon(QIcon(":/icon/search.png"));
    rmenu->addAction(search);

    QAction *networkTestAction = new QAction("网络测试");
    networkTestAction->setIcon(QIcon(":/icon/network.png"));
    rmenu->addAction(networkTestAction);
    netTest = new NetworkTest();
    connect(networkTestAction, &QAction::triggered, this, [this] {
        auto item = tabModel->item(selectRow, 2);
        if (item) {
            netTest->setTargetAddr(item->text());
        }
        item = tabModel->item(selectRow, 3);
        if (item) {
            netTest->setTargetMask(item->text());
        }
        netTest->exec();
    });

    QAction *addressAction = new QAction("添加地址");
    rmenu->addAction(addressAction);
    addressAction->setIcon(QIcon(":/icon/add.png"));
    connect(addressAction, &QAction::triggered, this, [this] {
        auto gatewayitem = tabModel->item(selectRow, 4);
        auto netmaskitem = tabModel->item(selectRow, 3);
        if (gatewayitem && netmaskitem) {
            NetworkTest::addAddress("以太网", gatewayitem->text(), netmaskitem->text());
        }
    });

    QAction *delAddressAction = new QAction("删除地址");
    rmenu->addAction(delAddressAction);
    delAddressAction->setIcon(QIcon(":/icon/del.png"));
    connect(delAddressAction, &QAction::triggered, this, [this] {
        auto gatewayitem = tabModel->item(selectRow, 4);
        if (gatewayitem) {
            NetworkTest::delAddress("以太网", gatewayitem->text());
        }
    });

    connect(search, SIGNAL(triggered()), this, SLOT(searchMenuSlot()));
    connect(tabView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuSlot(QPoint)));
}

IpConfigRecordTable::~IpConfigRecordTable()
{
    delete netTest;
}

void IpConfigRecordTable::addItem(const Devices::ipconfigRecord &data)
{
    QList<QStandardItem *> itemList;

    QStandardItem *snItem = new QStandardItem(data.sn);
    snItem->setToolTip(data.sn);
    itemList.append(snItem);
    QStandardItem *timeItem = new QStandardItem(data.time);
    timeItem->setToolTip(data.time);
    itemList.append(timeItem);
    QStandardItem *ipItem = new QStandardItem(data.ip);
    ipItem->setToolTip(data.ip);
    itemList.append(ipItem);
    QStandardItem *netmaskItem = new QStandardItem(data.netmask);
    netmaskItem->setToolTip(data.netmask);
    itemList.append(netmaskItem);
    QStandardItem *gatewayItem = new QStandardItem(data.gateway);
    gatewayItem->setToolTip(data.gateway);
    itemList.append(gatewayItem);
    QStandardItem *remarkItem = new QStandardItem(data.remark);
    remarkItem->setToolTip(data.remark);
    itemList.append(remarkItem);
    tabModel->insertRow(0, itemList);
}

void IpConfigRecordTable::ipcfgRecordExportSlot()
{
    OperateXlsx oxlsx;
    windowConsole("开始导出IP配置记录");
    oxlsx.modelToXlsx(*tabModel, "IP配置记录", progress);
    windowConsole("结束导IP配置记录");
}

void IpConfigRecordTable::ipcfgRecordImportSlot()
{
    QList<Devices::ipconfigRecord> ipcfgs;

    tabModel->clear();
    tabModel->setColumnCount(ipcfgItemTitle.length());
    for (int n = 0; n < ipcfgItemTitle.length(); n++) {
        tabModel->setHeaderData(n, Qt::Horizontal, ipcfgItemTitle[n]);
    }

    DeviceDB::select(ipcfgs);
    for (int n = 0; n < ipcfgs.size(); n++) {
        addItem(ipcfgs.at(n));
    }
}

void IpConfigRecordTable::newIpconfigRecordSlot(QString &sn, QString &ip, QString &mask, QString &gateway, QString &remark)
{
    Devices::ipconfigRecord data;

    data.sn = sn;
    data.ip = ip;
    data.netmask = mask;
    data.gateway = gateway;
    data.remark = remark;
    data.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    addItem(data);
    DeviceDB::save(data);
}

void IpConfigRecordTable::contextMenuSlot(QPoint pos)
{
    auto index = tabView->indexAt(pos);
    if (index.isValid()) {
        selectRow = tabView->rowAt(pos.y());
        selectColumn = tabView->columnAt(pos.x());
        rmenu->exec(QCursor::pos());
    }
}

void IpConfigRecordTable::searchMenuSlot()
{
    searchDialog->setSearchColumn(selectColumn);
    searchDialog->popUp();
}
