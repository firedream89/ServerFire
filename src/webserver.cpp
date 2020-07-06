#include "webserver.h"

WebServer::WebServer()
{
    #define className "WebServeur"
    type = Web;

    Init();
}

WebServer::WebServer(int priv, QString passwd, QStringList authNameList)
{
    SetPrivilege(priv);
    SetPassword(passwd);
    SetAuthNameList(authNameList);

    #define className "WebServeur"
    type = Web;

    Init();
}

WebServer::~WebServer()
{
    Stop();
    server->deleteLater();
}

bool WebServer::Start(int port)
{
    if(server->isListening() && server->serverPort() == port) return true;
    if(server->isListening()) return false;

    return server->listen(QHostAddress::Any,static_cast<quint16>(port));
}

bool WebServer::Stop()
{
    QWebSocket *socket;
    foreach (socket, client) {
        socket->close();
        socket->deleteLater();
    }
    client.clear();
    server->close();
    return true;
}

void WebServer::NewConnexion()
{
    while(server->hasPendingConnections())
    {
        QWebSocket *socket = server->nextPendingConnection();
        socket->setParent(server);

        connect(socket,&QWebSocket::textMessageReceived,this,&WebServer::ReceiptData);
        connect(socket,&QWebSocket::disconnected,this,&WebServer::Disconnect);

        client.insert(client.lastKey()+1,socket);
    }
}

void WebServer::Disconnect()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    //remove client auth
    ClientDisconnected(client.key(socket));

    //remove client
    client.remove(client.key(socket));

    socket->deleteLater();
}

bool WebServer::SendToClient(int idClient, QString data)
{
    QWebSocket *socket = client.value(idClient);
    if(!socket)
        return false;

    socket->sendTextMessage(data);
    return socket->flush();
}

void WebServer::ReceiptData(QString data)
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if(!socket)
        return;

    ReceiptDataFromClient(client.key(socket),data);
}

bool WebServer::DisconnectClient(int idClient, QString reason)
{
    SendToClient(idClient,reason);
    QWebSocket *socket = client.value(idClient);
    socket->close();
    client.remove(idClient);

    return true;
}
