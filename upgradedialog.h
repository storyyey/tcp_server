#ifndef UPGRADEDIALOG_H
#define UPGRADEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QStandardItemModel>
#include <QTableView>
#include "DeviceData.h"
#include "devicedb.h"

class UpgradeDialog : public QDialog
{
    Q_OBJECT
public:
    UpgradeDialog();
    void popUp();
    void setModel(const QString &model);
    void setSn(const QString &sn);
    void setCurrVersion(const QString &ver);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void lineEditClear();
    void appendFile(const Devices::fileStore &val);
private:
    bool isPressedDialog;
    QPoint m_lastPos;
    QString filePath;
    QLineEdit *upgradeSN;
    QLineEdit *currVersion;
    QLineEdit *devModel;
    QLineEdit *targetVer;
    QPushButton *fileSelectBtn;
    QLineEdit *fileSelectEdit;
    QLineEdit *md5Edit;
    QLineEdit *uptimeEdit;
    QLineEdit *sizeEdit;
    QLineEdit *remarkEdit;
    QPushButton *upgradeAgreeBtn;
    QPushButton *upgradeCancelBtn;
    Devices::upgradeRecord updata;
    QMenu *rightMenu;

    QStringList fileInfo = {"上传时间", "文件名", "MD5", "大小",\
                              "版本", "型号", "备注"};

    QStandardItemModel *fileodel;
    QTableView *fileView;

    const QString btnStyle = ("QPushButton{background-color: transparent; \
                                    color: rgb(255,255,255);\
                                    border:1px solid rgb(255,255,255);}"\
                              "QPushButton:hover{background-color: rgb(28,28,28);\
                                    border:1px solid rgb(28,28,28);}"\
                              "QPushButton:pressed{background-color: rgb(79,79,79);\
                                    border:1px solid rgb(255,255,255);}");
private slots:
    void fileSelectPopDialogSlot();
    void upgradeAgreeBtnSlot();
    void upgradeCancelBtnSlot();
    void tableSelectRow(const QModelIndex &index);
signals:
    void newUpgradeSignal(Devices::upgradeRecord &val);
};

#endif // UPGRADEDIALOG_H
