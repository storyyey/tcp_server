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

class DeviceTable : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceTable(QWidget *parent = nullptr);
    void IPConfigDialogInit();
    void remarkDialogInit();
    void upgradeDialogInit();
    void searchDialogInit();
    void IPConfigAutoEdit();
    void deviceDataTableInit();
    void upgradeRecordTabInit();
    void IPConfigRecordTabInit();
    void tableSaveToXlsx(QStandardItemModel &tab, const QString &path, QProgressBar *proBar);
    void devicesInit(Devices *devices);
    void devicesDBInit(DeviceDB *ddb);
    void addDevicesTableItem(Devices::devInfo &data);
    void addUpRecordTableItem(Devices::upgradeRecord &data);
    void addIpcfgRecordTableItem(Devices::ipconfigRecord &data);
private:
    DeviceDB *DDB;
    Devices *devices;
    QTabWidget *tabWidget;

    QList<QString> devInfoItemTitle = {"状态", "上线时间", "设备IP", \
                                       "设备序列号", "软件版本", "设备型号",\
                                       "备注"};
    QWidget *deviceDataWidget;
    QStandardItemModel *devTabModel;
    QTableView *deviceDataTabView;
    QMenu *deviceConfigMenu;
    QPushButton *deviceDataTabRefreshBtn;
    QPushButton *deviceDataTabExportBtn;
    QPushButton *deviceDataTabImportBtn;
    QProgressBar *deviceDataTabExportProgress;

    QList<QString> upRecordItemTitle = {"序列号", "升级时间", "原始版本",\
                                        "目标版本", "型号", "状态", "备注"};
    QWidget *upgradeRecordWidget;
    QStandardItemModel *upgradeRecordModel;
    QTableView *upgradeRecordTabView;
    QPushButton *upgradeRecordTabExportBtn;
    QPushButton *upgradeRecordTabImportBtn;
    QProgressBar *upgradeRecordTabExportProgress;

    QList<QString> ipcfgItemTitle = {"序列号", "时间", "IP", "掩码", "网关", "备注"};
    QWidget *IPConfigRecordWidget;
    QStandardItemModel *IPConfigModel;
    QTableView *IPConfigTabView;
    QPushButton *IPConfigTabExportBtn;
    QPushButton *IPConfigTabImportBtn;
    QProgressBar *IPConfigTabExportProgress;
    QDialog *IPConfigDialog;
    QComboBox *IPmodelLBoBox;
    QLineEdit *IPConfigSN;
    QWidget *IPConfigWidget;
    QLineEdit *ipEdit[4];
    uint32_t IPConfigIP = 0;
    QLineEdit *IPConfigMaskN;
    uint8_t IPConfigMaskNum = 0;
    QLineEdit *maskEdit[4];
    uint32_t IPConfigNetMask = 0;
    QLineEdit *gatewayEdit[4];
    uint32_t IPConfigGateway = 0;
    QPushButton *IPConfigOKBtn;
    QPushButton *IPConfigCancleBtn;

    QDialog *upgradeDialog;
    QLineEdit *upgradeSN;
    QLineEdit *currVersion;
    QLineEdit *devModel;
    QLineEdit *targetVer;
    QPushButton *fileSelectBtn;
    QLineEdit *fileSelectEdit;
    QPushButton *upgradeAgreeBtn;
    QPushButton *upgradeCancelBtn;
    QFileDialog *fileSelectDialog;

    QDialog *remarkDialog;
    QLineEdit *remarkLineEdit;

    QDialog *searchDialog;
    QPushButton *searchBtn;
    QLineEdit *searchLineEdit;
    QStandardItemModel *searchModel;
    QTableView *searchTabView;
    QLabel *searchResult;
    QProgressBar *searchProgress;
    // TableSearch *searchThread;

    int selectRow;
    int selectColumn;

    const QString btnStyle = ("QPushButton{background-color: transparent; \
                                    color: rgb(255,255,255);\
                                    border:1px solid rgb(255,255,255);}"\
                              "QPushButton:hover{background-color: rgb(28,28,28);\
                                    border:1px solid rgb(28,28,28);}"\
                              "QPushButton:pressed{background-color: rgb(79,79,79);\
                                    border:1px solid rgb(255,255,255);}");

signals:

private slots:
    void contextMenuSlot(QPoint pos);
    void deviceInfoChange(Devices::devInfo &data);
    void selectDeviceInfoImport(Devices::devInfo &data);
    void upgradeMenuSlot();
    void IPConfigMenuSlot();
    void remarkMenuSlot();
    void searchMenuSlot();
    void IPModelChangeSlot(int index);
    void IPConfigOKSlot();
    void IPConfigCancelSlot();
    void IPconfigChangeSlot(const QString &text);
    void IPMaskNumberChangeSlot(const QString &text);
    void IPConfigGatewayChangeSlot(const QString &text);
    void IPNetMaskChangeSlot(const QString &text);
    void fileSelectPopDialogSlot();
    void upgradeAgreeBtnSlot();
    void upgradeCancelBtnSlot();
    void remarkChangeSlot(const QString &text);
    void deviceDataSearchBtnSlot();
    void deviceDataTabRefreshSlot();
    void deviceDataTabExportSlot();
    void deviceDataTabImportSlot();
    void upgradeRecordExportSlot();
    void upgradeRecordImportSlot();
    void ipcfgRecordExportSlot();
    void ipcfgRecordImportSlot();
    void newUpgradeSlot(Devices::devInfo &data);
    void selectUpgradeRecord(Devices::upgradeRecord &data);
    void selectIpcfgRecord(Devices::ipconfigRecord &data);
};


#endif // DEVICETABLE_H
