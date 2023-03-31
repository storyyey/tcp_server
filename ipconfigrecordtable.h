#ifndef IPCONFIGRECORDTABLE_H
#define IPCONFIGRECORDTABLE_H

#include <QWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include <QProgressBar>
#include <QTableView>
#include <QLayout>
#include <QHeaderView>
#include "devicedb.h"
#include "operatexlsx.h"
#include "searchdialog.h"
#include "networktest.h"

class IpConfigRecordTable : public QWidget
{
    Q_OBJECT
public:
    explicit IpConfigRecordTable(QWidget *parent = nullptr);
    ~IpConfigRecordTable();
    void addItem(const Devices::ipconfigRecord &data);
private:
    QList<QString> ipcfgItemTitle = {"序列号", "时间", "IP", "掩码", "网关", "备注"};
    QStandardItemModel *tabModel;
    QTableView *tabView;
    QPushButton *exportBtn;
    QPushButton *importBtn;
    QProgressBar *progress;
    QMenu *rmenu;
    int selectRow;
    int selectColumn;
    SearchDialog *searchDialog;
    NetworkTest *netTest;
signals:

private slots:
    void ipcfgRecordExportSlot();
    void ipcfgRecordImportSlot();
    void newIpconfigRecordSlot(QString &sn, QString &ip, QString &mask, QString &gateway, QString &remark);
    void contextMenuSlot(QPoint pos);
    void searchMenuSlot();
};

#endif // IPCONFIGRECORDTABLE_H
