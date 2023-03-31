#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>

class udpserver : public QObject
{
    Q_OBJECT
public:
    explicit udpserver(QObject *parent = nullptr);
    QUdpSocket *udpSocket;
    void start();
    void stop();
signals:

};

#endif // UDPSERVER_H
