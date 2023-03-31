#include "DeviceData.h"
#include "mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>

QMap<QString, Devices::devInfo> Devices::SnToInfoMap;
QMap<QString, QQueue<QString> *> Devices::msgMap;
QMap<uint32_t, QString> Devices::ipToSnMap;
QString Devices::upgradeFilePath;
QMap<QString, uint32_t> Devices::upgradeTimerMap;
QMap<QString, QString> Devices::targetVer;
QString Devices::fileDownUrl;

bool Devices::setUpgrade(upgradeRecord &upval)
{
    QJsonObject upgradeCmd = QJsonObject {
        {
            {"command", "upgrade"},
            {"data",
                QJsonObject {
                    {"upgradable", "true"},
                    {"url",  getFileDownUrl() + "file=" + upval.md5},
                    {"version", upval.old_ver},
                    {"interval", 5},
                    {"model", upval.model},
                    {"md5", upval.md5},
                }
            },
        }
    };

    QJsonDocument doc;
    doc.setObject(upgradeCmd);
    windowConsole("添加等待发送的升级命令 --------->");
    windowConsole(doc.toJson());

    setStatus(upval.sn, Devices::upgradeWait);
    setTargetVer(upval.sn, upval.new_ver);
    setUpgradeFilePath(upval.filePath);
    addOperatorCmd(upval.sn, doc.toJson());
    addUpgradeRecord(upval.sn, Devices::upgradeWait, upval.old_ver, upval.new_ver, upval.remark);

    return true;
}

bool Devices::setAddress(ipconfigRecord &addr)
{
    QJsonObject ipconfigCmd = QJsonObject {
        {
            {"command", "ifconfig"},
            {"data",
                QJsonObject {
                    {"interface", "eth0"},
                    {"bootproto", addr.model},
                    {"address", addr.ip},
                    {"netmask", addr.netmask},
                    {"gateway", addr.gateway},
                }
            },
        }
    };
    QJsonDocument doc;
    doc.setObject(ipconfigCmd);
    windowConsole("添加等待发送的IP配置消息 --------->");
    windowConsole(doc.toJson());
    Devices::devInfo di;

    getInfo(addr.sn, di);
    addOperatorCmd(addr.sn, doc.toJson());
    addIpConfigRecord(addr.sn, addr.ip, addr.netmask, addr.gateway, di.remark);
    // NetworkTest::addAddress("以太网", addr.gateway, addr.netmask);

    return true;
}

bool Devices::setRemark(const QString &sn, const QString &remark)
{
    Devices::devInfo devinfo;

    if (getInfo(sn, devinfo) ||\
        DeviceDB::select(sn, devinfo)) {
        devinfo.remark = remark;
        addInfo(sn, devinfo);
        return true;
    }

    return false;
}

bool Devices::setReboot(const QString &sn)
{
    QJsonObject rebootCmd = QJsonObject {
        {
            {"command", "reboot"},
        }
    };
    QJsonDocument doc;
    doc.setObject(rebootCmd);

    setStatus(sn, Devices::waitReboot);
    addOperatorCmd(sn, doc.toJson());

    return true;
}
