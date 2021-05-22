#ifndef GLOBALSERVER_H
#define GLOBALSERVER_H

#include <QObject>
#include "../../CryptoFire/src/cryptofire.h"
#include <exception>

class GlobalServer : public QObject
{
    Q_OBJECT

public:
    enum NetworkError {noError, passwordError, dataError};
    enum Privilege {Admin, User};
    enum Type {TCP, Web};

    virtual bool Start(int port) = 0;
    virtual bool Stop() = 0;
    //virtual bool Reload() = 0;
    virtual bool IsOnline() = 0;
    virtual QStringList InfoServer() { return QStringList(password); }
    virtual bool SendToClient(int idClient, QString data) = 0;
    virtual bool DisconnectClient(int idClient, QString reason) = 0;
    bool SetCrypto(int keySize, int codeSize, int charFormat);
    void SetAuthName(bool activate) { authName = activate; }
    void SetPassword(QString passwd) { password = passwd; }
    void SetPrivilege(int priv) { if(priv == Admin || priv == User) privilege = priv; }
    void SetAuthNameList(QStringList list);
    bool AddAuthName(QString name);
    void EncryptData(int idClient, QString &data);

private slots:
    virtual void NewConnexion() = 0;
    virtual void Disconnect() = 0;

signals:
    void Receipt(QString client, QString &data);
    void Info(QString classname, QString text);

private:
    enum AuthStep {clientKey, passwordOk, ready};

    bool Auth(int client, QString &data);
    QMap<QString, int> clientAuth;
    int privilege;
    CryptoFire *crypto;
    bool authName;
    QStringList authNameList;
    QString password;

protected:
    void Init();
    void emitInfo(QString text);
    void ReceiptDataFromClient(int client, QString data);
    bool ClientDisconnected(int idClient);
    int type;
};

#endif // GLOBALSERVER_H
