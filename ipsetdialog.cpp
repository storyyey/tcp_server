#include "ipsetdialog.h"
#include <QIntValidator>
#include "mainwindow.h"

IPSetDialog::IPSetDialog()
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
                    width: 60px;\
                    height: 30px;\
                   }\
                   QComboBox:focus{\
                    border: 3px solid #63B8FF;\
                   }";

    thisStyle += "QDialog {\
                    background-color: #EDEDED;\
                   }";

    thisStyle += "QHeaderView::section {\
                    height: 36px;\
                   }"\
                  "QTreeWidget::item {\
                    height: 36px;\
                    width: 200px;\
                  }";
    thisStyle += "QTreeWidget {\
                   border: 3px solid #FFFFFF;\
                   border-radius: 4px;\
                 }";

    this->setStyleSheet(thisStyle);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    // mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setWindowTitle("设备IP配置");
    this->setLayout(mainLayout);
    this->setFixedWidth(780);
    //IPConfigDialog->setFixedSize(QSize(480, 240));
    QHBoxLayout *snLayout = new QHBoxLayout();
    QLabel *snLabel = new QLabel("序列号 :");
    //snLabel->setFixedWidth(50);
    snLayout->addWidget(snLabel);
    snEdit = new QLineEdit();
    snEdit->setText("123456789");
    snEdit->setEnabled(false);
    snLayout->addWidget(snEdit);
    mainLayout->addLayout(snLayout);

    QHBoxLayout *IPmodelLayout = new QHBoxLayout();
    QLabel *IPmodelLabel = new QLabel("IP模式 :");
    IPmodelLayout->addWidget(IPmodelLabel);
    addrModelSelectBox = new QComboBox();
    addrModelSelectBox->addItem("静态");
    addrModelSelectBox->addItem("动态");
    IPmodelLayout->addWidget(addrModelSelectBox);
    IPmodelLayout->addStretch();
    mainLayout->addLayout(IPmodelLayout);
    connect(addrModelSelectBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChange(int)));

    QRegExp regExp("((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})");
    QValidator *IPvalidator = new QRegExpValidator(regExp, this);
    addrEditWidget = new QWidget();
    mainLayout->addWidget(addrEditWidget);
    QGridLayout *IPConfigLayout = new QGridLayout();
    IPConfigLayout->setContentsMargins(0, 0, 0, 0);
    addrEditWidget->setLayout(IPConfigLayout);
    //QHBoxLayout *IPLayout = new QHBoxLayout();
    QLabel *IPLabel = new QLabel("IP/掩码:");
    //IPLabel->setFixedWidth(50);
    IPConfigLayout->addWidget(IPLabel, 2, 0);
    for (int n = 0; n < 4; n++) {
        addrEdit[n] = new QLineEdit();
        addrEdit[n]->setValidator(IPvalidator);
        addrEdit[n]->setPlaceholderText("0~255");
        IPConfigLayout->addWidget(addrEdit[n], 2, 1 + (2 * n));
        QString symbol(n < 3 ? "." : "/");
        IPConfigLayout->addWidget(new QLabel(symbol), 2, 2 + (2 * n));
        connect(addrEdit[n], SIGNAL(textEdited(QString)), this, SLOT(ipConfigChange(QString)));
    }

    QRegExp maskRegExp("([0-9]{1}|[1-2]{1}[0-9]{1}|3{1}[0-2]{1})");
    QValidator *MaskValidator = new QRegExpValidator(maskRegExp, this);
    addrMaskLengthEdit = new QLineEdit();
    addrMaskLengthEdit->setPlaceholderText("0~32");
    addrMaskLengthEdit->setValidator(MaskValidator);
    IPConfigLayout->addWidget(addrMaskLengthEdit, 2, 9);
    connect(addrMaskLengthEdit, SIGNAL(textEdited(QString)), this, SLOT(maskNumCahnge(QString)));

    QLabel *netMaskLabel = new QLabel("掩码   :");;
    IPConfigLayout->addWidget(netMaskLabel, 3, 0);
    for (int n = 0; n < 4; n++) {
        addrMaskEdit[n] = new QLineEdit();
        addrMaskEdit[n]->setValidator(IPvalidator);
        addrMaskEdit[n]->setPlaceholderText("0~255");
        IPConfigLayout->addWidget(addrMaskEdit[n], 3, 1 + (2 * n));
        QString symbol(n < 3 ? "." : " ");
        IPConfigLayout->addWidget(new QLabel(symbol), 3, 2 + (2 * n));
        connect(addrMaskEdit[n], SIGNAL(textEdited(QString)), this, SLOT(maskChange(QString)));
    }

    QLabel *gagtewayLabel = new QLabel("网关   :");
    IPConfigLayout->addWidget(gagtewayLabel, 4, 0);
    for (int n = 0; n < 4; n++) {
        addrGatewayEdit[n] = new QLineEdit();
        addrGatewayEdit[n]->setValidator(IPvalidator);
        addrGatewayEdit[n]->setPlaceholderText("0~255");
        IPConfigLayout->addWidget(addrGatewayEdit[n], 4, 1 + (2 * n));
        QString symbol(n < 3 ? "." : " ");
        IPConfigLayout->addWidget(new QLabel(symbol), 4, 2 + (2 * n));
        connect(addrGatewayEdit[n], SIGNAL(textEdited(QString)), this, SLOT(gatewayChange(QString)));
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();
    okBtn = new QPushButton("确定/配置");
    okBtn->setIcon(QIcon(":/icon/confirm.png"));
    okBtn->setFixedSize(150, 40);
    btnLayout->addWidget(okBtn);
    cancelBtn = new QPushButton("取消/关闭");
    cancelBtn->setIcon(QIcon(":/icon/cancel.png"));
    cancelBtn->setFixedSize(150, 40);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    presetAddrTabBtn = new QPushButton();
    presetAddrTabBtn->setIcon(QIcon(":/icon/down.png"));
    mainLayout->addWidget(presetAddrTabBtn);

    connect(okBtn, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(presetAddrTabBtn, SIGNAL(clicked()), this, SLOT(presetWidget()));

    QVBoxLayout *treeWidgetLayout = new QVBoxLayout();
    addrTreeWidget = new QTreeWidget();
    addrTreeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
    addrTreeWidget->setHeaderLabels(QStringList()<< "IP" << "掩码" << "网关" << "使用次数");
    mainLayout->addWidget(addrTreeWidget);
    addrTreeWidget->setLayout(treeWidgetLayout);
    connect(addrTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(checkTreeItem(QTreeWidgetItem*, int)));
}

void IPSetDialog::IPConfigAutoEdit()
{

}

void IPSetDialog::popUp()
{
    QList<Devices::presetIp> presetIps;

    windowConsole("弹出IP配置窗口");
    snEdit->setText(sn);
    addrTreeWidget->clear();
    presetIpClear();
    DeviceDB::select(presetIps);
    for (int n = 0; n < presetIps.size(); n++) {
        const Devices::presetIp &pi = presetIps.at(n);

        addLeafItem(pi.pos, QStringList() << \
                    QHostAddress(pi.ip).toString() <<\
                    QHostAddress(pi.mask).toString() <<\
                    QHostAddress(pi.gateway).toString() <<\
                    QString("%1").arg(pi.usen));
    }

    this->exec();
    windowConsole("关闭IP配置窗口");
}

void IPSetDialog::setSn(const QString &sn)
{
    this->sn = sn;
}

void IPSetDialog::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->globalPos();
    isPressedDialog = true;
}

void IPSetDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressedDialog) {
        this->move(this->x() + (event->globalX() - m_lastPos.x()),
                    this->y() + (event->globalY() - m_lastPos.y()));
        m_lastPos = event->globalPos();
    }
}

void IPSetDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_lastPos = event->globalPos();
    isPressedDialog = false;
}

void IPSetDialog::addTrunkItem(const QString &trunk)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(addrTreeWidget, QStringList(QString(trunk)));
    if (item) {
        trunkMap.insert(trunk, item);
    }
}

void IPSetDialog::addLeafItem(const QString &trunk, const QStringList &leaf)
{
    if (!trunkMap.contains(trunk)) {
        addTrunkItem(trunk);
    }
    if (trunkMap[trunk]) {
        QTreeWidgetItem *item = new QTreeWidgetItem(trunkMap[trunk], leaf);
        if (!leaf.contains("0")) {
            setTreeItemCaution(item);
        }
        trunkMap[trunk]->addChild(item);
    }
}

void IPSetDialog::setTreeItemCaution(QTreeWidgetItem *item)
{
    if (!item) {
        return ;
    }
    for (int n = 0; n < item->columnCount(); n++) {
        item->setBackground(n, QBrush(QColor("#ff6600")));
    }
}

void IPSetDialog::setTreeItemUseNum(QTreeWidgetItem *item, int n)
{
    if (!item || item->columnCount() < 4) {
        return ;
    }
    selectTreeItem->setText(3, QString("%1").arg(n));
}

void IPSetDialog::addrEditClear()
{
    int n = 0;

    for (n = 0; n < 4; n++) {
       addrEdit[n]->clear();
       addrMaskEdit[n]->clear();
       addrGatewayEdit[n]->clear();
    }
    addrMaskLengthEdit->clear();
}

QString IPSetDialog::getAddrString()
{
    QString ipString;

    for (int n = 0; n < 4; n++) {
        ipString += addrEdit[n]->text().size() ? addrEdit[n]->text() : "0";
        n < 3 ? ipString += "." : "";
    }

    return ipString;
}

QString IPSetDialog::getMaskString()
{
    QString maskString;

    for (int n = 0; n < 4; n++) {
        maskString += addrMaskEdit[n]->text().size() ? addrMaskEdit[n]->text() : "0";
        n < 3 ? maskString += "." : "";
    }

    return maskString;
}

QString IPSetDialog::getGatewayString()
{
    QString gatewayString;

    for (int n = 0; n < 4; n++) {
        gatewayString += addrGatewayEdit[n]->text().size() ? addrGatewayEdit[n]->text() : "0";
        n < 3 ? gatewayString += "." : "";
    }

    return gatewayString;
}

uint32_t IPSetDialog::getMaskLength()
{
    return addrMaskLengthEdit->text().size() ? addrMaskLengthEdit->text().toUInt() : 0;
}

QString IPSetDialog::gateway(const QString &addr, const QString &mask)
{
    QString gatewayString = "0.0.0.0";

    QPair<QHostAddress, int> pair;
    pair = QHostAddress::parseSubnet(addr + "/" + mask);
    if (pair.second >= 0) {
        gatewayString.clear();
        gatewayString = QHostAddress(pair.first.toIPv4Address() + 1).toString();
    }

    return gatewayString;
}

QString IPSetDialog::gateway(const QString &addr, const uint32_t mask)
{
    QString gatewayString = "0.0.0.0";

    QPair<QHostAddress, int> pair;
    pair = QHostAddress::parseSubnet(addr + "/" + QString("%1").arg(mask));
    if (pair.second >= 0) {
        gatewayString.clear();
        gatewayString = QHostAddress(pair.first.toIPv4Address() + 1).toString();
    }

    return gatewayString;
}

void IPSetDialog::gatewayAutoEdit(const QString &addr, const QString &mask)
{
    QString gatewayString = gateway(addr, mask);
    for (int n = 0; n < 4; n++) {
        addrGatewayEdit[n]->setText(gatewayString.split(".").at(n));
    }
}

void IPSetDialog::gatewayAutoEdit(const QString &addr, const uint32_t &mask)
{
    QString gatewayString = gateway(addr, mask);
    for (int n = 0; n < 4; n++) {
        addrGatewayEdit[n]->setText(gatewayString.split(".").at(n));
    }
}

void IPSetDialog::presetIpClear()
{
#if 0
    QMap<QString, QTreeWidgetItem *>::const_iterator tw = trunkMap.constBegin();
    while (tw != trunkMap.constEnd()) {
        delete tw.value();
        ++tw;
    }
#endif
    trunkMap.clear();
}

void IPSetDialog::modeChange(int index)
{
    if (index == 0) {
        addrEditWidget->show();
        addrTreeWidget->show();
        presetAddrTabBtn->setIcon(QIcon(":/icon/up.png"));
    }
    else if (index == 1) {
        addrEditWidget->hide();
        addrTreeWidget->hide();
        presetAddrTabBtn->setIcon(QIcon(":/icon/down.png"));
    }
}

void IPSetDialog::confirm()
{
    Devices::ipconfigRecord ipcfg;
    QMessageBox msgBox;

    QHostAddress ipAddr(getAddrString());
    QHostAddress gatewayAddr(getGatewayString());
    QHostAddress maskAddr(getMaskString());
    ipcfg.model = (addrModelSelectBox->currentText() == "静态" ? "static": "dhcp");

    if (ipcfg.model == "static" && (!ipAddr.toIPv4Address() ||\
        !gatewayAddr.toIPv4Address() ||\
        !maskAddr.toIPv4Address())) {
        QMessageBox::warning(NULL, "配置异常", "配置不全",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    QPair<QHostAddress, int> pair = \
            QHostAddress::parseSubnet(getAddrString() + "/" + getMaskString());
    if (pair.first.isNull() || pair.second < 0) {
        QMessageBox::warning(NULL, "配置异常", "掩码格式不正确",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }
    if (ipAddr.isInSubnet(gatewayAddr, pair.second) == false) {
        QMessageBox::warning(NULL, "配置异常", "网关地址不在子网内",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    if (QMessageBox::information(NULL, QString("配置确认:%1").arg(snEdit->text()), \
                                    QString("IP:<font color = blue><u>%1/%2</u></font><br>\
                                             掩码:<font color = blue><u>%3</u></font><br>\
                                             网关:<font color = blue><u>%4</u></font><br>").arg(getAddrString())\
                                    .arg(pair.second).arg(getMaskString()).arg(getGatewayString()),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) ==  QMessageBox::Yes) {

        ipcfg.sn = snEdit->text();
        ipcfg.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ipcfg.ip = getAddrString();
        ipcfg.netmask = getMaskString();
        ipcfg.gateway = getGatewayString();

        emit newIpConfigSignal(ipcfg);
        if (isUsePresetAddr && ipcfg.model == "static") {
            presetAddrVal.usen++;
            DeviceDB::update(presetAddrVal);
            setTreeItemUseNum(selectTreeItem, presetAddrVal.usen);
            setTreeItemCaution(selectTreeItem);
        }
        this->close();
    }
}

void IPSetDialog::cancel()
{
    this->close();
}

void IPSetDialog::ipConfigChange(const QString &text)
{
    text.size();
    isUsePresetAddr = false;
    gatewayAutoEdit(getAddrString(), getMaskString());
}

void IPSetDialog::maskNumCahnge(const QString &text)
{
    isUsePresetAddr = false;
    gatewayAutoEdit(getAddrString(), text.toInt());

    uint32_t maskVal = 0;
    for (int n = 0; n < text.toInt(); n++) {
        maskVal |= 0x80000000 >> n;
    }
    for (int n = 0; n < 4; n++) {
        addrMaskEdit[n]->setText(QHostAddress(maskVal).toString().split(".").at(n));
    }
}

void IPSetDialog::gatewayChange(const QString &text)
{
    text.size();
    isUsePresetAddr = false;
}

void IPSetDialog::maskChange(const QString &text)
{
    text.size();
    isUsePresetAddr = false;
    QPair<QHostAddress, int> pair = \
            QHostAddress::parseSubnet(getAddrString() + "/" + getMaskString());
    if (pair.second > 0) {
        addrMaskLengthEdit->setText(QString("%1").arg(pair.second));
    }
    gatewayAutoEdit(getAddrString(), getMaskString());
}

void IPSetDialog::presetWidget()
{
    if (addrTreeWidget->isHidden()) {
        addrTreeWidget->show();
        presetAddrTabBtn->setIcon(QIcon(":/icon/up.png"));
    }
    else {
        addrTreeWidget->hide();
        presetAddrTabBtn->setIcon(QIcon(":/icon/down.png"));
    }
}

void IPSetDialog::checkTreeItem(QTreeWidgetItem *item, int column)
{
    selectTreeItem = NULL;
    isUsePresetAddr = false;
    if (!item->text(0).size() ||\
        !item->text(1).size() ||\
        !item->text(2).size() ||\
        !item->text(3).size()) {
        return ;
    }

    presetAddrVal.pos = item->parent()->text(0);
    presetAddrVal.ip = QHostAddress(item->text(0)).toIPv4Address();
    for (int n = 0; n < 4; n++) {
        addrEdit[n]->setText(item->text(0).split(".").at(n));
    }
    presetAddrVal.mask = QHostAddress(item->text(1)).toIPv4Address();
    for (int n = 0; n < 4; n++) {
        addrMaskEdit[n]->setText(item->text(1).split(".").at(n));
    }
    presetAddrVal.gateway = QHostAddress(item->text(2)).toIPv4Address();
    for (int n = 0; n < 4; n++) {
        addrGatewayEdit[n]->setText(item->text(2).split(".").at(n));
    }
    QPair<QHostAddress, int> pair = \
            QHostAddress::parseSubnet(item->text(0) + "/" + item->text(1));
    addrMaskLengthEdit->setText(QString("%1").arg(pair.second));

    presetAddrVal.usen = item->text(3).toUInt();
    if (presetAddrVal.usen) {
        switch (QMessageBox::warning(NULL, tr("确认"), \
                                     tr("该IP已被使用，是否继续使用？"),\
                                     tr("是"), tr("否"), 0, 1)) {
          case 1: addrEditClear(); return ;
          case 0: /* ignore */
          default: break;
        }
    }
    isUsePresetAddr = true;
    selectTreeItem = item;
}

