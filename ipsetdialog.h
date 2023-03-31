#ifndef IPSETDIALOG_H
#define IPSETDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHostAddress>
#include <QMessageBox>
#include <QTreeWidget>
#include "DeviceData.h"
#include "devicedb.h"

class IPSetDialog : public QDialog
{
    Q_OBJECT
public:
    IPSetDialog();
    void IPConfigAutoEdit();
    void popUp();
    void setSn(const QString &sn);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void addTrunkItem(const QString &trunk);
    void addLeafItem(const QString &trunk, const QStringList &leaf);
    void setTreeItemCaution(QTreeWidgetItem *item);
    void setTreeItemUseNum(QTreeWidgetItem *item, int n);
    void addrEditClear();
    QString getAddrString();
    QString getMaskString();
    QString getGatewayString();
    uint32_t getMaskLength();
    QString gateway(const QString &addr, const QString &mask);
    QString gateway(const QString &addr, const uint32_t mask);
    void gatewayAutoEdit(const QString &addr, const QString &mask);
    void gatewayAutoEdit(const QString &addr, const uint32_t &mask);
    void presetIpClear();
private:
    bool isPressedDialog;
    QPoint m_lastPos;
    QString sn;
    QComboBox *addrModelSelectBox;
    QLineEdit *snEdit;
    QWidget *addrEditWidget;
    QLineEdit *addrEdit[4];
    QLineEdit *addrMaskLengthEdit;
    QLineEdit *addrMaskEdit[4];
    QLineEdit *addrGatewayEdit[4];
    QPushButton *okBtn;
    QPushButton *cancelBtn;
    QPushButton *presetAddrTabBtn;
    QTreeWidget *addrTreeWidget;
    QMap<QString, QTreeWidgetItem *> trunkMap;
    Devices::presetIp presetAddrVal;
    bool isUsePresetAddr;
    QTreeWidgetItem *selectTreeItem;
private slots:
    void modeChange(int index);
    void confirm();
    void cancel();
    void ipConfigChange(const QString &text);
    void maskNumCahnge(const QString &text);
    void gatewayChange(const QString &text);
    void maskChange(const QString &text);
    void presetWidget();
    void checkTreeItem(QTreeWidgetItem* ,int);
signals:
    void newIpConfigSignal(Devices::ipconfigRecord &val);
};

#endif // IPSETDIALOG_H
