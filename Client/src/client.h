#ifndef CLIENT_H
#define CLIENT_H

#include "Client_global.h"
#include <QObject>
#include "QTcpSocket"
#include <QDataStream>
#include <QtWebSockets/QWebSocket>
#include "../../../../CryptoFire/src/cryptofire.h"

class CLIENT_EXPORT client : public QObject
{
    Q_OBJECT

    enum authlvl { unhautorised, sendKey, sendHash, sendName, autorised };
public:
    enum type { TCP, Web };
    enum privilege { Admin, User };

    client(int type, int privilege, QString cname);
    void SetCrypto(int keySize, int codeSize, int charFormat);
    bool Connect(QString ip, int port, QString password);
    void Disconnect();

public slots:
    void Disconnected();
    void SendToServer(QString data);
    bool TCPSend(QString data);
    void TCPReceipt();
    bool WebSend(QString data);
    void WebReceipt(QString data);
    void newConnexion() { Auth(QString()); }

signals:
    void Ready();
    void close(int ret);
    void ReceiptData(QString data);

private:
    void Auth(QString data);
    int AuthLvl;
    int type;
    int privilege;
    QList<int> cryptoOption;
    QString password;
    CryptoFire *crypto;
    QString name;

    QTcpSocket *tcp;
    QWebSocket *web;
    quint16 dataSize;
};

#endif // CLIENT_H
