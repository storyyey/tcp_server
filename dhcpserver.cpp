#include "dhcpserver.h"
#include <QHostAddress>
#include "mainwindow.h"
// #include <cstring>
#include <WinSock2.h>

DhcpServer::DhcpServer()
{
    dhcpserver = NULL;
}

DhcpServer::~DhcpServer()
{
    stop();
}
#if 0
static void memcpy(void *dst, void *src, int size)
{
    while (size--)
        ((uint8_t *)dst)[size] = ((uint8_t *)src)[size];
}
#endif

uint32_t DhcpServer::decode(uint8_t *data, dhcpFrame &frame)
{
    dhcptu &tu = frame.tu;
    int offset = 0;

    tu.op = data[0];
    tu.htype = data[1];
    tu.hlen = data[2];
    tu.hops = data[3];

    tu.xid = byteToBigEndInteger(&data[4]);
    tu.secs = byteToBigEndShort(&data[8]);
    tu.flags = byteToBigEndShort(&data[10]);
    tu.ciaddr = byteToBigEndInteger(&data[12]);
    tu.yiaddr = byteToBigEndInteger(&data[16]);
    tu.siaddr = byteToBigEndInteger(&data[20]);
    tu.giaddr = byteToBigEndInteger(&data[24]);
    memcpy(tu.chaddr, &data[28], sizeof(tu.chaddr));
    memcpy(tu.sname, &data[44], sizeof(tu.sname));
    memcpy(tu.file, &data[108], sizeof(tu.file));

    offset = 108 + sizeof(tu.file);
    offset += 4;

    for (int n = 0; n < OPTION_MAX; n++) {
        frame.op[n].code = data[offset++];
        if (frame.op[n].code == 0xff) {
            break;
        }
        frame.op[n].len = data[offset++];
        memcpy(frame.op[n].padd, &data[offset], frame.op[n].len);
        offset += frame.op[n].len;
        frame.opn++;
    }
#if 0
    windowConsole(QString("Message type : %1").arg(tu.op));
    windowConsole(QString("Hardware type : %1").arg(tu.htype));
    windowConsole(QString("Hardware addresslen : %1").arg(tu.hlen));
    windowConsole(QString("Hops : %1").arg(tu.hops));
    windowConsole(QString("xid : "));
    windowConsole(QString("secs : %1").arg(tu.secs));
    windowConsole(QString("flags : %1").arg(tu.flags));
    windowConsole(QString("ciaddr : ") + QHostAddress(tu.ciaddr).toString());
    windowConsole(QString("yiaddr : ") + QHostAddress(tu.yiaddr).toString());
    windowConsole(QString("siaddr : ") + QHostAddress(tu.siaddr).toString());
    windowConsole(QString("giaddr : ") + QHostAddress(tu.giaddr).toString());
    windowConsole(QString("chaddr : ") + QByteArray(tu.chaddr, sizeof(tu.chaddr)).toHex());
    windowConsole(QString("sname : ") + QByteArray(tu.sname, sizeof(tu.sname)).toHex());
    windowConsole(QString("file : ") + QByteArray(tu.file, sizeof(tu.file)).toHex());
#endif
    return 0;
}

uint32_t DhcpServer::encode(uint8_t *data, dhcpFrame &frame)
{
    dhcptu &tu = frame.tu;
    int offset = 0;

    data[0] = tu.op;
    data[1] = tu.htype;
    data[2] = tu.hlen;
    data[3] = tu.hops;
    memcpy(&data[4], bigEndIntegerToByte(tu.xid), 4);
    memcpy(&data[8], bigEndShortToByte(tu.secs), 2);
    memcpy(&data[10], bigEndShortToByte(tu.flags), 2);
    memcpy(&data[12], bigEndIntegerToByte(tu.ciaddr), 4);
    memcpy(&data[16], bigEndIntegerToByte(tu.yiaddr), 4);
    memcpy(&data[20], bigEndIntegerToByte(tu.siaddr), 4);
    memcpy(&data[24], bigEndIntegerToByte(tu.giaddr), 4);
    memcpy(&data[28], tu.chaddr,  sizeof(tu.chaddr));
    memcpy(&data[44], tu.sname, sizeof(tu.sname));
    memcpy(&data[108], tu.file, sizeof(tu.file));

    offset = 108 + sizeof(tu.file);
    memcpy(&data[offset], "\x63\x82\x53\x63",  4);
    offset += 4;
    for (int nn = 0; nn < frame.opn; nn++) {
        data[offset++] = frame.op[nn].code;
        data[offset++] = frame.op[nn].len;
        memcpy(&data[offset], frame.op[nn].padd, frame.op[nn].len);
        offset += frame.op[nn].len;
    }
    data[offset++] = 255;

    return offset;
}

bool DhcpServer::addOption(DhcpServer::dhcpFrame &frame, uint8_t code, uint8_t len, uint8_t *val)
{
    if (frame.opn >= OPTION_MAX)
        return false;

    frame.op[frame.opn].code = code;
    frame.op[frame.opn].len = len;
    memcpy(frame.op[frame.opn].padd, val, len);
    frame.opn++;

    return true;
}

bool DhcpServer::findOption(DhcpServer::dhcpFrame &frame, uint8_t code, uint8_t *len, uint8_t *val)
{
    for (int n = 0; n < frame.opn; n++) {
        if (frame.op[n].code == code) {
            *len = frame.op[n].len;
            memcpy(val, frame.op[n].padd, *len);
            return true;
        }
    }

    return false;
}

void DhcpServer::reply(QHostAddress &addr, dhcpFrame &request)
{
    dhcpFrame reply;
    char remsg[1024] = {0};
    uint32_t srvip = cfg.srvip;
    uint32_t netMask = cfg.netmask;
    uint32_t router = cfg.router;
    uint32_t broadcast = cfg.broadcast;

    memset(&reply, 0, sizeof(reply));
    reply.tu = request.tu;
    uint8_t msgtype = 0;
    uint8_t oplen;
    if (!findOption(request, 53, &oplen, &msgtype)) {
        windowConsole("未找到选项字段");
        return ;
    }

    switch (msgtype) {
        case DhcpServer::DISCOVER: {
            windowConsole("收到 DHCP DISCOVER 报文");
            sessionRecord sess;
            if (findSessionRecord(reply.tu.xid, sess)) {
                reply.tu.yiaddr = sess.yiaddr;
            }
            else {
                reply.tu.yiaddr = deAvailableIP();
                sess.yiaddr = reply.tu.yiaddr;
                addSessionRecord(reply.tu.xid, sess);
            }

            reply.tu.op = 2;
            reply.tu.secs = htons(0);

            windowConsole(QString("分配IP地址%1到客户端").arg(QHostAddress(reply.tu.yiaddr).toString()));

            uint8_t msgtype = DhcpServer::OFFER;
            addOption(reply, 53, 1, &msgtype);
            addOption(reply, 54, 4, bigEndIntegerToByte(srvip));
            addOption(reply, 1, 4, bigEndIntegerToByte(netMask));
            addOption(reply, 3, 4, bigEndIntegerToByte(router));
            addOption(reply, 6, 4, bigEndIntegerToByte(srvip));
            const char *sname = "devices mamage tools";
            addOption(reply, 15, strlen(sname), (uint8_t *)sname);
            addOption(reply, 28, 4, bigEndIntegerToByte(broadcast));
            uint32_t len =  encode((uint8_t *)remsg, reply);
            dhcpserver->writeDatagram(remsg, len, QHostAddress("255.255.255.255"), 68);
            windowConsole("回应 DHCP OFFER 报文");
            break;
        }
        case DhcpServer::REQUEST: {
            windowConsole("收到 DHCP REQUEST 报文");
            sessionRecord sess;
            if (!findSessionRecord(reply.tu.xid, sess)) {
                windowConsole("未知DHCP会话忽略");
                break ;
            }

            uint8_t requestip[16] = {0};
            uint8_t oplen;
            if (findOption(request, 50, &oplen, requestip)) {
                if (sess.yiaddr != byteToBigEndInteger(requestip)) {
                    windowConsole(QString("DHCP请求的IP(%1)地址异常拒绝").arg(QHostAddress(byteToBigEndInteger(requestip)).toString()));
                    break;
                }
                else {
                    reply.tu.yiaddr = byteToBigEndInteger(requestip);
                }
                windowConsole(QString("分配IP地址%1到客户端").arg(QHostAddress(reply.tu.yiaddr).toString()));
            }
            else {
                break;
            }

            reply.tu.op = 2;
            reply.tu.secs = 0;

            uint8_t msgtype = DhcpServer::ACK;
            addOption(reply, 53, 1, &msgtype);
            addOption(reply, 54, 4, bigEndIntegerToByte(srvip));
            addOption(reply, 1, 4, bigEndIntegerToByte(netMask));
            addOption(reply, 3, 4, bigEndIntegerToByte(router));
            addOption(reply, 6, 4, bigEndIntegerToByte(srvip));
            const char *sname = "devices mamage tools";
            addOption(reply, 15, strlen(sname), (uint8_t *)sname);
            addOption(reply, 28, 4, bigEndIntegerToByte(broadcast));
            uint32_t len =  encode((uint8_t *)remsg, reply);

            //windowConsole(QString("dhcpsend %1").arg( dhcpsend->state()));
            dhcpserver->writeDatagram(remsg, len, QHostAddress("255.255.255.255"), 68);

            windowConsole("回应 DHCP ACK 报文");
            break;
        }
    }
}

uint8_t *DhcpServer::bigEndIntegerToByte(uint32_t bigInt32)
{
    static uint8_t arr[4];

    arr[0] = (bigInt32 >> 24) & 0x000000ff;
    arr[1] = (bigInt32 >> 16) & 0x000000ff;
    arr[2] = (bigInt32 >> 8) & 0x000000ff;
    arr[3] = (bigInt32 >> 0) & 0x000000ff;

    return arr;
}

uint32_t DhcpServer::byteToBigEndInteger(uint8_t arr[])
{
    uint32_t Integer = 0;

    Integer |= (arr[0] & 0x000000ff) << 24;
    Integer |= (arr[1] & 0x000000ff) << 16;
    Integer |= (arr[2] & 0x000000ff) << 8;
    Integer |= (arr[3] & 0x000000ff) << 0;

    return Integer;
}

uint8_t *DhcpServer::bigEndShortToByte(uint16_t bigShort)
{
    static uint8_t arr[2];

    arr[0] = (bigShort >> 8) & 0x00ff;
    arr[1] = (bigShort >> 0) & 0x00ff;

    return arr;
}

uint16_t DhcpServer::byteToBigEndShort(uint8_t arr[])
{
    uint16_t Short = 0;

    Short |= (arr[0] & 0x00ff) << 8;
    Short |= (arr[1] & 0x00ff) << 0;

    return Short;
}

bool DhcpServer::start(DhcpServer::dhcpcfg &cfg)
{
    this->cfg = cfg;
    currAvailableIP = this->cfg.startip;
    if (!dhcpserver) {
        dhcpserver = new QUdpSocket();
        if (dhcpserver->bind(QHostAddress(this->cfg.srvip)/* QHostAddress::Any */, 67) == false) {
            windowConsole("DHCP SERVER 启动失败");
            windowConsole(dhcpserver->errorString());
            stop();
            return false;
        }
        connect(dhcpserver, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    }

    return true;
}

bool DhcpServer::stop()
{
   if (dhcpserver) {
       delete dhcpserver;
       dhcpserver = NULL;
   }

   return true;
}

uint32_t DhcpServer::deAvailableIP()
{
    if (currAvailableIP + 1 <= cfg.endip) {
        currAvailableIP++;
    }
    else {
        currAvailableIP = this->cfg.startip;
    }

    return  currAvailableIP;
}

bool DhcpServer::addSessionRecord(uint32_t xid, DhcpServer::sessionRecord &data)
{
    sessMap.insert(xid, data);

    return true;
}

bool DhcpServer::findSessionRecord(uint32_t xid, DhcpServer::sessionRecord &data)
{
    if (sessMap.contains(xid)) {
        data = sessMap[xid];
        return true;
    }
    else {
        return false;
    }
}

void DhcpServer::readPendingDatagrams()
{
    if (!dhcpserver)
        return ;

    while (dhcpserver->hasPendingDatagrams()) {
        char data[2048] = {0};
        QHostAddress address;
        quint16 port;
        qint64 recvsize = dhcpserver->readDatagram(data, sizeof(data), &address, &port);
        if (recvsize > 0) {
            dhcpFrame request;
            memset(&request, 0, sizeof(request));
            decode((uint8_t *)data, request);
            reply(address, request);
        }
    }
}
