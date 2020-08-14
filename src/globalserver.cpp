#include "globalserver.h"

void GlobalServer::Init()
{
    crypto = new CryptoFire();

    authName = false;
}

void GlobalServer::SetCrypto(int keySize, int codeSize, int charFormat)
{
    crypto = new CryptoFire(keySize, codeSize, charFormat);
    crypto->Add_Encrypted_Key("server",password);
}

void GlobalServer::ReceiptDataFromClient(int client, QString data)
{
    if(!Auth(client, data))
        return;

    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(client);
    crypto->Decrypt_Data(data,clientStr);

    emit Receipt(clientStr, data);
}

bool GlobalServer::ClientDisconnected(int idClient)
{
    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(idClient);
    clientAuth.remove(clientStr);
    return crypto->Remove_Encrypted_Key(clientStr);
}

bool GlobalServer::Auth(int client, QString data)
{
    //qDebug() << authName << authNameList;
    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(client);

    int step = clientAuth.value(clientStr,-1);
    switch (step) {
    case clientKey:
        //qDebug() << QCryptographicHash::hash(crypto->Get_Key().toLatin1(),QCryptographicHash::Sha256).toHex();
        //qDebug() << crypto->Key_To_SHA256("server") << data;
        if(data == crypto->Key_To_SHA256("server")) {
            SendToClient(client, "OK");
            clientAuth.insert(clientStr, passwordOk);
        }
        else {
            DisconnectClient(client,"Bad Password");
        }
        break;
    case passwordOk:
        qDebug() << "AuthName " << authName;
        if(authName) {
            crypto->Decrypt_Data(data,clientStr);
            if(authNameList.contains(data)) {
                clientAuth.insert(clientStr, ready);
                SendToClient(client, "READY");
            }
            else {
                DisconnectClient(client,"Bad Name");
            }
        }
        else {
            clientAuth.insert(clientStr, ready);
            SendToClient(client, "READY");
        }
        break;
    case ready:
        crypto->Decrypt_Data(data, clientStr);
        return true;
        break;
    default:
        //newConnexion
        if(!crypto->Add_Encrypted_Key(clientStr,password,data)) {
            DisconnectClient(client,"Fail to encrypt key !");
            return false;
        }
        QString key = crypto->Get_Key();
        crypto->Encrypt_Data(key, clientStr);
        if(SendToClient(client,key)) {
            clientAuth.insert(clientStr,clientKey);
        }
    }
    return false;
}

bool GlobalServer::AddAuthName(QString name)
{
     if(authNameList.contains(name))
         return false;

     authNameList.append(name);
     return true;
}

void GlobalServer::SetAuthNameList(QStringList list)
{
    if(!list.isEmpty()) {
        authName = true;
    }
    authNameList = list;

    qDebug() << authName << authNameList;
}
