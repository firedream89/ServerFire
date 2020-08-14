#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <globalserver.h>
#include <QtNetwork>

class TCPServer : public GlobalServer
{
    enum state {NormalClose, ErrorClose};

public:
    TCPServer();
    TCPServer(int priv, QString passwd, QStringList authNameList);
    ~TCPServer();
    bool Start(int port) override;
    bool Stop() override;
    bool SendToClient(int idClient, QString data) override;
    bool DisconnectClient(int idClient, QString reason) override;
    bool IsOnline() override { return server->isListening(); }
    QStringList InfoServer() override;

private slots:
    void NewConnexion() override;
    void ReceiptData();
    void Disconnect() override;

private:
    QMap<int,QTcpSocket*> client;
    QTcpServer *server;
    quint16 dataSize;
    QMap<int, quint16> datSize;
};

#endif // TCPSERVER_H
