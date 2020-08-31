#ifndef SERVERFIRE_H
#define SERVERFIRE_H

#include "ServerFire_global.h"
#include <QObject>
#include "tcpserver.h"
#include "webserver.h"

class SERVERFIRE_EXPORT ServerFire : public QObject
{
    Q_OBJECT

public:
    explicit ServerFire(QObject *parent = nullptr);
    bool RunServer(int type, int privilege, int port, QString password, QStringList authNameList);
    void Test();
    void TestReceipt(QString client, QString data);
    bool StopServers();
    bool StopServer(int privilege);
    bool SendDataToClient(QString client, QString data);
    void SetCrypto(int keySize, int codeSize, int charFormat);
    bool IsOnline(int privilege);
    QStringList ServerInfo(int privilege);

private slots:
    void getInfo(QString className, QString data) { qDebug() << "INFO : " << data; }

signals:
    void Info(QString className, QString data);
    void ReceiptData(QString client, QString data);

private:
    GlobalServer *adminServer;
    GlobalServer *userServer;
    QList<int> cryptoOption;
};

#endif // SERVERFIRE_H
