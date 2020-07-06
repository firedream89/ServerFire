#include "tcpserver.h"

TCPServer::TCPServer()
{
    #define className "TCPServeur"

    Init();
}

TCPServer::~TCPServer()
{
    Stop();
    server->deleteLater();
}

bool TCPServer::Start(int port)
{
    if(server->isListening() && server->serverPort() == static_cast<quint16>(port)) return true;
    if(server->isListening()) return false;

    return server->listen(QHostAddress::Any,static_cast<quint16>(port));
}

bool TCPServer::Stop()
{
    QTcpSocket *socket;
    foreach (socket, client) {
        socket->close();
        socket->deleteLater();
    }
    client.clear();
    server->close();
    return true;
}

void TCPServer::NewConnexion()
{
    while(server->hasPendingConnections())
    {
        QTcpSocket *newCo = server->nextPendingConnection();
        newCo->setParent(server);

        connect(newCo,&QTcpSocket::readyRead,this,&TCPServer::ReceiptData);
        connect(newCo,&QTcpSocket::disconnected,this,&TCPServer::Disconnect);

        client.insert(client.lastKey()+1,newCo);
    }
}

void TCPServer::Disconnect()
{
    QTcpSocket *co = qobject_cast<QTcpSocket*>(sender());

    //remove client auth
    ClientDisconnected(client.key(co));

    //remove client
    client.remove(client.key(co));

    co->deleteLater();
}

bool TCPServer::SendToClient(int idClient, QString data)
{
    QTcpSocket *socket = client.value(idClient);
    if(!socket)
        return false;

    QByteArray paquet;

    QDataStream out(&paquet, QIODevice::WriteOnly);

    quint16 empty(0);
    out << empty;

    out << data;

    out.device()->seek(0);
    out << static_cast<quint16>(static_cast<uint>(paquet.size()) - sizeof(quint16));

    socket->write(paquet);
    return socket->flush();
}

void TCPServer::ReceiptData()
{
    QTcpSocket *socket = new QTcpSocket;
    while(socket)
    {
        socket = qobject_cast<QTcpSocket *>(sender());

        if (!socket)
            return;

        QDataStream in(socket);

        if(dataSize == 0)
        {
            if(socket->bytesAvailable() < static_cast<uint>(sizeof(quint16)))
                 return;
            in >> dataSize;
        }

        if(socket->bytesAvailable() < dataSize)
            return;

        QString data;
        in >> data;

        ReceiptDataFromClient(client.key(socket),data);

        dataSize = 0;
    }
}

bool TCPServer::DisconnectClient(int idClient, QString reason)
{
    SendToClient(idClient,reason);
    QTcpSocket *socket = client.value(idClient);
    socket->close();
    client.remove(idClient);

    return true;
}
