#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <QUdpSocket>

class DhcpServer : public QObject
{
    Q_OBJECT

public:
    struct dhcptu {
        uint8_t op;
        uint8_t htype;
        uint8_t hlen;
        uint8_t hops;
        uint32_t xid;
        uint16_t secs;
        uint16_t flags;
        uint32_t ciaddr;
        uint32_t yiaddr;
        uint32_t siaddr;
        uint32_t giaddr;
        char chaddr[16];
        char sname[64];
        char file[128];
    };
    struct dhcpop {
        uint8_t code;
        uint8_t len;
        char padd[255];
    };
#define OPTION_MAX (16)
    struct dhcpFrame {
        struct dhcptu tu;
        struct dhcpop op[OPTION_MAX];
        uint8_t opn;
    };
    enum messageType {
        DISCOVER = 0x01,
        OFFER = 0x02,
        REQUEST = 0x03,
        DECLINE = 0x04,
        ACK = 0x05,
        NAK = 0x06,
        RELEASE = 0x07,
        INFORM = 0x08,
    };

    struct dhcpcfg {
        uint32_t srvip;
        uint32_t startip;
        uint32_t endip;
        uint32_t netmask;
        uint32_t gateway;
        uint32_t broadcast;
        uint32_t router;
    };

    struct sessionRecord {
        uint32_t yiaddr;
    };

    DhcpServer();
    ~DhcpServer();
    uint32_t decode(uint8_t *data, dhcpFrame &frame);
    uint32_t encode(uint8_t *data, dhcpFrame &frame);
    bool addOption(dhcpFrame &frame, uint8_t code, uint8_t len, uint8_t *val);
    bool findOption(dhcpFrame &frame, uint8_t code, uint8_t *len, uint8_t *val);
    void reply(QHostAddress &addr, dhcpFrame &frame);
    inline uint8_t *bigEndIntegerToByte(uint32_t Integer);
    inline uint32_t byteToBigEndInteger(uint8_t arr[4]);
    inline uint8_t *bigEndShortToByte(uint16_t Short);
    inline uint16_t byteToBigEndShort(uint8_t arr[2]);
    bool start(dhcpcfg &cfg);
    bool stop();
    uint32_t deAvailableIP();
    bool addSessionRecord(uint32_t xid, sessionRecord &data);
    bool findSessionRecord(uint32_t xid, sessionRecord &data);
private:
    QUdpSocket *dhcpserver;
    dhcpcfg cfg;
    uint32_t currAvailableIP;
    QMap <uint32_t, sessionRecord> sessMap;
private slots:
    void readPendingDatagrams();
};

#endif // DHCPSERVER_H
