#include "devicedb.h"
#include "mainwindow.h"
#include <QApplication>

QSqlQuery DeviceDB::sqlquery;
QString DeviceDB::devicesList = "DEVICELIST";
QString DeviceDB::ipconfigRecord = "IPCONFIGRECORD";
QString DeviceDB::upgradeRecord = "UPGRADERECORD";
QString DeviceDB::fileList = "FILELIST";
QString DeviceDB::presetIpTab = "PRESETIP";
QList<Devices::fileStore> DeviceDB::filesStore;

DeviceDB::DeviceDB()
{
    rwMutex = new QMutex();
    QString path = QDir::currentPath();
    QApplication::addLibraryPath(path + QString("/plugins"));
    QPluginLoader loader(path + QString("/plugins/sqldrivers/qsqlite.dll"));
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(path + "/devices.db");


    if (!database.open()) {
       windowConsole("打开数据库失败" + QString(database.lastError().text()));
    }
    else {
        windowConsole("打开数据库成功");
        sqlquery = QSqlQuery(database);
    }

    if (!isTableExist(devicesList)) {
        sqlquery.exec(QString("CREATE TABLE %1(\
                       SN        CHAR(20) NOT NULL,\
                       TIME      TEXT NOT NULL,\
                       IP        TEXT NOT NULL,\
                       VERSION   TEXT NOT NULL,\
                       MODEL     TEXT NOT NULL,\
                       REMARK    TEXT NOT NULL);").arg(devicesList));
        windowConsole("创建表" + devicesList);
    }

    if (!isTableExist(upgradeRecord)) {
        sqlquery.exec(QString("CREATE TABLE %1(\
                       SN        CHAR(20) NOT NULL,\
                       TIME      TEXT NOT NULL,\
                       OLD_VERSION   TEXT NOT NULL,\
                       NEW_VERSION   TEXT NOT NULL,\
                       MODEL     TEXT NOT NULL,\
                       STATE     BOOLEAN NOT NULL,\
                       REMARK    TEXT NOT NULL);").arg(upgradeRecord));
        windowConsole("创建表" + upgradeRecord);
    }

    if (!isTableExist(ipconfigRecord)) {
        sqlquery.exec(QString("CREATE TABLE %1(\
                       SN        CHAR(20) NOT NULL,\
                       TIME      TEXT NOT NULL,\
                       IP        TEXT NOT NULL,\
                       NETMASK   TEXT NOT NULL,\
                       GATEWAY   TEXT NOT NULL,\
                       REMARK    TEXT NOT NULL);").arg(ipconfigRecord));
        windowConsole("创建表" + ipconfigRecord);
    }

    if (!isTableExist(fileList)) {
        sqlquery.exec(QString("CREATE TABLE %1(\
                       MD5     TEXT NOT NULL,\
                       TIME    TEXT NOT NULL,\
                       NAME    TEXT NOT NULL,\
                       DATA    BLOB NOT NULL,\
                       SIZE    INTEGER NOT NULL,\
                       VERSION TEXT,\
                       MODEL TEXT,\
                       REMARK  TEXT);").arg(fileList));
        windowConsole("创建表" + fileList);
    }

    if (!isTableExist(presetIpTab)) {
        sqlquery.exec(QString("CREATE TABLE %1(\
                       POS     TEXT NOT NULL,\
                       IP      INTEGER NOT NULL,\
                       MASK    INTEGER NOT NULL,\
                       GATEWAY INTEGER NOT NULL,\
                       USE     INTEGER NOT NULL);").arg(presetIpTab));
        windowConsole("创建表" + presetIpTab);
    }

    windowConsole(database.lastError().text());
    foreachFileList();
}

DeviceDB::~DeviceDB()
{
    database.close();
}

bool DeviceDB::isTableExist(QString &table)
{
    sqlquery.exec(findTableSql.arg(table));

    return sqlquery.next();
}

bool DeviceDB::save(Devices::devInfo &data)
{
    if (!data.sn.length()) {
        return false;
    }

    QString find = QString("SELECT * FROM %1 WHERE SN IS '%2'").arg(devicesList).arg(data.sn);

    /* 更新数据 */
    QString update;
    if (data.remark.length()) {
        update = QString("UPDATE %1 SET TIME = '%2', IP = '%3',\
                              VERSION = '%4', MODEL = '%5', REMARK = '%6' WHERE SN = '%7';"\
                            ).arg(devicesList)\
                            .arg(data.time)\
                            .arg(data.ip)\
                            .arg(data.version)\
                            .arg(data.model)\
                            .arg(data.remark)\
                            .arg(data.sn);
    }
    else {
        update = QString("UPDATE %1 SET TIME = '%2', IP = '%3',\
                              VERSION = '%4', MODEL = '%5' WHERE SN = '%6';"\
                            ).arg(devicesList)\
                            .arg(data.time)\
                            .arg(data.ip)\
                            .arg(data.version)\
                            .arg(data.model)\
                            .arg(data.sn);
    }

    /* 新设备 */
    QString insert = QString("INSERT INTO %1 (SN,TIME,IP,VERSION,MODEL,REMARK)\
                             VALUES ('%2', '%3', '%4', '%5', '%6', '%7');").arg(devicesList)\
                             .arg(data.sn)\
                             .arg(data.time)\
                             .arg(data.ip)\
                             .arg(data.version)\
                             .arg(data.model)\
                             .arg(data.remark);

    if (sqlquery.exec(find) && sqlquery.next()) {
        sqlquery.exec(update);
    }
    else {
        windowConsole(sqlquery.lastError().text());
        if (sqlquery.exec(insert) == false) {
            windowConsole(insert);
            windowConsole(sqlquery.lastError().text());
        }
    }

    return true;
}

bool DeviceDB::save(Devices::upgradeRecord &data)
{
    QString insert = QString("INSERT INTO %1 (SN,TIME,OLD_VERSION,NEW_VERSION,MODEL,STATE,REMARK)\
                             VALUES ('%2', '%3', '%4', '%5', '%6', '%7', '%8');").arg(upgradeRecord)\
                             .arg(data.sn)\
                             .arg(data.time)\
                             .arg(data.old_ver)\
                             .arg(data.new_ver)\
                             .arg(data.model)\
                             .arg(data.state)\
                             .arg(data.remark);

    if (sqlquery.exec(insert) == false) {
        windowConsole(insert);
        windowConsole(sqlquery.lastError().text());
    }

    return true;
}

bool DeviceDB::save(Devices::ipconfigRecord &data)
{
    QString insert = QString("INSERT INTO %1 (SN,TIME,IP,NETMASK,GATEWAY,REMARK)\
                             VALUES ('%2', '%3', '%4', '%5', '%6', '%7');").arg(ipconfigRecord)\
                             .arg(data.sn)\
                             .arg(data.time)\
                             .arg(data.ip)\
                             .arg(data.netmask)\
                             .arg(data.gateway)\
                             .arg(data.remark);

    if (sqlquery.exec(insert) == false) {
        windowConsole(insert);
        windowConsole(sqlquery.lastError().text());
    }

    return true;
}

bool DeviceDB::save(Devices::fileStore &fs)
{
    if (fileIsExist(fs.md5)) {
        return false;
    }

    QString insert = QString("INSERT INTO %1(MD5,TIME,NAME,DATA,SIZE,VERSION,MODEL,REMARK)\
                             VALUES(:MD5,:TIME,:NAME,:DATA,:SIZE,:VERSION,:MODEL,:REMARK)").arg(fileList);

    sqlquery.prepare(insert);
    sqlquery.bindValue(":MD5", fs.md5);
    sqlquery.bindValue(":TIME", fs.time);
    sqlquery.bindValue(":NAME", fs.name);
    sqlquery.bindValue(":SIZE", fs.size);
    sqlquery.bindValue(":DATA", QVariant(fs.data));
    sqlquery.bindValue(":VERSION", fs.version);
    sqlquery.bindValue(":MODEL", fs.model);
    sqlquery.bindValue(":REMARK", fs.remark);

    if (sqlquery.exec() == false) {
        windowConsole(insert);
        windowConsole(sqlquery.lastError().text());
    }
    else {
        filesStore.append(fs);
    }

    return true;
}

bool DeviceDB::save(Devices::presetIp &pi)
{
    QString find = QString("SELECT * FROM %1 WHERE IP IS '%2' AND POS IS '%3'")\
            .arg(presetIpTab).arg(pi.ip).arg(pi.pos);

    if (sqlquery.exec(find) && sqlquery.next()) {
       windowConsole(find);
       windowConsole(sqlquery.lastError().text());

       return false;
    }

    QString insert = QString("INSERT INTO %1(POS,IP,MASK,GATEWAY,USE)\
                             VALUES(:POS,:IP,:MASK,:GATEWAY,:USE)").arg(presetIpTab);

    sqlquery.prepare(insert);
    sqlquery.bindValue(":POS", pi.pos);
    sqlquery.bindValue(":IP", pi.ip);
    sqlquery.bindValue(":MASK", pi.mask);
    sqlquery.bindValue(":GATEWAY", pi.gateway);
    sqlquery.bindValue(":USE", pi.usen);

    if (sqlquery.exec() == false) {
        windowConsole(insert);
        windowConsole(sqlquery.lastError().text());
    }

    return true;
}

bool DeviceDB::del(Devices::fileStore &fs)
{
    QString del = QString("DELETE FROM %1 WHERE MD5 IS '%2'").arg(fileList).arg(fs.md5);
    if (sqlquery.exec() == false) {
        windowConsole(del);
        windowConsole(sqlquery.lastError().text());
    }

    return true;
}

bool DeviceDB::update(Devices::presetIp &pi)
{
   QString update = QString("UPDATE %1 SET USE = '%2' WHERE IP = '%3'AND\
                          MASK = '%4' AND GATEWAY = '%5' AND POS = '%6';"\
                        ).arg(presetIpTab)\
                        .arg(pi.usen)\
                        .arg(pi.ip)\
                        .arg(pi.mask)\
                        .arg(pi.gateway)\
                        .arg(pi.pos);

   if (sqlquery.exec(update) == false) {
       windowConsole(update);
       windowConsole(sqlquery.lastError().text());
       return false;
   }

   return true;
}

bool DeviceDB::select(const QString &sn, Devices::devInfo &data)
{
    QString find = QString("SELECT * FROM %1 WHERE SN IS '%2'").arg(devicesList).arg(sn);
    sqlquery.exec(find);

    if (sqlquery.next()) {
        data.state = Devices::offline;
        data.sn = sqlquery.value("SN").toString();
        data.time = sqlquery.value("TIME").toString();
        data.ip = sqlquery.value("IP").toString();
        data.version = sqlquery.value("VERSION").toString();
        data.model = sqlquery.value("MODEL").toString();
        data.remark = sqlquery.value("REMARK").toString();
        return true;
    }

    return false;
}

bool DeviceDB::getFile(const QString &md5, QByteArray &data)
{
    QString find = QString("SELECT DATA FROM %1 WHERE MD5 IS '%2'").arg(fileList).arg(md5);
    if (sqlquery.exec(find) && sqlquery.next()) {
       data = sqlquery.value("DATA").toByteArray();
       return true;
    }

    return false;
}

bool DeviceDB::fileIsExist(QString &md5)
{
    QString find = QString("SELECT * FROM %1 WHERE MD5 IS '%2'").arg(fileList).arg(md5);
    if (sqlquery.exec(find) && sqlquery.next()) {
        return true;
    }

    return false;
}

bool DeviceDB::select(QList<Devices::devInfo> &devinfos)
{
    QString find = QString("SELECT * FROM %1").arg(devicesList);
    sqlquery.exec(find);

    while (sqlquery.next()) {
        Devices::devInfo data;
        data.sn = sqlquery.value("SN").toString();
        data.time = sqlquery.value("TIME").toString();
        data.ip = sqlquery.value("IP").toString();
        data.version = sqlquery.value("VERSION").toString();
        data.model = sqlquery.value("MODEL").toString();
        data.remark = sqlquery.value("REMARK").toString();
        devinfos.append(data);
    }

    return true;
}

bool DeviceDB::select(QList<Devices::upgradeRecord> &records)
{
    QString find = QString("SELECT * FROM %1").arg(upgradeRecord);
    sqlquery.exec(find);

    while (sqlquery.next()) {
        Devices::upgradeRecord data;
        data.sn = sqlquery.value("SN").toString();
        data.time = sqlquery.value("TIME").toString();
        data.old_ver = sqlquery.value("OLD_VERSION").toString();
        data.new_ver = sqlquery.value("NEW_VERSION").toString();
        data.model = sqlquery.value("MODEL").toString();
        data.state = sqlquery.value("STATE").toString();
        data.remark = sqlquery.value("REMARK").toString();
        records.append(data);
    }

    return true;
}

bool DeviceDB::select(QList<Devices::ipconfigRecord> &ipcfgs)
{
    QString find = QString("SELECT * FROM %1").arg(ipconfigRecord);
    sqlquery.exec(find);

    while (sqlquery.next()) {
        Devices::ipconfigRecord data;
        data.sn = sqlquery.value("SN").toString();
        data.time = sqlquery.value("TIME").toString();
        data.ip = sqlquery.value("IP").toString();
        data.netmask = sqlquery.value("NETMASK").toString();
        data.gateway = sqlquery.value("GATEWAY").toString();
        data.remark = sqlquery.value("REMARK").toString();
        ipcfgs.append(data);
    }

    return true;
}

bool DeviceDB::foreachFileList()
{
    QString find = QString("SELECT MD5, TIME, NAME, SIZE, VERSION, MODEL, REMARK FROM %1").arg(fileList);
    sqlquery.exec(find);
    windowConsole(sqlquery.lastError().text());
    while (sqlquery.next()) {
        Devices::fileStore fs;
        fs.md5 = sqlquery.value("MD5").toString();
        fs.time = sqlquery.value("TIME").toString();
        fs.name = sqlquery.value("NAME").toString();
        fs.size = sqlquery.value("SIZE").toInt();
        fs.version = sqlquery.value("VERSION").toString();
        fs.model = sqlquery.value("MODEL").toString();
        fs.remark = sqlquery.value("REMARK").toString();
        filesStore.append(fs);
    }

    return true;
}

bool DeviceDB::select(QList<Devices::presetIp> &presetIps)
{
    QString find = QString("SELECT * FROM %1").arg(presetIpTab);
    sqlquery.exec(find);

    while (sqlquery.next()) {
        Devices::presetIp data;
        data.pos = sqlquery.value("POS").toString();
        data.ip = sqlquery.value("IP").toUInt();
        data.mask = sqlquery.value("MASK").toUInt();
        data.gateway = sqlquery.value("GATEWAY").toUInt();
        data.usen = sqlquery.value("USE").toUInt();
       presetIps.append(data);
    }

    return true;
}

QList<Devices::fileStore> &DeviceDB::getFileStore()
{
    return filesStore;
}

