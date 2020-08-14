#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QWebSocket>
#include <../../../CryptoFire/src/cryptofire.h>

class client : public QObject
{
    Q_OBJECT

    enum authlvl { unhautorised, sendKey, sendHash, sendName, autorised };
public:
    enum type { TCP, Web };
    enum privilege { Admin, User };

    client(int type, int privilege, QString cname);
    void SetCrypto(int keySize, int codeSize, int charFormat);
    void ReceiptData(QString data);
    bool Connect(QString ip, int port, QString password);

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
