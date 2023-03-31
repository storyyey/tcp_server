#include "devicetable.h"
#include "mainwindow.h"
#include "QtXlsx/xlsxdocument.h"

QTXLSX_USE_NAMESPACE

DeviceTable::DeviceTable(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    tabWidget = new QTabWidget(this);
    tabWidget->setDocumentMode(true);
    tabWidget->setStyleSheet("QTabBar::tab:selected{width:170;height:40;background-color:rgb(79,79,79);} \
                              QTabBar::tab:!selected{width:170;height:40;background-color:rgb(28,28,28);}");
    mainLayout->addWidget(tabWidget);
    this->setLayout(mainLayout);

    deviceDataTableInit();
    upgradeRecordTabInit();
    IPConfigRecordTabInit();
    IPConfigDialogInit();
    upgradeDialogInit();
    remarkDialogInit();
    searchDialogInit();

}

void DeviceTable::IPConfigDialogInit()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    // mainLayout->setContentsMargins(0, 0, 0, 0);
    IPConfigDialog = new QDialog(this);
    IPConfigDialog->setWindowTitle("设备IP配置");
    IPConfigDialog->setLayout(mainLayout);
    IPConfigDialog->setFixedWidth(480);
    //IPConfigDialog->setFixedSize(QSize(480, 240));
    QHBoxLayout *snLayout = new QHBoxLayout();
    QLabel *snLabel = new QLabel("序列号 :");
    //snLabel->setFixedWidth(50);
    snLayout->addWidget(snLabel);
    IPConfigSN = new QLineEdit();
    IPConfigSN->setText("123456789");
    IPConfigSN->setEnabled(false);
    snLayout->addWidget(IPConfigSN);
    mainLayout->addLayout(snLayout);

    QHBoxLayout *IPmodelLayout = new QHBoxLayout();
    QLabel *IPmodelLabel = new QLabel("IP模式 :");
    IPmodelLayout->addWidget(IPmodelLabel);
    IPmodelLBoBox = new QComboBox();
    IPmodelLBoBox->addItem("静态");
    IPmodelLBoBox->addItem("动态");
    IPmodelLayout->addWidget(IPmodelLBoBox);
    IPmodelLayout->addStretch();
    mainLayout->addLayout(IPmodelLayout);
    connect(IPmodelLBoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(IPModelChangeSlot(int)));

    QValidator *IPvalidator=new QIntValidator(0, 255, this);
    IPConfigWidget = new QWidget();
    mainLayout->addWidget(IPConfigWidget);
    QGridLayout *IPConfigLayout = new QGridLayout();
    IPConfigLayout->setContentsMargins(0, 0, 0, 0);
    IPConfigWidget->setLayout(IPConfigLayout);
    //QHBoxLayout *IPLayout = new QHBoxLayout();
    QLabel *IPLabel = new QLabel("IP/掩码:");
    //IPLabel->setFixedWidth(50);
    IPConfigLayout->addWidget(IPLabel, 2, 0);
    for (int n = 0; n < 4; n++) {
        ipEdit[n] = new QLineEdit();
        ipEdit[n]->setValidator(IPvalidator);
        ipEdit[n]->setPlaceholderText("0~255");
        IPConfigLayout->addWidget(ipEdit[n], 2, 1 + (2 * n));
        QString symbol(n < 3 ? "." : "/");
        IPConfigLayout->addWidget(new QLabel(symbol), 2, 2 + (2 * n));
        connect(ipEdit[n], SIGNAL(textEdited(QString)), this, SLOT(IPconfigChangeSlot(QString)));
    }

    QValidator *MaskValidator = new QIntValidator(1, 31, this);
    IPConfigMaskN = new QLineEdit();
    IPConfigMaskN->setPlaceholderText("1~31");
    IPConfigMaskN->setValidator(MaskValidator);
    IPConfigLayout->addWidget(IPConfigMaskN, 2, 9);
    connect(IPConfigMaskN, SIGNAL(textEdited(QString)), this, SLOT(IPMaskNumberChangeSlot(QString)));

    QLabel *netMaskLabel = new QLabel("掩码   :");;
    IPConfigLayout->addWidget(netMaskLabel, 3, 0);
    for (int n = 0; n < 4; n++) {
        maskEdit[n] = new QLineEdit();
        maskEdit[n]->setValidator(IPvalidator);
        maskEdit[n]->setPlaceholderText("0~255");
        IPConfigLayout->addWidget(maskEdit[n], 3, 1 + (2 * n));
        QString symbol(n < 3 ? "." : " ");
        IPConfigLayout->addWidget(new QLabel(symbol), 3, 2 + (2 * n));
        connect(maskEdit[n], SIGNAL(textEdited(QString)), this, SLOT(IPNetMaskChangeSlot(QString)));
    }

    QLabel *gagtewayLabel = new QLabel("网关   :");
    IPConfigLayout->addWidget(gagtewayLabel, 4, 0);
    for (int n = 0; n < 4; n++) {
        gatewayEdit[n] = new QLineEdit();
        gatewayEdit[n]->setValidator(IPvalidator);
        gatewayEdit[n]->setPlaceholderText("0~255");
        IPConfigLayout->addWidget(gatewayEdit[n], 4, 1 + (2 * n));
        QString symbol(n < 3 ? "." : " ");
        IPConfigLayout->addWidget(new QLabel(symbol), 4, 2 + (2 * n));
        connect(gatewayEdit[n], SIGNAL(textEdited(QString)), this, SLOT(IPConfigGatewayChangeSlot(QString)));
    }

    QLabel *paddingLabel1 = new QLabel();
    mainLayout->addWidget(paddingLabel1);
    QLabel *paddingLabel2 = new QLabel();
    mainLayout->addWidget(paddingLabel2);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    IPConfigOKBtn = new QPushButton("确定");
    IPConfigOKBtn->setFixedWidth(60);
    btnLayout->addWidget(IPConfigOKBtn);
    QLabel *spaceLabel = new QLabel();
    spaceLabel->setFixedWidth(40);
    btnLayout->addWidget(spaceLabel);
    IPConfigCancleBtn = new QPushButton("取消");
    IPConfigCancleBtn->setFixedWidth(60);
    btnLayout->addWidget(IPConfigCancleBtn);

    connect(IPConfigOKBtn, SIGNAL(clicked()), this, SLOT(IPConfigOKSlot()));
    connect(IPConfigCancleBtn, SIGNAL(clicked()), this, SLOT(IPConfigCancelSlot()));

    mainLayout->addLayout(btnLayout);
}

void DeviceTable::remarkDialogInit()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    remarkDialog = new QDialog(this);
    remarkDialog->setWindowTitle("备注");
    remarkDialog->setLayout(mainLayout);

    remarkLineEdit = new QLineEdit();
    remarkLineEdit->setMinimumWidth(320);
    mainLayout->addWidget(remarkLineEdit);
    connect(remarkLineEdit, SIGNAL(textEdited(QString)), this, SLOT(remarkChangeSlot(QString)));
}

void DeviceTable::upgradeDialogInit()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    upgradeDialog = new QDialog(this);
    upgradeDialog->setWindowTitle("设备升级");
    upgradeDialog->setLayout(mainLayout);
    IPConfigDialog->setFixedWidth(480);
    //upgradeDialog->setFixedSize(QSize(480, 240));
    QHBoxLayout *snLayout = new QHBoxLayout();
    QLabel *snLabel = new QLabel("序列号  :");
    snLayout->addWidget(snLabel);
    upgradeSN = new QLineEdit();
    upgradeSN->setText("123456789");
    upgradeSN->setEnabled(false);
    snLayout->addWidget(upgradeSN);
    mainLayout->addLayout(snLayout);

    QHBoxLayout *verLayout = new QHBoxLayout();
    QLabel *verLabel = new QLabel("当前版本:");
    verLayout->addWidget(verLabel);
    currVersion = new QLineEdit();
    currVersion->setText("000000000000");
    currVersion->setEnabled(false);
    verLayout->addWidget(currVersion);
    mainLayout->addLayout(verLayout);

    QHBoxLayout *modelLayout = new QHBoxLayout();
    QLabel *modelLabel = new QLabel("设备型号:");
    modelLayout->addWidget(modelLabel);
    devModel = new QLineEdit();
    devModel->setText("设备类型");
    devModel->setEnabled(false);
    modelLayout->addWidget(devModel);
    mainLayout->addLayout(modelLayout);

    QHBoxLayout *tvLaout = new QHBoxLayout();
    QLabel *tvLabel = new QLabel("目标版本:");
    tvLaout->addWidget(tvLabel);
    targetVer = new QLineEdit();
    targetVer->setToolTip("目标版本");
    tvLaout->addWidget(targetVer);
    mainLayout->addLayout(tvLaout);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileSelectBtn = new QPushButton();
    fileSelectBtn->setToolTip("选择升级文件");
    fileSelectBtn->setIcon(QIcon(":/icon/diricon.png"));
    fileSelectBtn->setIconSize(QSize(60, 20));
    fileSelectBtn->setStyleSheet(btnStyle);
    fileLayout->addWidget(fileSelectBtn);
    fileSelectEdit = new QLineEdit();
    fileSelectEdit->setPlaceholderText("未选择升级文件...");
    fileLayout->addWidget(fileSelectEdit);
    mainLayout->addLayout(fileLayout);
    connect(fileSelectBtn, SIGNAL(clicked()), this, SLOT(fileSelectPopDialogSlot()));

    fileSelectDialog = new QFileDialog();
    fileSelectDialog->setViewMode(QFileDialog::Detail);

    mainLayout->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    upgradeAgreeBtn = new QPushButton("确认升级");
    btnLayout->addWidget(upgradeAgreeBtn);
    upgradeAgreeBtn->setFixedWidth(80);
    upgradeCancelBtn = new QPushButton("取消升级");
    btnLayout->addWidget(upgradeCancelBtn);
    upgradeCancelBtn->setFixedWidth(80);
    mainLayout->addLayout(btnLayout);
    connect(upgradeAgreeBtn, SIGNAL(clicked()), this, SLOT(upgradeAgreeBtnSlot()));
    connect(upgradeCancelBtn, SIGNAL(clicked()), this, SLOT(upgradeCancelBtnSlot()));
}

void DeviceTable::searchDialogInit()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    searchDialog = new QDialog(this);
    searchDialog->setWindowTitle("查找");
    searchDialog->setLayout(mainLayout);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchBtn = new QPushButton();
    searchBtn->setToolTip("搜索");
    searchLayout->addWidget(searchBtn);
    QIcon searchIcon;
    searchIcon.addFile(tr(":/icon/search.png"));
    searchBtn->setIcon(searchIcon);
    searchBtn->setStyleSheet(btnStyle);

    searchLineEdit = new QLineEdit();
    searchLayout->addWidget(searchLineEdit);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout *progressLayout = new QHBoxLayout();
    QLabel *searchProLabel = new QLabel("查找进度");
    progressLayout->addWidget(searchProLabel);
    searchProgress = new QProgressBar();
    progressLayout->addWidget(searchProgress);
    mainLayout->addLayout(progressLayout);

    QHBoxLayout *resultLayout = new QHBoxLayout();
    QLabel *resultPrefix = new QLabel("总数:");
    resultPrefix->setFixedWidth(40);
    resultLayout->addWidget(resultPrefix);
    searchResult = new QLabel();
    resultLayout->addWidget(searchResult);
    mainLayout->addLayout(resultLayout);

    searchModel = new QStandardItemModel();
    searchModel->setColumnCount(7);
    searchModel->setHeaderData(0, Qt::Horizontal, "状态");
    searchModel->setHeaderData(1, Qt::Horizontal, "上线时间");
    searchModel->setHeaderData(2, Qt::Horizontal, "网关IP");
    searchModel->setHeaderData(3, Qt::Horizontal, "序列号");
    searchModel->setHeaderData(4, Qt::Horizontal, "软件版本");
    searchModel->setHeaderData(5, Qt::Horizontal, "设备型号");
    searchModel->setHeaderData(6, Qt::Horizontal, "备注");

    searchTabView = new QTableView();
    searchTabView->setModel(searchModel);
    searchTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    searchTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    searchTabView->setAlternatingRowColors(true);
    mainLayout->addWidget(searchTabView);
    connect(searchBtn, SIGNAL(clicked()), this, SLOT(deviceDataSearchBtnSlot()));
}

void DeviceTable::IPConfigAutoEdit()
{
    int n = 0;
    QHostAddress addr;
    uint32_t subNet = 0;

    IPConfigNetMask = 0;
    for (n = 0; n < IPConfigMaskNum; n++) {
        IPConfigNetMask |= (0x80000000 >> n);
    }
    subNet = IPConfigNetMask & IPConfigIP;

    IPConfigMaskN->setText(QString("%1").arg(IPConfigMaskNum));

    maskEdit[0]->setText(QString("%1").arg(0xff & (IPConfigNetMask >> 24)));
    maskEdit[1]->setText(QString("%1").arg(0xff & (IPConfigNetMask >> 16)));
    maskEdit[2]->setText(QString("%1").arg(0xff & (IPConfigNetMask >> 8)));
    maskEdit[3]->setText(QString("%1").arg(0xff & (IPConfigNetMask >> 0)));

    addr.setAddress(subNet);
    windowConsole(QString("当前配置子网IP为:") + addr.toString());
    IPConfigGateway = subNet + 1;
    addr.setAddress(IPConfigGateway);
    windowConsole(QString("当前配置最小可用IP地址为:") + addr.toString());
    gatewayEdit[0]->setText(QString("%1").arg(0xff & (IPConfigGateway >> 24)));
    gatewayEdit[1]->setText(QString("%1").arg(0xff & (IPConfigGateway >> 16)));
    gatewayEdit[2]->setText(QString("%1").arg(0xff & (IPConfigGateway >> 8)));
    gatewayEdit[3]->setText(QString("%1").arg(0xff & (IPConfigGateway >> 0)));
}

void DeviceTable::deviceDataTableInit()
{
    deviceDataWidget = new QWidget();
    deviceDataWidget->setStyleSheet(".QWidget{background-color: rgb(79,79,79);}");
    QVBoxLayout *deviceTabVBoxLayout = new QVBoxLayout();
    //deviceTabVBoxLayout->setContentsMargins(0, 0, 0, 0);
    deviceDataWidget->setLayout(deviceTabVBoxLayout);
    tabWidget->addTab(deviceDataWidget, "设备表");
    tabWidget->tabBar()->setTabTextColor(tabWidget->count() - 1, QColor(255, 255, 255));
    tabWidget->setTabIcon(tabWidget->count() - 1, QIcon(":/icon/excel.png"));

    QHBoxLayout *deviceTabHboxLayout = new QHBoxLayout();
    deviceDataTabRefreshBtn = new QPushButton("刷新");
    deviceDataTabRefreshBtn->setToolTip("刷新设备数据表");
    deviceDataTabRefreshBtn->setStyleSheet(btnStyle);
    deviceDataTabRefreshBtn->setIcon(QIcon(":/icon/refresh.png"));
    deviceDataTabRefreshBtn->setIconSize(QSize(50, 24));
    deviceDataTabRefreshBtn->setFixedSize(120, 35);
    deviceTabHboxLayout->addWidget(deviceDataTabRefreshBtn);
    connect(deviceDataTabRefreshBtn, SIGNAL(clicked()), this, SLOT(deviceDataTabRefreshSlot()));

    deviceDataTabExportBtn = new QPushButton("导出xlsx");
    deviceDataTabExportBtn->setToolTip("数据表导出");
    deviceDataTabExportBtn->setStyleSheet(btnStyle);
    deviceDataTabExportBtn->setIcon(QIcon(":/icon/export.png"));
    deviceDataTabExportBtn->setIconSize(QSize(50, 24));
    deviceDataTabExportBtn->setFixedSize(120, 35);
    deviceTabHboxLayout->addWidget(deviceDataTabExportBtn);
    connect(deviceDataTabExportBtn, SIGNAL(clicked()), this, SLOT(deviceDataTabExportSlot()));

    deviceDataTabImportBtn = new QPushButton("导入历史数据");
    deviceDataTabImportBtn->setToolTip("导入历史数据");
    deviceDataTabImportBtn->setStyleSheet(btnStyle);
    //deviceDataTabImportBtn->setIcon(QIcon(":/icon/refresh.png"));
    //deviceDataTabImportBtn->setIconSize(QSize(50, 24));
    deviceDataTabImportBtn->setFixedSize(120, 35);
    deviceTabHboxLayout->addWidget(deviceDataTabImportBtn);
    connect(deviceDataTabImportBtn, SIGNAL(clicked()), this, SLOT(deviceDataTabImportSlot()));

    deviceDataTabExportProgress = new QProgressBar();
    deviceTabHboxLayout->addWidget(deviceDataTabExportProgress);
    deviceDataTabExportProgress->hide();
    deviceTabHboxLayout->addStretch();
    deviceTabVBoxLayout->addLayout(deviceTabHboxLayout);

    devTabModel = new QStandardItemModel();
    devTabModel->setColumnCount(devInfoItemTitle.length());
    for (int n = 0; n < devInfoItemTitle.length(); n++) {
        devTabModel->setHeaderData(n, Qt::Horizontal, devInfoItemTitle[n]);
    }

    for (int nn = 0; nn < 0; nn++) {
        QList<QStandardItem *> itemList;
        itemList.append(new QStandardItem("在线"));
        itemList.append(new QStandardItem("2021/7/18 12:20:20"));
        itemList.append(new QStandardItem("192.168.1.1"));
        itemList.append(new QStandardItem("F030000000000012"));
        itemList.append(new QStandardItem("3.10"));
        itemList.append(new QStandardItem("1000"));
        itemList.append(new QStandardItem(QString("%1").arg(nn)));
        devTabModel->insertRow(0, itemList);
    }

    deviceDataTabView = new QTableView();
    deviceDataTabView->setModel(devTabModel);
    //dDevTab->resizeColumnsToContents();
    deviceDataTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //dDevTab->verticalHeader()->hide();
    deviceDataTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    deviceDataTabView->setAlternatingRowColors(true);
    //dDevTab->setSelectionMode(QAbstractItemView::SingleSelection);
    //dDevTab->setSelectionBehavior(QAbstractItemView::SelectRows);
    deviceDataTabView->setContextMenuPolicy(Qt::CustomContextMenu);
    deviceDataTabView->setAutoScroll(true);
    deviceConfigMenu = new QMenu(deviceDataTabView);

    QAction *actionUpgrade = new QAction("升级");
    actionUpgrade->setIcon(QIcon(":/icon/upgrade.png"));

    QAction *remark = new QAction("备注");
    remark->setIcon(QIcon(":/icon/remark.png"));

    QAction *ipconfig = new QAction("IP配置");
    ipconfig->setIcon(QIcon(":/icon/set.png"));

    QAction *search = new QAction("查找");
    search->setIcon(QIcon(":/icon/search.png"));

    deviceConfigMenu->addAction(actionUpgrade);
    deviceConfigMenu->addSeparator();
    deviceConfigMenu->addAction(remark);
    deviceConfigMenu->addSeparator();
    deviceConfigMenu->addAction(ipconfig);
    deviceConfigMenu->addSeparator();
    deviceConfigMenu->addAction(search);
    deviceTabVBoxLayout->addWidget(deviceDataTabView);

    connect(actionUpgrade, SIGNAL(triggered()), this, SLOT(upgradeMenuSlot()));
    connect(ipconfig, SIGNAL(triggered()), this, SLOT(IPConfigMenuSlot()));
    connect(remark, SIGNAL(triggered()), this, SLOT(remarkMenuSlot()));
    connect(search, SIGNAL(triggered()), this, SLOT(searchMenuSlot()));

    connect(deviceDataTabView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuSlot(QPoint)));
}

void DeviceTable::upgradeRecordTabInit()
{
    upgradeRecordWidget = new QWidget();
    upgradeRecordWidget->setStyleSheet(".QWidget{background-color: rgb(79,79,79);}");
    QVBoxLayout *upgradeRecordTabVBoxLayout = new QVBoxLayout();
    upgradeRecordWidget->setLayout(upgradeRecordTabVBoxLayout);
    tabWidget->addTab(upgradeRecordWidget, "升级记录");
    tabWidget->tabBar()->setTabTextColor(tabWidget->count() - 1, QColor(255, 255, 255));
    tabWidget->setTabIcon(tabWidget->count() - 1, QIcon(":/icon/upgrade.png"));

    QHBoxLayout *upgradeRecordTabHboxLayout = new QHBoxLayout();

    upgradeRecordTabExportBtn = new QPushButton("导出xlsx");
    upgradeRecordTabExportBtn->setToolTip("数据表导出");
    upgradeRecordTabExportBtn->setStyleSheet(btnStyle);
    upgradeRecordTabExportBtn->setIcon(QIcon(":/icon/export.png"));
    upgradeRecordTabExportBtn->setIconSize(QSize(50, 24));
    upgradeRecordTabExportBtn->setFixedSize(120, 35);
    upgradeRecordTabHboxLayout->addWidget(upgradeRecordTabExportBtn);
    connect(upgradeRecordTabExportBtn, SIGNAL(clicked()), this, SLOT(upgradeRecordExportSlot()));

    upgradeRecordTabImportBtn = new QPushButton("导入历史数据");
    upgradeRecordTabImportBtn->setToolTip("导入历史数据");
    upgradeRecordTabImportBtn->setStyleSheet(btnStyle);
    upgradeRecordTabImportBtn->setFixedSize(120, 35);
    upgradeRecordTabHboxLayout->addWidget(upgradeRecordTabImportBtn);
    connect(upgradeRecordTabImportBtn, SIGNAL(clicked()), this, SLOT(upgradeRecordImportSlot()));

    upgradeRecordTabExportProgress = new QProgressBar();
    upgradeRecordTabHboxLayout->addWidget(upgradeRecordTabExportProgress);
    upgradeRecordTabExportProgress->hide();
    upgradeRecordTabHboxLayout->addStretch();
    upgradeRecordTabVBoxLayout->addLayout(upgradeRecordTabHboxLayout);

    upgradeRecordModel = new QStandardItemModel();
    upgradeRecordModel->setColumnCount(upRecordItemTitle.length());
    for (int n = 0; n < upRecordItemTitle.length(); n++) {
        upgradeRecordModel->setHeaderData(n, Qt::Horizontal, upRecordItemTitle[n]);
    }

    upgradeRecordTabView = new QTableView();
    upgradeRecordTabView->setModel(upgradeRecordModel);
    //dDevTab->resizeColumnsToContents();
    upgradeRecordTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //dDevTab->verticalHeader()->hide();
    upgradeRecordTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    upgradeRecordTabView->setAlternatingRowColors(true);
    upgradeRecordTabVBoxLayout->addWidget(upgradeRecordTabView);
}

void DeviceTable::IPConfigRecordTabInit()
{
    IPConfigRecordWidget = new QWidget();
    IPConfigRecordWidget->setStyleSheet(".QWidget{background-color: rgb(79,79,79);}");
    QVBoxLayout *IPConfigVBoxLayout = new QVBoxLayout();
    IPConfigRecordWidget->setLayout(IPConfigVBoxLayout);
    tabWidget->addTab(IPConfigRecordWidget, "  IP配置记录");
    tabWidget->tabBar()->setTabTextColor(tabWidget->count() - 1, QColor(255, 255, 255));
    tabWidget->setTabIcon(tabWidget->count() - 1, QIcon(":/icon/set.png"));

    QHBoxLayout *IPConfigHboxLayout = new QHBoxLayout();

    IPConfigTabExportBtn = new QPushButton("导出xlsx");
    IPConfigTabExportBtn->setToolTip("数据表导出");
    IPConfigTabExportBtn->setStyleSheet(btnStyle);
    IPConfigTabExportBtn->setIcon(QIcon(":/icon/export.png"));
    IPConfigTabExportBtn->setIconSize(QSize(50, 24));
    IPConfigTabExportBtn->setFixedSize(120, 35);
    IPConfigHboxLayout->addWidget(IPConfigTabExportBtn);
    connect(IPConfigTabExportBtn, SIGNAL(clicked()), this, SLOT(ipcfgRecordExportSlot()));

    IPConfigTabImportBtn = new QPushButton("导入历史数据");
    IPConfigTabImportBtn->setToolTip("导入历史数据");
    IPConfigTabImportBtn->setStyleSheet(btnStyle);
    IPConfigTabImportBtn->setFixedSize(120, 35);
    IPConfigHboxLayout->addWidget(IPConfigTabImportBtn);
    connect(IPConfigTabImportBtn, SIGNAL(clicked()), this, SLOT(ipcfgRecordImportSlot()));

    IPConfigTabExportProgress = new QProgressBar();
    IPConfigHboxLayout->addWidget(IPConfigTabExportProgress);
    IPConfigTabExportProgress->hide();
    IPConfigHboxLayout->addStretch();
    IPConfigVBoxLayout->addLayout(IPConfigHboxLayout);
    IPConfigModel = new QStandardItemModel();
    IPConfigModel->setColumnCount(ipcfgItemTitle.length());
    for (int n = 0; n < ipcfgItemTitle.length(); n++) {
        IPConfigModel->setHeaderData(n, Qt::Horizontal, ipcfgItemTitle[n]);
    }

    IPConfigTabView = new QTableView();
    IPConfigTabView->setModel(IPConfigModel);
    //dDevTab->resizeColumnsToContents();
    IPConfigTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //dDevTab->verticalHeader()->hide();
    IPConfigTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    IPConfigTabView->setAlternatingRowColors(true);
    IPConfigVBoxLayout->addWidget(IPConfigTabView);
}

void DeviceTable::tableSaveToXlsx(QStandardItemModel &tab, const QString &file, QProgressBar *proBar)
{
    Document xlsx;
    int row = 0, column = 0;

    if (!tab.rowCount()) {
        QMessageBox::information(NULL, "导出结束", QString("无数据"),
                             QMessageBox::Yes, QMessageBox::Yes);

        return ;
    }

    xlsx.addSheet("sheet1");

    if (proBar) {
        proBar->show();
        proBar->setMinimum(0);
        proBar->setMaximum(tab.rowCount());
    }
    for (column = 1; column <= tab.columnCount() ; column++) {
        xlsx.setColumnWidth(column, 20);
        QStandardItem *item = tab.horizontalHeaderItem(column - 1);
        if (item) {
            xlsx.write(1, column, item->text());
        }
    }

    for (row = 2; row <= tab.rowCount() + 1; row++) {
        for (column = 1; column <= tab.columnCount() ; column++) {
            QStandardItem *item = tab.item(row - 2, column - 1);
            if (item) {
                xlsx.write(row, column, item->text());
            }
        }
        if (proBar) {
            proBar->setValue(row - 1);
        }
    }
    if (proBar) {
        proBar->setValue(tab.rowCount());
    }
    xlsx.saveAs(file + ".xlsx");
    QMessageBox::information(NULL, "导出结束", QString("导出成功=>%1").arg(file + ".xlsx"),
                             QMessageBox::Yes, QMessageBox::Yes);
    if (proBar) {
        proBar->reset();
        proBar->hide();
    }
}

void DeviceTable::devicesInit(Devices *devices)
{
    this->devices = devices;
}

void DeviceTable::devicesDBInit(DeviceDB *ddb)
{
    this->DDB = ddb;
}

static inline QString deviceStatusStr(Devices *devices, Devices::devInfo &data)
{
    if (data.state == Devices::UPGRADE_PROGRESS) {
        return Devices::deviceStatusText(data.state) + QString(" (...%1秒)").arg(devices->UpgradeTimerValue(data.sn));
    }
    else {
        return Devices::deviceStatusText(data.state);
    }
}

static inline QString deviceModelStr(QString &model)
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

void DeviceTable::addDevicesTableItem(Devices::devInfo &data)
{
    QList<QStandardItem *> idItem = devTabModel->findItems(data.sn, Qt::MatchFixedString, 3);

    if (idItem.length() == 0) {
        QList<QStandardItem *> itemList;

        itemList.append(new QStandardItem(deviceStatusStr(devices, data)));
        itemList.append(new QStandardItem(data.time));
        itemList.append(new QStandardItem(data.ip));
        itemList.append(new QStandardItem(data.sn));
        itemList.append(new QStandardItem(data.version));
        QStandardItem *modelItem = new QStandardItem(data.model);
        modelItem->setToolTip(deviceModelStr(data.model));
        itemList.append(modelItem);
        itemList.append(new QStandardItem(data.remark));
        devTabModel->insertRow(0, itemList);
        windowConsole(QString("发现新设备当前列表设备总数%1").arg(devTabModel->rowCount()));
    }
    else {
        int row = 0;
        foreach(QStandardItem * entry, idItem) {
            row = entry->row();
            devTabModel->item(row, 0)->setText(deviceStatusStr(devices, data));
            devTabModel->item(row, 1)->setText(data.time);
            devTabModel->item(row, 2)->setText(data.ip);
            devTabModel->item(row, 3)->setText(data.sn);
            devTabModel->item(row, 4)->setText(data.version);
            devTabModel->item(row, 5)->setText((data.model));
            devTabModel->item(row, 5)->setToolTip(deviceModelStr(data.model));
            devTabModel->item(row, 6)->setText(data.remark);
        }
    }
}

void DeviceTable::addUpRecordTableItem(Devices::upgradeRecord &data)
{
    QList<QStandardItem *> itemList;

    itemList.append(new QStandardItem(data.sn));
    itemList.append(new QStandardItem(data.time));
    itemList.append(new QStandardItem(data.old_ver));
    itemList.append(new QStandardItem(data.new_ver));
    itemList.append(new QStandardItem(data.model));
    itemList.append(new QStandardItem(data.state));
    QStandardItem *remarkItem = new QStandardItem(data.remark);
    remarkItem->setToolTip(data.remark);
    itemList.append(remarkItem);
    upgradeRecordModel->insertRow(0, itemList);
}

void DeviceTable::addIpcfgRecordTableItem(Devices::ipconfigRecord &data)
{
    QList<QStandardItem *> itemList;

    itemList.append(new QStandardItem(data.sn));
    itemList.append(new QStandardItem(data.time));
    itemList.append(new QStandardItem(data.ip));
    itemList.append(new QStandardItem(data.netmask));
    itemList.append(new QStandardItem(data.gateway));
    QStandardItem *remarkItem = new QStandardItem(data.remark);
    remarkItem->setToolTip(data.remark);
    itemList.append(remarkItem);
    IPConfigModel->insertRow(0, itemList);
}

void DeviceTable::contextMenuSlot(QPoint pos)
{
    auto index = deviceDataTabView->indexAt(pos);
    if (index.isValid()) {
        selectRow = deviceDataTabView->rowAt(pos.y());
        selectColumn = deviceDataTabView->columnAt(pos.x());
        windowConsole(QString("当前鼠标右键选择第%1行%2列").arg(selectRow).arg(selectColumn));
        deviceConfigMenu->exec(QCursor::pos());
    }
}



void DeviceTable::deviceInfoChange(Devices::devInfo &data)
{
    addDevicesTableItem(data);
    DDB->save(data);
}

void DeviceTable::selectDeviceInfoImport(Devices::devInfo &data)
{
    Devices::devInfo devinfo;
    if (devices->getInfo(data.sn, devinfo)) {
        data.state = devinfo.state;
    }
    else {
        data.state = Devices::OFFLINE;
    }
    devices->addInfo(data.sn, data);
}

void DeviceTable::upgradeMenuSlot()
{
    windowConsole("弹出升级窗口");
    QString sn = devTabModel->item(selectRow, 3)->text();
    QString ver = devTabModel->item(selectRow, 4)->text();
    QString model = devTabModel->item(selectRow, 5)->text();

    if (devices->isOffline(sn)) {
        QMessageBox::warning(NULL, "升级设备", "设备不在线",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        return ;
    }

    upgradeSN->setText(sn);
    currVersion->setText(ver);
    devModel->setText(model);
    if (!targetVer->text().size()) {
        targetVer->setText(ver);
    }
    upgradeDialog->exec();
    windowConsole("关闭升级窗口");
}

void DeviceTable::IPConfigMenuSlot()
{
    windowConsole("弹出IP配置窗口");
    QString sn = devTabModel->item(selectRow, 3)->text();

    if (devices->isOffline(sn)) {
        QMessageBox::warning(NULL, "IP配置", "设备不在线",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }
    IPConfigSN->setText(sn);
    IPConfigDialog->exec();
    windowConsole("关闭IP配置窗口");
}

void DeviceTable::remarkMenuSlot()
{
    windowConsole("弹出备注窗口");
    if (devTabModel->item(selectRow, 6)) {
        remarkLineEdit->setText(devTabModel->item(selectRow, 6)->text());
    }
    searchProgress->reset();
    remarkDialog->exec();
    windowConsole("关闭备注窗口");
}

void DeviceTable::searchMenuSlot()
{
    QDesktopWidget dw;

    windowConsole("弹出查找窗口");
    searchProgress->reset();
    searchDialog->resize(QSize(dw.width() / 2, dw.height() / 3 ));
    searchResult->clear();
    searchLineEdit->clear();
    searchLineEdit->setPlaceholderText(devTabModel->horizontalHeaderItem(selectColumn)->text());
    searchDialog->exec();
    searchModel->removeRows(0, searchModel->rowCount());
    windowConsole("关闭查找窗口");
}

void DeviceTable::IPModelChangeSlot(int index)
{
    if (index == 0) {
        IPConfigWidget->show();
    }
    else if (index == 1) {
        IPConfigWidget->hide();
    }
}

void DeviceTable::IPConfigOKSlot()
{
    int nn = 0;
    QMessageBox msgBox;

    for (nn = 0; nn < 4; nn++) {
        if (!ipEdit[nn]->text().length()) {
            ipEdit[nn]->setText("0");
        }
        if (!maskEdit[nn]->text().length()) {
            maskEdit[nn]->setText("0");
        }
        if (!gatewayEdit[nn]->text().length()) {
            gatewayEdit[nn]->setText("0");
        }
    }
    QString ipstr = ipEdit[0]->text() + "." +\
                    ipEdit[1]->text() + "." +\
                    ipEdit[2]->text() + "." +\
                    ipEdit[3]->text();
    QHostAddress ip(ipstr);
    IPConfigIP = ip.toIPv4Address();

    QString maskstr = maskEdit[0]->text() + "." +\
                    maskEdit[1]->text() + "." +\
                    maskEdit[2]->text() + "." +\
                    maskEdit[3]->text();
    QHostAddress mask(maskstr);
    IPConfigNetMask = mask.toIPv4Address();

    QString gatewaystr = gatewayEdit[0]->text() + "." +\
                    gatewayEdit[1]->text() + "." +\
                    gatewayEdit[2]->text() + "." +\
                    gatewayEdit[3]->text();
    QHostAddress gateway(gatewaystr);
    IPConfigGateway = gateway.toIPv4Address();

    for (nn = 0; nn < 32; nn++) {
        if (!(IPConfigNetMask & (0x80000000 >> nn))) {
            for (; nn < 32; nn++) {
                if (IPConfigNetMask & (0x80000000 >> nn)) {
                    QMessageBox::warning(NULL, "配置异常", "掩码格式不正确",
                                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

                    return ;
                }
            }
        }
    }

    if (ip.isInSubnet(gateway, IPConfigMaskNum) == false) {
        QMessageBox::warning(NULL, "配置异常", "网关地址不在子网内",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    int ret = QMessageBox::information(NULL, QString("配置确认:%1").arg(IPConfigSN->text()), \
                             QString("IP    : %1/%2\n掩码 : %3\n网关 : %4\n").arg(ipstr)\
                             .arg(IPConfigMaskNum).arg(maskstr).arg(gatewaystr),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (QMessageBox::Yes == ret) {
        windowConsole(QString("\n设备(%1)IP配置\n").arg(IPConfigSN->text()) +\
                  QString("IP   : %1/%2\n掩码 : %3\n网关 : %4").arg(ipstr)\
                 .arg(IPConfigMaskNum).arg(maskstr).arg(gatewaystr));

        QString sn = IPConfigSN->text();
        QString bootproto = (IPmodelLBoBox->currentText() == "静态" ? "static": "dhcp");

        QJsonObject ipconfigCmd = QJsonObject {
            {
                {"command", "ifconfig"},
                {"data",
                    QJsonObject{
                        {"interface", "eth0"},
                        {"bootproto", bootproto},
                        {"address", ipstr},
                        {"netmask", maskstr},
                        {"gateway", gatewaystr},
                    }
                },
            }
        };
        QJsonDocument doc;
        doc.setObject(ipconfigCmd);
        windowConsole("添加等待发送的IP配置消息 --------->");
        windowConsole(doc.toJson());
        devices->addOperatorCmd(sn, doc.toJson());

        Devices::ipconfigRecord ipcfg;
        Devices::devInfo devinfo;

        devices->getInfo(sn, devinfo);
        ipcfg.sn = sn;
        ipcfg.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ipcfg.ip = ipstr;
        ipcfg.netmask = maskstr;
        ipcfg.gateway = gatewaystr;
        ipcfg.remark = devinfo.remark;
        addIpcfgRecordTableItem(ipcfg);
        DDB->save(ipcfg);

        IPConfigDialog->close();
    }
}

void DeviceTable::IPConfigCancelSlot()
{
    IPConfigDialog->close();
}

void DeviceTable::IPconfigChangeSlot(const QString &text)
{
    uint32_t val = text.toUInt();

    if (text.length() == 0) {
        return ;
    }

    if (val > 0xff) {
        for (int n = 0; n < 4; n++) {
            if (ipEdit[n]->text().toInt() > 0xff) {
                ipEdit[n]->setText(ipEdit[n]->text().left(\
                                       ipEdit[n]->text().size() - 1));
                break;
            }
        }
    }
    else {
        QString ip = ipEdit[0]->text() + "." +\
                     ipEdit[1]->text() + "." +\
                     ipEdit[2]->text() + "." +\
                     ipEdit[3]->text();
        IPConfigIP = QHostAddress(ip).toIPv4Address();
        IPConfigAutoEdit();
    }
}

void DeviceTable::IPMaskNumberChangeSlot(const QString &text)
{
    int netMaskNum = IPConfigMaskN->text().toInt();

    if (text.length() == 0) {
        return ;
    }

    if (netMaskNum < 1 || netMaskNum > 31) {
        IPConfigMaskN->setText(IPConfigMaskN->text().left(IPConfigMaskN->text().size() - 1));
    }
    IPConfigMaskNum = IPConfigMaskN->text().toInt();

    IPConfigAutoEdit();
}

void DeviceTable::IPConfigGatewayChangeSlot(const QString &text)
{
    uint32_t val = text.toUInt();

    if (text.length() == 0) {
        return ;
    }

    if (val > 0xff) {
        for (int n = 0; n < 4; n++) {
            if (gatewayEdit[n]->text().toInt() > 0xff) {
                gatewayEdit[n]->setText(gatewayEdit[n]->text().left(\
                                       gatewayEdit[n]->text().size() - 1));
                break;
            }
        }
    }
    else {
        QString ip = gatewayEdit[0]->text() + "." +\
                     gatewayEdit[1]->text() + "." +\
                     gatewayEdit[2]->text() + "." +\
                     gatewayEdit[3]->text();
        IPConfigGateway = QHostAddress(ip).toIPv4Address();
    }
}

void DeviceTable::IPNetMaskChangeSlot(const QString &text)
{
    int n = 0;
    uint32_t val = text.toUInt();
    uint32_t IPConfigNetMaskTmp = 0;
    uint8_t IPConfigMaskNumTmp = 0;

    if (text.length() == 0) {
        return ;
    }

    if (val > 0xff) {
        for (int n = 0; n < 4; n++) {
            if (maskEdit[n]->text().toInt() > 0xff) {
                maskEdit[n]->setText(maskEdit[n]->text().left(\
                                       maskEdit[n]->text().size() - 1));
                break;
            }
        }
    }
    else {
        QString ip = maskEdit[0]->text() + "." +\
                     maskEdit[1]->text() + "." +\
                     maskEdit[2]->text() + "." +\
                     maskEdit[3]->text();
        IPConfigNetMask = QHostAddress(ip).toIPv4Address();
    }

    for (n = 0; n < 32; n++) {
        if (IPConfigNetMask & (0x80000000 >> n)) {
            IPConfigMaskNumTmp++;
            IPConfigNetMaskTmp |= (0x80000000 >> n);
        }
        else {
            break;
        }
    }

    if (IPConfigNetMaskTmp == IPConfigNetMask) {
        IPConfigMaskNum = IPConfigMaskNumTmp;
        IPConfigAutoEdit();
    }
}

void DeviceTable::fileSelectPopDialogSlot()
{
    QString fileFull = QFileDialog::getOpenFileName(this, tr("升级文件选择"));

    QFileInfo fileInfo(fileFull);
    QString fileName = fileInfo.fileName();
    QString filePath = fileInfo.absolutePath();
    fileSelectEdit->setText(fileName);
    devices->setUpgradeFilePath(filePath);
}

void DeviceTable::upgradeAgreeBtnSlot()
{
    QString info = QString("SN:\t%1 \n"\
                           "型号:\t%2 \n"\
                           "当前版本:\t%3 \n"\
                           "目标版本:\t%4 \n"\
                           "文件:\t%5 \n")\
                            .arg(upgradeSN->text())\
                            .arg(devModel->text())\
                            .arg(currVersion->text())\
                            .arg(targetVer->text())\
                            .arg(fileSelectEdit->text());


    int ret = QMessageBox::information(NULL, "确认", info,\
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

     QFile theFile(fileSelectEdit->text());
     if (theFile.open(QIODevice::ReadOnly)) {
        QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
        windowConsole(QString(ba.toHex()));
        theFile.close();
     }

    if (QMessageBox::Yes == ret) {
        QString file = devices->getFileDownUrl() + "file=" + fileSelectEdit->text();
        QString version = currVersion->text();
        QString model = devModel->text();
        QString md5 = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5).toHex();
        QString sn = upgradeSN->text();
        QString tVer = targetVer->text();

        QJsonObject upgradeCmd = QJsonObject{
            {
                {"command", "upgrade"},
                {"data",
                    QJsonObject{
                        {"upgradable", "true"},
                        {"url", file},
                        {"version", version},
                        {"interval", 5},
                        {"model", model},
                        {"md5", md5},
                    }
                },
            }
        };

        QJsonDocument doc;
        doc.setObject(upgradeCmd);
        windowConsole("添加等待发送的升级命令 --------->");
        windowConsole(doc.toJson());
        devices->addOperatorCmd(sn, doc.toJson());
        devices->setStatus(sn, Devices::UPGRADE_WAIT);
        if (tVer.size()) {
            devices->setTargetVer(sn, tVer);
        }
        else {
            devices->setTargetVer(sn, version);
        }

        /* 保存升级操作记录 */
        Devices::upgradeRecord urData;

        urData.remark = QString("升级文件为:" + fileSelectEdit->text());
        urData.sn = sn;
        urData.model = model;
        devices->getTargetVer(sn, urData.new_ver);
        urData.old_ver = version;
        urData.state = Devices::deviceStatusText(Devices::UPGRADE_WAIT);
        urData.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        addUpRecordTableItem(urData);
        DDB->save(urData);

        upgradeDialog->close();
    }
}

void DeviceTable::upgradeCancelBtnSlot()
{
    upgradeDialog->close();
}

void DeviceTable::remarkChangeSlot(const QString &text)
{
    QStandardItem *item = devTabModel->item(selectRow, 3);
    if (item) {
        Devices::devInfo devinfo;
        if (devices->getInfo(item->text(), devinfo)) {
            devinfo.remark = text;
            devices->addInfo(item->text(), devinfo);
        }
    }
}

void DeviceTable::deviceDataSearchBtnSlot()
{
    int nn = 0;
    int row = 0;

    QList<QStandardItem *> item = devTabModel->findItems(searchLineEdit->text(), Qt::MatchContains, selectColumn);

    searchModel->removeRows(0, searchModel->rowCount());
    searchResult->setText(QString("-- %1 --").arg(item.size()));
    searchProgress->reset();
    if (!item.size()) {
        searchProgress->setMinimum(0);
        searchProgress->setMaximum(1);
        searchProgress->setValue(1);
        return;
    }

    searchProgress->setMinimum(0);
    searchProgress->setMaximum(item.size());
    foreach(QStandardItem * entry, item) {
        for (nn = 0; nn < devTabModel->columnCount(); nn++) {
            QStandardItem *item = devTabModel->item(entry->row(), nn);
            if (item) {
                searchModel->setItem(row, nn, new QStandardItem(item->text()));
            }
        }
        row++;
        searchProgress->setValue(row);
    }
}

void DeviceTable::deviceDataTabRefreshSlot()
{
    windowConsole("刷新数据开始");
    windowConsole("刷新数据结束");
}

void DeviceTable::deviceDataTabExportSlot()
{
    windowConsole("导出数据开始");
    tableSaveToXlsx(*devTabModel, "设备数据表", deviceDataTabExportProgress);
    windowConsole("导出数据结束");
}

void DeviceTable::deviceDataTabImportSlot()
{
    DDB->foreachDevInfo();
}

void DeviceTable::upgradeRecordExportSlot()
{
    windowConsole("导出升级记录开始");
    tableSaveToXlsx(*upgradeRecordModel, "升级记录", upgradeRecordTabExportProgress);
    windowConsole("导出升级记录开始");
}

void DeviceTable::upgradeRecordImportSlot()
{
    upgradeRecordModel->clear();
    upgradeRecordModel->setColumnCount(upRecordItemTitle.length());
    for (int n = 0; n < upRecordItemTitle.length(); n++) {
        upgradeRecordModel->setHeaderData(n, Qt::Horizontal, upRecordItemTitle[n]);
    }
    DDB->foreachUpgradeRecord();
}

void DeviceTable::ipcfgRecordExportSlot()
{
    windowConsole("导出IP配置开始");
    tableSaveToXlsx(*IPConfigModel, "IP配置记录", IPConfigTabExportProgress);
    windowConsole("导出IP配置开始");
}

void DeviceTable::ipcfgRecordImportSlot()
{
    IPConfigModel->clear();
    IPConfigModel->setColumnCount(ipcfgItemTitle.length());
    for (int n = 0; n < ipcfgItemTitle.length(); n++) {
        IPConfigModel->setHeaderData(n, Qt::Horizontal, ipcfgItemTitle[n]);
    }
    DDB->foreachIPconfigRecord();
}

void DeviceTable::newUpgradeSlot(Devices::devInfo &data)
{
    Devices::upgradeRecord urData;
    Devices::devInfo olddata;

    if (devices->getInfo(data.sn, olddata)) {
        urData.old_ver = olddata.version;
        urData.remark = olddata.remark;
    }
    urData.sn = data.sn;
    urData.model = data.model;
    devices->getTargetVer(urData.sn, urData.new_ver);
    urData.state = Devices::deviceStatusText(data.state);
    urData.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    addUpRecordTableItem(urData);
    DDB->save(urData);
}

void DeviceTable::selectUpgradeRecord(Devices::upgradeRecord &data)
{
    addUpRecordTableItem(data);
}

void DeviceTable::selectIpcfgRecord(Devices::ipconfigRecord &data)
{
    addIpcfgRecordTableItem(data);
}
