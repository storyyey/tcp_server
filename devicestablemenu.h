#ifndef DEVICESTABLEMENU_H
#define DEVICESTABLEMENU_H

#include <QMenu>
#include "upgradedialog.h"
#include "ipsetdialog.h"
#include "searchdialog.h"
#include "remarkdialog.h"
#include "DeviceData.h"
#include "devicedb.h"

class DevicesTableMenu : public QMenu
{
    Q_OBJECT
public:
    DevicesTableMenu();
    ~DevicesTableMenu();
    void setSearchColumn(int column);
    void setSearchModel(QStandardItemModel *model);
    void setTargetModel(QStandardItemModel *model);
    void setSelectPos(int row, int column);
    void setSearchHint(const QString &hint);
    void setUpgradeHint(const QString &hint);
    void setRemarkHint(const QString &hint);
    void setRebootHint(const QString &hint);
    void setAddrConfigHint(const QString &hint);
private:
    QAction *upgradeAction;
    QAction *remarkAction;
    QAction *addrCfgAction;
    QAction *rebootAction;
    QAction *searchAction;
    IPSetDialog *ipsetDialog;
    UpgradeDialog *upgradeDialog;
    SearchDialog *searchDialog;
    RemarkDialog *remarkDialog;
    QStandardItemModel *targetModel;
    int selectRow;
    int selectColumn;

signals:
    void newIpConfigSignal(Devices::ipconfigRecord &val);
    void newRemarkSignal(const QString &val);
    void newUpgradeSignal(Devices::upgradeRecord &val);

private slots:
    void upgradeMenuSlot();
    void IPConfigMenuSlot();
    void remarkMenuSlot();
    void searchMenuSlot();
};

#endif // DEVICESTABLEMENU_H
