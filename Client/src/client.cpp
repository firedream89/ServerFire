#include "client.h"

client::client(int t, int priv, QString cname)
{
    SetCrypto(50,4,UTF8);

    name = cname;
    type = t;
    switch (type) {
    case TCP:
        tcp = new QTcpSocket();
        connect(tcp, &QTcpSocket::connected, this, &client::newConnexion);
        connect(tcp, &QTcpSocket::readyRead, this, &client::TCPReceipt);
        connect(tcp, &QTcpSocket::disconnected, this, &client::Disconnected);
        break;
    case Web:
        web = new QWebSocket();
        connect(web,&QWebSocket::connected, this, &client::newConnexion);
        connect(web,&QWebSocket::textMessageReceived,this,&client::WebReceipt);
        connect(web,&QWebSocket::disconnected,this,&client::Disconnected);
    }
    privilege = priv;
    AuthLvl = unhautorised;
}

bool client::Connect(QString ip, int port, QString passwd)
{
    crypto = new CryptoFire(cryptoOption.at(0), cryptoOption.at(1), cryptoOption.at(2));
    qDebug() << crypto->Add_Encrypted_Key("client",passwd);

    password = passwd;
    switch (type) {
    case TCP:
        tcp->connectToHost(ip,port);
        return tcp->isOpen();
        break;
    case Web:
        web->open(QUrl("ws://" + ip + ":" + QString::number(port)));
    }
    return true;
}

void client::Disconnect()
{
    tcp->disconnectFromHost();
    web->close(QWebSocketProtocol::CloseCodeNormal);
}

void client::Auth(QString data)
{
    switch(AuthLvl) {
    case unhautorised:
        SendToServer(crypto->Get_Key());
        AuthLvl = sendKey;
        break;
    case sendKey:
        crypto->Decrypt_Data(data, "client");
        crypto->Add_Encrypted_Key("server",password,data);
        SendToServer(crypto->Key_To_SHA256("server"));
        AuthLvl = sendHash;
        break;
    case sendHash:
        if(data == "OK") {
            QString data = name;
            crypto->Encrypt_Data(data,"client");
            SendToServer(data);
            AuthLvl = sendName;
        }
        break;
    case sendName:
        if(data == "READY") {
            AuthLvl = autorised;
            emit Ready();
        }
        break;
    case autorised:
        crypto->Decrypt_Data(data,"server");
        qDebug() << data;
        emit ReceiptData(data);
    }
}

bool client::TCPSend(QString data)
{
    QByteArray paquet;

    QDataStream out(&paquet, QIODevice::WriteOnly);

    quint16 empty(0);
    out << empty;

    out << data;

    out.device()->seek(0);
    out << static_cast<quint16>(static_cast<uint>(paquet.size()) - sizeof(quint16));

    tcp->write(paquet);
    return tcp->flush();
}

void client::TCPReceipt()
{
    QDataStream in(tcp);

    if(dataSize == 0)
    {
        if(tcp->bytesAvailable() < static_cast<uint>(sizeof(quint16)))
             return;
        in >> dataSize;
    }

    if(tcp->bytesAvailable() < dataSize)
        return;

    QString data;
    in >> data;

    Auth(data);

    dataSize = 0;
}

bool client::WebSend(QString data)
{
    web->sendTextMessage(data);
    return web->flush();
}

void client::WebReceipt(QString data)
{
    Auth(data);
}

void client::Disconnected()
{
    AuthLvl = unhautorised;
    dataSize = 0;
    crypto->Remove_Encrypted_Key("client");
    emit close(0);
}

void client::SetCrypto(int keySize, int codeSize, int charFormat)
{
    cryptoOption = {keySize, codeSize, charFormat};
}

void client::SendToServer(QString data)
{
    qDebug() << "Send data : " << data;

    if(AuthLvl == autorised)
        crypto->Encrypt_Data(data,"client");

    switch (type) {
    case TCP:
        TCPSend(data);
        break;
    case Web:
        WebSend(data);
    }
}
