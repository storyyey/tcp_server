#include "mainwindow.h"
#include <QApplication>
#include "onlyapp.h"
#include "arpscan.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OnlyApp oa;

    //ArpScan arp;
    //arp.start();

    if (!oa.isOnly()) {
        QMessageBox::warning(NULL, "提示", "程序已经在运行!",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return 0;
    }

    MainWindow w;
    a.setWindowIcon(QIcon(":/icon/device.png"));
    w.setWindowTitle("网关管理配置工具");
    w.show();
    //w.showMaximized();
    qSetMessagePattern("%{file} %{function} %{line}:%{message}");

    return a.exec();
}
