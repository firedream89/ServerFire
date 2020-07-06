#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <globalserver.h>
#include <QtNetwork>

class TCPServer : public GlobalServer
{
    enum state {NormalClose, ErrorClose};

public:
    TCPServer();
    ~TCPServer();
    bool Start(int port) override;
    bool Stop() override;
    bool SendToClient(int idClient, QString data) override;
    bool DisconnectClient(int idClient, QString reason) override;

private slots:
    void NewConnexion() override;
    void ReceiptData();
    void Disconnect() override;

private:
    QMap<int,QTcpSocket*> client;
    QTcpServer *server;
    quint16 dataSize;
};

#endif // TCPSERVER_H
