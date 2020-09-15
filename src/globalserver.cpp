#include "globalserver.h"

void GlobalServer::Init()
{
    #define className "Server"

    crypto = new CryptoFire();

    authName = false;
}

void GlobalServer::emitInfo(QString text)
{
    emit Info(className,text);
}

bool GlobalServer::SetCrypto(int keySize, int codeSize, int charFormat)
{
    crypto = new CryptoFire(keySize, codeSize, charFormat);
    try {
        crypto->Add_Encrypted_Key("server",password);
        return true;
    }
    catch(CryptoFire::error &e) {
        switch (e) {
        case CryptoFire::codeSize:
            emitInfo("Error(Crypto) : Password is too short !");
            break;
        case CryptoFire::sameName:
            emitInfo("Error(Crypto) : Name already exist !");
            break;
        case CryptoFire::badKey:
            emitInfo("Error(Crypto) : Key not valid !");
            break;
        default:
            emitInfo("Error(Crypto) : Unknown error");
        }
        return false;
    }
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
    emitInfo("Client " + clientStr + " disconnected");
    return crypto->Remove_Encrypted_Key(clientStr);
}

bool GlobalServer::Auth(int client, QString data)
{
    if(data.contains("Error key not found")) {
        return false;
    }

    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(client);

    int step = clientAuth.value(clientStr,-1);
    switch (step) {
    case clientKey:
        if(data == crypto->Key_To_SHA256("server")) {
            SendToClient(client, "OK");
            clientAuth.insert(clientStr, passwordOk);
        }
        else {
            DisconnectClient(client,"Bad Password");
        }
        break;
    case passwordOk:
        crypto->Decrypt_Data(data,clientStr);
        if(authName) {
            if(authNameList.contains(data)) {
                SendToClient(client, "READY");
                clientAuth.insert(clientStr, ready);
                emitInfo(data + "(" + clientStr + ") Authentified successfully");          
            }
            else {
                DisconnectClient(client,"Bad Name");
            }
        }
        else {
            SendToClient(client, "READY");
            clientAuth.insert(clientStr, ready);
            emitInfo(data + "(" + clientStr + ") Authentified successfully");    
        }
        break;
    case ready:
        crypto->Decrypt_Data(data, clientStr);
        return true;
        break;
    default:
        //newConnexion
        try {
            crypto->Add_Encrypted_Key(clientStr,password,data);
        } catch (CryptoFire::error &e) {
            switch (e) {
            case CryptoFire::codeSize:
                emitInfo("Error(Crypto) : Password is too short !");
                break;
            case CryptoFire::sameName:
                emitInfo("Error(Crypto) : Name already exist !");
                break;
            case CryptoFire::badKey:
                emitInfo("Error(Crypto) : Key not valid !");
                break;
            default:
                emitInfo("Error(Crypto) : Unknown error");
            }
            DisconnectClient(client,"Fail to encrypt key !");
            return false;
        }
        QString key = crypto->Get_Key();
        crypto->Encrypt_Data(key, clientStr);
        if(SendToClient(client,key)) {
            clientAuth.insert(clientStr,clientKey);
        }
        emitInfo("Client " + clientStr + " connected");
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

void GlobalServer::EncryptData(int idClient, QString &data)
{
    QString clientStr = (privilege == Admin) ? "A" : "U";
    clientStr += QString::number(idClient);

    if(clientAuth.value(clientStr) == ready)
        crypto->Encrypt_Data(data,"server");
}
