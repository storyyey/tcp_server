#ifndef OTHERDIALOG_H
#define OTHERDIALOG_H

#include <QDialog>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QStackedWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QHostAddress>
#include <QComboBox>
#include "DeviceData.h"
#include "devicedb.h"

class OtherDialog : public QDialog
{
    Q_OBJECT
public:
    OtherDialog();
    void setdb(DeviceDB *db);
    void uploadFileWdiget();
    void presetIpWidget();
    void addPresetPosBoxItem(const QString &val);
private:
    QListWidget *listWidget;
    QStackedWidget *mainStackWidget;
    QHBoxLayout *mainLayout;
    QPushButton *fileSelectBtn;
    QLineEdit *fileSelectEdit;
    QPushButton *upgradeAgreeBtn;
    QPushButton *upgradeCancelBtn;
    QFileDialog *fileSelectDialog;
    QLineEdit *fileMd5Edit;
    QLineEdit *fileVersionEdit;
    QLineEdit *fileModelEdit;
    QLineEdit *fileRemarkEdit;
    QPushButton *confirmBtn;
    QPushButton *cancelBtn;
    QString fileFull;
    Devices::fileStore fileStore;

    QStringList boxItemList;
    QComboBox *presetPosBox;
    QLineEdit *presetIp;
    QLineEdit *presetMask;
    QLineEdit *presetGateway;
    QLineEdit *presetPos;
    QPushButton *preseTconfirmBtn;
    QPushButton *preseTcancelBtn;
private slots:
    void fileSelectPopDialogSlot();
    void confirmUpload();
    void cancelUpload();
    void preseTconfirm();
    void preseTcancel();
};

#endif // OTHERDIALOG_H
