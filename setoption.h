#ifndef SETOPTION_H
#define SETOPTION_H

#include <QSettings>
#include <QTextCodec>

class SetOption {
public:
    SetOption();
    static void setValue(const QString &group, const QString &key, const QVariant &value) {
        QSettings settings("gatewayTool.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));

        settings.beginGroup(group);
        settings.setValue(key, value);
        settings.endGroup();
    }
    static QVariant value(const QString &group, const QString &key) {
        QSettings settings("gatewayTool.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));

        return settings.value(group + "/" + key);
    }
};

#endif // SETOPTION_H
