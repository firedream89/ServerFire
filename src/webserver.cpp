#include "webserver.h"

WebServer::WebServer()
{
    Init();

    #define className "WebServeur"
    type = Web;
    SetPrivilege(User);

    server = new QWebSocketServer("web",QWebSocketServer::NonSecureMode);
    connect(server, &QWebSocketServer::newConnection, this, &WebServer::NewConnexion);
}

WebServer::WebServer(int priv, QString passwd, QStringList authNameList)
{
    Init();

    SetPrivilege(priv);
    SetPassword(passwd);
    SetAuthNameList(authNameList);

    #define className "WebServeur"
    type = Web;

    server = new QWebSocketServer("web",QWebSocketServer::NonSecureMode);
    connect(server, &QWebSocketServer::newConnection, this, &WebServer::NewConnexion);
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
    for(QWebSocket *socket : client.values()) {
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

        if(client.isEmpty())
            client.insert(0,socket);
        else
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
    socket->deleteLater();

    emit Info(className, "Client disconnected : " + reason);

    return true;
}

QStringList WebServer::InfoServer()
{
    QStringList info = GlobalServer::InfoServer();
    info.append(QString::number(server->serverPort()));
    info.append(QString::number(client.count()));
    return info;
}
