#ifndef DEVICESTABLE_H
#define DEVICESTABLE_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QProgressBar>
#include <QHeaderView>
#include <QLayout>
#include "DeviceData.h"
#include "devicestablemenu.h"
#include "devicedb.h"
#include "operatexlsx.h"
#include "otherdialog.h"

class DevicesTable : public QWidget
{
    Q_OBJECT
public:
    explicit DevicesTable(QWidget *parent = nullptr);
    void addItem(const Devices::devInfo &data);
    int findSnColumn();
    void setStateIcon(QStandardItem *item, Devices::deviceState state);
private:
    OtherDialog *otherDialog;
    QStringList tabItemTitle = {"状态", "上线时间", "IP", \
                                "序列号", "固件版本", "型号",\
                                "备注"};
    QStandardItemModel *tabModel;
    QTableView *tabView;
    QMenu *deviceConfigMenu;
    QPushButton *refreshBtn;
    QPushButton *exportBtn;
    QPushButton *importBtn;
    QPushButton *otherBtn;
    QProgressBar *exportProgress;

    DevicesTableMenu *operMenu;
    int selectRow;
    int selectColumn;
    int snInCloumn = 0;

    struct flickerItem {
        int statusLevel;
        QList<QStandardItem *> Item;
    };
    QQueue<QList<QStandardItem *>> flicker;
    QQueue<flickerItem> flickerRun;
    void insertFlickerItem(QList<QStandardItem *> &item);
signals:

private slots:
    void contextMenuSlot(QPoint pos);
    void newIpConfigSolt(Devices::ipconfigRecord &val);
    void newRemarkSolt(const QString &val);
    void newUpgradeSolt(Devices::upgradeRecord &val);
    void deviceInfoChange(Devices::devInfo &data);
    void historyImport();
    void tableExport();
    void tableRefresh();
    void otherFunc();
};

#endif // DEVICESTABLE_H
