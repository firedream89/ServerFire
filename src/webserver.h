#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <globalserver.h>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

class WebServer : public GlobalServer
{
public:
    WebServer();
    ~WebServer();
    bool Start(int port) override;
    bool Stop() override;
    bool SendToClient(int idClient, QString data) override;
    bool DisconnectClient(int idClient, QString reason) override;

private slots:
    void NewConnexion() override;
    void ReceiptData(QString text);
    void Disconnect() override;

private:
    QMap<int,QWebSocket*> client;
    QWebSocketServer *server;
};

#endif // WEBSERVER_H
