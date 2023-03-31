#include "upgraderecordtable.h"
#include "mainwindow.h"

UpgradeRecordTable::UpgradeRecordTable(QWidget *parent) : QWidget(parent)
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
    connect(exportBtn, SIGNAL(clicked()), this, SLOT(exportSlot()));

    importBtn = new QPushButton("历史记录");
    importBtn->setToolTip("导入历史数据");
    importBtn->setIcon(QIcon(":/icon/import.png"));
    importBtn->setIconSize(QSize(50, 24));
    importBtn->setFixedSize(120, 35);
    toolBarLayout->addWidget(importBtn);
    connect(importBtn, SIGNAL(clicked()), this, SLOT(importSlot()));

    progress = new QProgressBar();
    toolBarLayout->addWidget(progress);
    progress->hide();
    toolBarLayout->addStretch();
    mainLayout->addLayout(toolBarLayout);

    tabModel = new QStandardItemModel();
    tabModel->setColumnCount(upRecordItemTitle.length());
    for (int n = 0; n < upRecordItemTitle.length(); n++) {
        tabModel->setHeaderData(n, Qt::Horizontal, upRecordItemTitle[n]);
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

    connect(search, SIGNAL(triggered()), this, SLOT(searchMenuSlot()));
    connect(tabView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuSlot(QPoint)));
}

void UpgradeRecordTable::additem(const Devices::upgradeRecord &data)
{
    QList<QStandardItem *> itemList;

    QStandardItem *snItem = new QStandardItem(data.sn);
    snItem->setToolTip(data.sn);
    itemList.append(snItem);
    QStandardItem *timeItem = new QStandardItem(data.time);
    timeItem->setToolTip(data.time);
    itemList.append(timeItem);
    QStandardItem *oldVerItem = new QStandardItem(data.old_ver);
    oldVerItem->setToolTip(data.old_ver);
    itemList.append(oldVerItem);
    QStandardItem *newVerItem = new QStandardItem(data.new_ver);
    newVerItem->setToolTip(data.new_ver);
    itemList.append(newVerItem);
    QStandardItem *modelItem = new QStandardItem(data.model);
    modelItem->setToolTip(data.model);
    itemList.append(modelItem);
    QStandardItem *stateItem = new QStandardItem(data.state);
    stateItem->setToolTip(data.state);
    itemList.append(stateItem);
    QStandardItem *remarkItem = new QStandardItem(data.remark);
    remarkItem->setToolTip(data.remark);
    itemList.append(remarkItem);
    tabModel->insertRow(0, itemList);
}

void UpgradeRecordTable::exportSlot()
{
    OperateXlsx oxlsx;
    windowConsole("开始导出升级记录");
    oxlsx.modelToXlsx(*tabModel, "升级记录", progress);
    windowConsole("结束导出升级记录");
}

void UpgradeRecordTable::importSlot()
{
    QList<Devices::upgradeRecord> records;

    tabModel->clear();
    tabModel->setColumnCount(upRecordItemTitle.length());
    for (int n = 0; n < upRecordItemTitle.length(); n++) {
        tabModel->setHeaderData(n, Qt::Horizontal, upRecordItemTitle[n]);
    }

    DeviceDB::select(records);
    for (int n = 0; n < records.size(); n++) {
        additem(records.at(n));
    }
}

void UpgradeRecordTable::newUpgradeRecordSlot(QString &sn, Devices::deviceState state, QString &oldVer, QString &newVer, QString &model, QString &remark)
{
    Devices::upgradeRecord data;

    data.sn = sn;
    data.state = Devices::statusText(state);
    data.old_ver = oldVer;
    data.new_ver = newVer;
    data.model = model;
    data.remark = remark;
    data.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    additem(data);
    DeviceDB::save(data);
}

void UpgradeRecordTable::contextMenuSlot(QPoint pos)
{
    auto index = tabView->indexAt(pos);
    if (index.isValid()) {
        selectRow = tabView->rowAt(pos.y());
        selectColumn = tabView->columnAt(pos.x());
        rmenu->exec(QCursor::pos());
    }
}

void UpgradeRecordTable::searchMenuSlot()
{
    searchDialog->setSearchColumn(selectColumn);
    searchDialog->popUp();
}
