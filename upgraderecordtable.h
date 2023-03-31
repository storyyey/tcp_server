#ifndef UPGRADERECORDTABLE_H
#define UPGRADERECORDTABLE_H

#include <QWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include <QProgressBar>
#include <QTableView>
#include <QLayout>
#include <QHeaderView>
#include "devicedb.h"
#include "DeviceData.h"
#include "operatexlsx.h"
#include "searchdialog.h"

class UpgradeRecordTable : public QWidget
{
    Q_OBJECT
public:
    explicit UpgradeRecordTable(QWidget *parent = nullptr);
    void additem(const Devices::upgradeRecord &data);
private:
    DeviceDB *DDB;
    QList<QString> upRecordItemTitle = {"序列号", "升级时间", "原始版本",\
                                        "目标版本", "型号", "状态", "备注"};
    QStandardItemModel *tabModel;
    QTableView *tabView;
    QPushButton *exportBtn;
    QPushButton *importBtn;
    QProgressBar *progress;
    QMenu *rmenu;
    int selectRow;
    int selectColumn;
    SearchDialog *searchDialog;
signals:

private slots:
    void exportSlot();
    void importSlot();
    void newUpgradeRecordSlot(QString &sn, Devices::deviceState state, QString &oldVer, QString &newVer, QString &model, QString &remark);
    void contextMenuSlot(QPoint pos);
    void searchMenuSlot();
};

#endif // UPGRADERECORDTABLE_H
