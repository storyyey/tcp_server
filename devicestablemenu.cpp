#include "devicestablemenu.h"
#include "mainwindow.h"

DevicesTableMenu::DevicesTableMenu()
{
    QString style = "QMenu {\
                      border: 3px ;\
                      border-radius: 10px;\
                    }";

    this->setStyleSheet(style);
    upgradeAction = new QAction("升级固件");
    upgradeAction->setShortcut(tr("Ctrl+u"));
    upgradeAction->setIcon(QIcon(":/icon/uptab.png"));

    remarkAction = new QAction("备注");
    remarkAction->setShortcut(tr("Ctrl+r"));
    remarkAction->setIcon(QIcon(":/icon/remark.png"));

    addrCfgAction = new QAction("地址配置");
    addrCfgAction->setShortcut(tr("Ctrl+a"));
    addrCfgAction->setIcon(QIcon(":/icon/networkset.png"));

    rebootAction = new QAction("重启设备");
    rebootAction->setShortcut(tr("Ctrl+b"));
    rebootAction->setIcon(QIcon(":/icon/reboot.png"));

    searchAction = new QAction("查找");
    searchAction->setShortcut(tr("Ctrl+c"));
    searchAction->setIcon(QIcon(":/icon/search.png"));

    this->addAction(upgradeAction);
    this->addAction(addrCfgAction);
    this->addAction(rebootAction);
    this->addSeparator();
    this->addAction(remarkAction);
    this->addAction(searchAction);

    ipsetDialog = new IPSetDialog();
    upgradeDialog = new UpgradeDialog();
    searchDialog = new SearchDialog();
    remarkDialog = new RemarkDialog();

    connect(upgradeAction, SIGNAL(triggered()), this, SLOT(upgradeMenuSlot()));
    connect(addrCfgAction, SIGNAL(triggered()), this, SLOT(IPConfigMenuSlot()));
    connect(remarkAction, SIGNAL(triggered()), this, SLOT(remarkMenuSlot()));
    connect(searchAction, SIGNAL(triggered()), this, SLOT(searchMenuSlot()));
    connect(remarkDialog, &RemarkDialog::remarkEdited, this, \
            [this](const QString &text){emit newRemarkSignal(text);});
    connect(ipsetDialog, &IPSetDialog::newIpConfigSignal, this,\
            [this](Devices::ipconfigRecord &val) {emit newIpConfigSignal(val);});
    connect(upgradeDialog, &UpgradeDialog::newUpgradeSignal, this, \
            [this](Devices::upgradeRecord &val){emit newUpgradeSignal(val);});

    connect(rebootAction, &QAction::triggered, this, [this]{
        if (targetModel) {
            QStandardItem *item = targetModel->item(selectRow, 3);
            if (item) {
                if (Devices::isOffline(item->text())) {
                    QMessageBox::warning(NULL, "设备重启", \
                                         QString("设备(<font color = red>%1</font>)离线无需重启！").arg(item->text()),
                                         tr("确认"));
                    return ;
                }

                switch (QMessageBox::information(this, tr("设备重启"), \
                        QString("重启设备(<font color = red>%1</font>)？").arg(item->text()), tr("是"), tr("否"), 0, 1 )) {
                  case 0:
                       break;
                  case 1:
                  default:
                      return ;
                      break;
                }
                MainWindow::devices->setReboot(item->text());
            }
        }
    });
}

DevicesTableMenu::~DevicesTableMenu()
{
    delete ipsetDialog;
    delete upgradeDialog;
    delete searchDialog;
    delete remarkDialog;
}

void DevicesTableMenu::setSearchColumn(int column)
{
    searchDialog->setSearchColumn(column);
}

void DevicesTableMenu::setSearchModel(QStandardItemModel *model)
{
    searchDialog->setSearchModel(model);
}

void DevicesTableMenu::setTargetModel(QStandardItemModel *model)
{
    this->targetModel = model;
    searchDialog->setSearchModel(model);
}

void DevicesTableMenu::setSelectPos(int row, int column)
{
    this->selectRow = row;
    this->selectColumn = column;
    if (targetModel) {
        QStandardItem *item = targetModel->item(selectRow, 3);
        if (item) {
            setUpgradeHint(item->text());
            setRebootHint(item->text());
            setRemarkHint(item->text());
            setAddrConfigHint(item->text());
        }
        setSearchHint(targetModel->horizontalHeaderItem(selectColumn)->text());
    }
}

void DevicesTableMenu::setSearchHint(const QString &hint)
{
    searchAction->setText("查找(" + hint + ")");
}

void DevicesTableMenu::setUpgradeHint(const QString &hint)
{
    upgradeAction->setToolTip("升级固件(" + hint + ")");
}

void DevicesTableMenu::setRemarkHint(const QString &hint)
{
    remarkAction->setToolTip("备注(" + hint + ")");
}

void DevicesTableMenu::setRebootHint(const QString &hint)
{
    rebootAction->setToolTip("重启(" + hint + ")");
}

void DevicesTableMenu::setAddrConfigHint(const QString &hint)
{
    addrCfgAction->setToolTip("地址配置(" + hint + ")");
}

void DevicesTableMenu::upgradeMenuSlot()
{
    QStandardItem *item = targetModel->item(selectRow, 3);
    if (item) {
        upgradeDialog->setSn(item->text());
        if (Devices::isOffline(item->text())) {
            QMessageBox::warning(NULL, "设备升级", \
                                 QString("设备(<font color = red>%1</font>)离线无法升级！").arg(item->text()),
                                 tr("确认"));
            return ;
        }
    }
    item = targetModel->item(selectRow, 4);
    if (item) {
       upgradeDialog->setCurrVersion(item->text());
    }
    item = targetModel->item(selectRow, 5);
    if (item) {
        upgradeDialog->setModel(item->text());
    }

    upgradeDialog->popUp();
}

void DevicesTableMenu::IPConfigMenuSlot()
{
    QStandardItem *item = targetModel->item(selectRow, 3);
    if (item) {
        ipsetDialog->setSn(item->text());
        if (Devices::isOffline(item->text())) {
            QMessageBox::warning(NULL, " 设备地址配置",\
                                 QString("设备(<font color = red>%1</font>)离线无法配置地址！").arg(item->text()),
                                 tr("确认"));
            return ;
        }
    }
    ipsetDialog->popUp();
}

void DevicesTableMenu::remarkMenuSlot()
{
    QStandardItem *item = targetModel->item(selectRow, 3);
    if (item) {
        remarkDialog->setSn(item->text());
    }

    item = targetModel->item(selectRow, 6);
    if (item) {
        remarkDialog->setDefaultRemark(item->text());
    }

    remarkDialog->popUp();
}

void DevicesTableMenu::searchMenuSlot()
{
    searchDialog->setSearchColumn(selectColumn);
    searchDialog->popUp();
}
