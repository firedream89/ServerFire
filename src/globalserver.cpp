#include "globalserver.h"

void GlobalServer::Init()
{
    crypto = new CryptoFire();

    authName = false;
}

void GlobalServer::SetCrypto(int keySize, int codeSize, int charFormat)
{
    crypto = new CryptoFire(keySize, codeSize, charFormat);
}

void GlobalServer::ReceiptDataFromClient(int client, QString data)
{
    if(!Auth(client, data))
        return;

    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(client);

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
    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(client);

    int step = clientAuth.value(clientStr,-1);
    switch (step) {
    case clientKey:
        if(data == crypto->Key_To_SHA256(clientStr)) {
            SendToClient(client, "OK");
            clientAuth.insert(clientStr, passwordOk);
        }
        else {
            DisconnectClient(client,"Bad Password");
        }
        break;
    case passwordOk:
        if(authName) {
            if(authNameList.contains(data)) {
                clientAuth.insert(clientStr, ready);
                SendToClient(client, "READY");
            }
        }
        else {
            clientAuth.insert(clientStr, ready);
            SendToClient(client, "READY");
        }
        break;
    case ready:
        emit Receipt(clientStr,data);
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
}
