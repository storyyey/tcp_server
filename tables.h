#ifndef DEVICETABLE_H
#define DEVICETABLE_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QMap>
#include <QMenu>
#include <QGridLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QRegExp>
#include <QMessageBox>
#include <QFileDialog>
#include "DeviceData.h"
#include "tablesearch.h"
#include "devicedb.h"
#include "devicestable.h"
#include "upgraderecordtable.h"
#include "ipconfigrecordtable.h"

class Tables : public QWidget
{
    Q_OBJECT
public:
    explicit Tables(QWidget *parent = nullptr);
    void widgetInit();
    void tableSaveToXlsx(QStandardItemModel &tab, const QString &path, QProgressBar *proBar);
private:
    DevicesTable *devicesTab;
    UpgradeRecordTable *urTable;
    IpConfigRecordTable *icTable;
    QTabWidget *tabWidget;

signals:

private slots:


};


#endif // DEVICETABLE_H
