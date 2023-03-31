#ifndef ONLYAPP_H
#define ONLYAPP_H

#include <QObject>
#include <QLocalServer>

class OnlyApp : public QObject
{
    Q_OBJECT
public:
    explicit OnlyApp(QObject *parent = nullptr);
    bool isOnly();
private:
    QLocalServer *m_localServer;
signals:

private slots:
    void newLocalSocketConnection();
};

#endif // ONLYAPP_H
