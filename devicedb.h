#ifndef DEVICEDB_H
#define DEVICEDB_H

#include <QtSql>
#include <QStandardItemModel>
#include <QMutex>
#include "DeviceData.h"

class DeviceDB : public QObject
{
    Q_OBJECT
public:
    DeviceDB();
    ~DeviceDB();
    bool isTableExist(QString &table);
    static bool save(Devices::devInfo &data);
    static bool save(Devices::upgradeRecord &data);
    static bool save(Devices::ipconfigRecord &data);
    static bool save(Devices::fileStore &fs);
    static bool save(Devices::presetIp &pi);
    static bool del(Devices::fileStore &fs);
    static bool update(Devices::presetIp &pi);
    static bool select(const QString &sn, Devices::devInfo &data);
    static bool getFile(const QString &md5, QByteArray &data);
    static bool fileIsExist(QString &md5);

    static bool select(QList<Devices::devInfo> &devinfos);
    static bool select(QList<Devices::upgradeRecord> &records);
    static bool select(QList<Devices::ipconfigRecord> &ipcfgs);
    bool foreachFileList();
    static bool select(QList<Devices::presetIp> &presetIps);

    static QList<Devices::fileStore>& getFileStore();
private:
    QSqlDatabase database;
    static QSqlQuery sqlquery;
    QString findTableSql = "select name from sqlite_master where name=\'%1\';";
    static QString devicesList;
    static QString ipconfigRecord;
    static QString upgradeRecord;
    static QString fileList;
    static QString presetIpTab;
    QMutex *rwMutex;
    static QList<Devices::fileStore> filesStore;
signals:
    void selectDBData(Devices::devInfo &data);
    void selectUpgradeRecord(Devices::upgradeRecord &data);
    void selectIpcfgRecord(Devices::ipconfigRecord &data);
    void selectPresetIp(Devices::presetIp &pi);
};

#endif // DEVICEDB_H
