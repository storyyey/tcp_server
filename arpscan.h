#ifndef ARPSCAN_H
#define ARPSCAN_H

#include <QWidget>

class ArpScan : public QWidget
{
    Q_OBJECT
public:
    explicit ArpScan(QWidget *parent = nullptr);
    void start();
    void stop();
signals:

};

#endif // ARPSCAN_H
