#ifndef DEVICEDATA_H
#define DEVICEDATA_H

#include <QString>
#include <QMap>
#include <QQueue>
#include <QHostAddress>
#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QColor>

#define upgradeThresholdFloor (20)
#define upgradeThresholdUpper (200)

class Devices : public QObject {
    Q_OBJECT

public:
    enum deviceState {
        offline = 0,
        online = 1,
        upgradeWait = 2,
        upgradeInProgress = 3,
        upgradeFinish = 4,
        upgradeFailed = 5,
        upgradeSuccess = 6,
        waitReboot = 7,
        rebootInProgress = 8,
        deviceStatusEnd
    };
    struct devInfo {
        QString sn;
        QString time;
        QString model;
        QString version;
        QString ip;
        QString remark;
        uint32_t interval;
        enum deviceState state;
    };

    struct ipconfigRecord {
        QString sn;
        QString time;
        QString ip;
        QString netmask;
        QString gateway;
        QString version;
        QString model;
        QString remark;
    };

    struct upgradeRecord {
        QString sn;
        QString time;
        QString old_ver;
        QString new_ver;
        QString model;
        QString state;
        QString remark;
        QString md5;
        QString file;
        QString filePath;
    };

    struct fileStore {
        QString md5;
        QString name;
        QString time;
        QByteArray data;
        uint32_t size;
        QString version;
        QString model;
        QString remark;
    };

    struct presetIp {
        QString pos;
        uint32_t ip;
        uint32_t mask;
        uint32_t gateway;
        uint32_t usen;
    };
private:
    static QMap<QString, struct devInfo> SnToInfoMap;
    static QMap<QString, QQueue<QString> *> msgMap;
    static QMap<uint32_t, QString> ipToSnMap;
    static QMap<QString, uint32_t> upgradeTimerMap;
    static QMap<QString, QString> targetVer;
    QTimer *uptimer;
    static QString upgradeFilePath;
    static QString fileDownUrl;
    QTimer *sfTimer;
public:
    bool setUpgrade(Devices::upgradeRecord &upval);
    bool setAddress(Devices::ipconfigRecord &addr);
    bool setRemark(const QString &sn, const QString &remark);
    bool setReboot(const QString &sn);

    bool addInfo(const QString &sn, struct devInfo &val) {
        if (sn.size() <= 0) {
            return false;
        }

        SnToInfoMap.insert(sn, val);
        if (val.ip.size() > 0) {
            ipToSnMap.insert(QHostAddress(val.ip).toIPv4Address(), sn);
        }
        emit infoUpdate(val);

        return true;
    };

    static bool getInfo(const QString &sn, struct devInfo &val) {
        if (SnToInfoMap.contains(sn)) {
            val = SnToInfoMap[sn];
            return true;
        }
        else {
            return false;
        }
    };

    static bool getInfo(uint32_t ip, struct devInfo &val) {
        if (!ipToSnMap.contains(ip)) {
            return false;
        }
        return getInfo(ipToSnMap[ip], val);
    };

    static bool addOperatorCmd(const QString &sn, const QString &msg) {
        if (sn.size() <= 0 || msg.size() <= 0) {
            return false;
        }

        if (msgMap.contains(sn)) {
            QQueue<QString> *queue = msgMap.value(sn);
            if (queue) {
                queue->enqueue(msg);
            }
        }
        else {
            QQueue<QString> *queue = new QQueue<QString>();
            if (queue) {
                queue->enqueue(msg);
                msgMap.insert(sn, queue);
            }
        }

        return true;
    }

    static const QString popOperatorCmd(const QString &sn) {
        if (msgMap.contains(sn)) {
            QQueue<QString> *queue = msgMap.value(sn);
            if (queue && !queue->isEmpty()) {
                return queue->dequeue();
            }
            else {
                return QString("");
            }
        }

        return "";
    }

    static void setUpgradeFilePath(const QString &path) {
        upgradeFilePath = path;
    }

    static QString getUpgradeFilePath() {
        return upgradeFilePath;
    }

    bool setStatus(const QString &sn, enum deviceState state) {
        if (!SnToInfoMap.contains(sn)) {
            return false;
        }
        SnToInfoMap[sn].state = state;
        if (state == upgradeInProgress) {
            reloadUpgradeTimer(sn);
        }
        else {
            removeUpgradeTimer(sn);
        }
        emit infoUpdate(SnToInfoMap[sn]);

        return true;
    }

    bool setStatus(uint32_t ip, enum deviceState state) {
        if (!ipToSnMap.contains(ip)) {
            return false;
        }

        setStatus(ipToSnMap[ip], state);

        return true;
    }

    static enum deviceState status(const QString &sn) {
        return SnToInfoMap.contains(sn) ? SnToInfoMap[sn].state : offline;
    }

    static enum deviceState status(uint32_t ip) {
        return ipToSnMap.contains(ip) ? status(ipToSnMap[ip]) : offline;
    }

    bool reloadUpgradeTimer(const QString &sn) {
        if (upgradeTimerMap.contains(sn)) {
            upgradeTimerMap[sn] = 0;
        }
        else {
            upgradeTimerMap.insert(sn, 0);
        }

        return true;
    }

    void removeUpgradeTimer(const QString &sn) {
        if (upgradeTimerMap.contains(sn)) {
            upgradeTimerMap.remove(sn);
        }
    }

    static uint32_t UpgradeTimerValue(const QString &sn) {
        if (upgradeTimerMap.contains(sn)) {
            return upgradeTimerMap[sn];
        }
        else {
            return 0;
        }
    }

    static const QString statusText(enum deviceState state) {
        if (state >= deviceStatusEnd) {
            return "未知";
        }

        const char *statusText[deviceStatusEnd] = {0};
        statusText[offline] = "离线";
        statusText[online] = "在线";
        statusText[upgradeWait] = "等待升级";
        statusText[upgradeInProgress] = "升级中";
        statusText[upgradeFinish] = "升级结束";
        statusText[upgradeFailed] = "升级失败";
        statusText[upgradeSuccess] = "升级成功";
        statusText[waitReboot] = "等待重启";
        statusText[rebootInProgress] = "重启中";

        return statusText[state] ? statusText[state] : "未知";
    }

    static QColor stateColor(enum deviceState state) {
        const char *color = "#FFFFFF";

        switch (state) {
            case Devices::upgradeInProgress:
                color = "#66ffcc";
                break;
            case Devices::upgradeSuccess:
                color = "#43CD80";
                break;
            case Devices::upgradeWait:
                color = "#66ffff";
                break;
            case Devices::upgradeFinish:
                color = "#66ff99";
                break;
            case Devices::offline:
                color = "#FFFFFF";
                break;
            case Devices::online:
                color = "#00FF00";
                break;
            case Devices::upgradeFailed:
                color = "#CD2626";
                break;
            case Devices::waitReboot:
                color = "#FFFFE0";
                break;
            case Devices::rebootInProgress:
                color = "#FFFACD";
                break;
            case Devices::deviceStatusEnd:
                break;
        }
        return QColor(color);
    }

    static void setFileDownUrl(const QString &url) {
        fileDownUrl = url;
    }

    static QString getFileDownUrl() {
        return fileDownUrl;
    }

    static bool isOffline(const QString &sn) {
        if (status(sn) == online ||\
            status(sn) == upgradeWait ||\
            status(sn) == upgradeFinish ||\
            status(sn) == waitReboot ||\
            status(sn) == upgradeSuccess) {

            return false;
        }
        else {
            return true;
        }
    }

    static bool isOnline(const QString &sn) {
        return !!!isOffline(sn);
    }

    bool setTargetVer(QString &sn, QString &ver) {
        targetVer.insert(sn, ver);
        return true;
    }

    static bool getTargetVer(QString &sn, QString &ver) {
        if (!targetVer.contains(sn)) {
            return false;
        }
        else {
            ver = targetVer[sn];
            return true;
        }
    }

    void addUpgradeRecord(QString &sn, Devices::deviceState state, QString &oldVer, QString &newVer, QString &remark) {
        struct devInfo val;

        getInfo(sn, val);
        emit newUpgradeRecord(sn, state, oldVer, newVer, val.model, remark);
    }

    void addIpConfigRecord(QString &sn, QString &ip, QString &mask, QString &gateway, QString &remark) {
        emit newIpconfigRecord(sn, ip, mask, gateway, remark);
    }

    static QStringList devTypeList() {
        return QStringList() << "LWG-1000" << "LWG-3000" << "LWG-5000";
    }

    Devices() {
        uptimer = new QTimer();
        connect(uptimer, SIGNAL(timeout()), this, SLOT(upgradeTimerUpload()));
        uptimer->setInterval(1000);
        uptimer->start();

        sfTimer = new QTimer();
        connect(sfTimer, SIGNAL(timeout()), this, SLOT(autoRefreshStatus()));
        sfTimer->setInterval(10000);
        sfTimer->start();
    }

    ~Devices() {
        delete sfTimer;
        delete uptimer;
    }

signals:
    void infoUpdate(Devices::devInfo &data);
    void newUpgradeRecord(QString &sn, Devices::deviceState state, QString &oldVer, QString &newVer, QString &model, QString &remark);
    void newIpconfigRecord(QString &sn, QString &ip, QString &mask, QString &gateway, QString &remark);

private slots:
    void upgradeTimerUpload() {
        QMap<QString, uint32_t> ::iterator iter = upgradeTimerMap.begin();
        while (iter != upgradeTimerMap.end()) {
            iter.value()++;

            struct devInfo val;
            if (getInfo(iter.key(), val) == true) {
                addInfo(iter.key(), val);
            }
            iter++;
        }
    }
    void autoRefreshStatus() {
        QMap<QString, devInfo>::iterator iter = SnToInfoMap.begin();
        while (iter != SnToInfoMap.end()) {
            QDateTime time = QDateTime::fromString(iter.value().time, "yyyy-MM-dd hh:mm:ss");
            if (time.isValid()) {
               if (iter.value().state == upgradeInProgress || \
                   iter.value().state == upgradeWait) {
                   if (time.secsTo(QDateTime::currentDateTime()) > iter.value().interval + 300) {
                        setStatus(iter.value().sn, upgradeFailed);
                   }
               }
               else {
                   if (time.secsTo(QDateTime::currentDateTime()) > iter.value().interval) {
                        setStatus(iter.value().sn, offline);
                   }
               }
            }
            iter++;
        }
    }
};

#endif // DEVICEDATA_H
