#if 0
#include "arpscan.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <QDebug>
#include <QByteArray>
#include <QHostAddress>
#include <QString>
#include <mstcpip.h>

#define SOURCE_PORT 7234
#define MAX_RECEIVEBYTE 255
#define MAX_ADDR_LEN 32
//#define SIO_RCVALL  (IOC_IN | IOC_VENDOR | 1)//定义网卡为混杂模式

ArpScan::ArpScan(QWidget *parent) : QWidget(parent)
{

}

void ArpScan::start()
{
    SOCKET sock;
    char recvBuf[65535] = { 0 };
    DWORD dwBytesRet;
    unsigned int optval = 1;
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_AH);
    if (sock < 0) {
        qDebug() << "socket create error";
    }

    char FAR name[MAXBYTE];
    gethostname(name, MAXBYTE);
    qDebug() << "gethostname" << name;
    struct hostent FAR* pHostent;
    pHostent = (struct hostent*)malloc(sizeof(struct hostent));
    pHostent = gethostbyname(name);
    SOCKADDR_IN sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(1);
    uint32_t ip = QHostAddress("10.200.67.222").toIPv4Address();
    sa.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    //memcpy(&sa.sin_addr, &ip, pHostent->h_length);
    qDebug() << "bind addr" << QString("%1 -- %2").arg(pHostent->h_addr_list[0]).arg(pHostent->h_length);

    bind(sock, (SOCKADDR*)&sa, sizeof(sa));//绑定
    if (WSAGetLastError() == 10013) {
        qDebug() << "bind error";
        exit(0);
    }


    qDebug() << "WSAIoctl = " << WSAIoctl(sock, SIO_RCVALL, &optval, sizeof(optval), nullptr, 0, &dwBytesRet, nullptr, nullptr);

    while (1) {
        int ret = recv(sock, recvBuf, sizeof(recvBuf), 0);
        if (ret > 0) {
            QByteArray recv(recvBuf, ret);
            qDebug() << recv.toHex();
        }
        else {
           // qDebug() << "recv = "<< ret;
        }
    }
}

void ArpScan::stop()
{
    WSACleanup();
}
#endif
