#include "serverfire.h"

ServerFire::ServerFire(QObject *parent) : QObject(parent)
{
    cryptoOption = {50, 4, UTF8};
}

void ServerFire::Test()
{
    QString linkClient = "../ServerFire/test/build/release/ClientTest";
    QProcess p;
    QEventLoop loop;
    QTimer timer;
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);
    QStringList arg = {"type=", "privilege=0", "port=52000", "ip=127.0.0.1", "password=", "name=test"};
    static int pos = 0;
    qDebug() << pos;
    switch (pos) {
    case 0:
        connect(this,&ServerFire::ReceiptData,this,&ServerFire::TestReceipt);
        connect(this,&ServerFire::Info,this,&ServerFire::getInfo);
        std::cout << "Starting test...\n";
        std::cout << "Testing server TCP...";
        if(RunServer(GlobalServer::TCP, GlobalServer::Admin, 50000, "passwd", QStringList())) {
            std::cout << "Ok\n";
        }
        else {
            std::cout << "FAIL\n";
            pos = -1;
        }
        qDebug() << "Testing server info values : " << ServerInfo(GlobalServer::Admin);
        std::cout << "Testing server webSocket...";
        if(RunServer(GlobalServer::TCP, GlobalServer::User, 52000, "user", QStringList())) {
            std::cout << "Ok\n";
        }
        else {
            std::cout << "FAIL\n";
            pos = -1;
        }
        qDebug() << "Testing server info values : " << ServerInfo(GlobalServer::User);
        qDebug() << "USER :" << userServer << "ADMIN :" << adminServer;
        qDebug() << "USER :" << IsOnline(GlobalServer::User) << "ADMIN :" << IsOnline(GlobalServer::Admin);
        pos++;
        break;
    case 20:
        std::cout << "Ok\n";
        SetCrypto(100,8,UTF16);
        std::cout << "Stop server user " << StopServer(GlobalServer::User) << "\n";
        std::cout << "Server user is online : " << IsOnline(GlobalServer::User) << "\n";
        std::cout << "Starting Server user(TCP) :" << RunServer(GlobalServer::TCP, GlobalServer::User, 52000, "useruser", QStringList("Test")) << "\n";
        std::cout << "Server user is online : " << IsOnline(GlobalServer::User) << "\n";
        pos++;
        break;
    case 30:
        std::cout << "Stop servers " << StopServers() << "\n";
        std::cout << "End test\n";
    default:
        pos++;
    }
    if(pos == -1) {
        std::cout << "Test fail !\n";
        pos = 20;
    }

}

void ServerFire::TestReceipt(QString client, QString data)
{
    std::cout << "Receipt data from " << client.toStdString() << " data : " << data.toStdString() << "\n";
    Test();
}

bool ServerFire::RunServer(int type, int privilege, int port, QString password, QStringList authNameList)
{
    GlobalServer *server;
    if(type == GlobalServer::TCP) {
        server = new TCPServer(privilege,password,authNameList);
        connect(server, &TCPServer::Receipt, this, &ServerFire::ReceiptData);
        connect(server, &TCPServer::Info, this, &ServerFire::Info);
    }
    else {
        server = new WebServer(privilege,password,authNameList);
        connect(server, &WebServer::Receipt, this, &ServerFire::ReceiptData);
        connect(server, &WebServer::Info, this, &ServerFire::Info);
    }
    if(!server->SetCrypto(cryptoOption.at(0),cryptoOption.at(1),cryptoOption.at(2))) {
        return false;
    }

    if(privilege == GlobalServer::Admin) {
        adminServer = server;
    }
    else {
        userServer = server;
    }
    return server->Start(port);
}

bool ServerFire::StopServer(int privilege)
{

    switch (privilege) {
    case GlobalServer::Admin:
        return adminServer->Stop();
        break;
    case GlobalServer::User:
        return userServer->Stop();
    }
    return false;
}

bool ServerFire::StopServers()
{
    if(adminServer->Stop() && userServer->Stop())
        return true;
    return false;
}

bool ServerFire::SendDataToClient(QString clientStr, QString data)
{
    int privilege = clientStr.contains("A") ? GlobalServer::Admin : GlobalServer::User;
    int client = (privilege == GlobalServer::Admin) ? clientStr.remove('A').toInt() : clientStr.remove('U').toInt();
    switch (privilege) {
    case GlobalServer::Admin:
        return adminServer->SendToClient(client,data);
        break;
    case GlobalServer::User:
        return userServer->SendToClient(client,data);
    }

    return false;
}

void ServerFire::SetCrypto(int keySize, int codeSize, int charFormat)
{
    cryptoOption = {keySize, codeSize, charFormat};
}

bool ServerFire::IsOnline(int privilege)
{
    switch (privilege) {
    case GlobalServer::Admin:
        return adminServer->IsOnline();
        break;
    case GlobalServer::User:
        return userServer->IsOnline();
    }
    return false;
}

QStringList ServerFire::ServerInfo(int privilege)
{
    switch (privilege) {
    case GlobalServer::Admin:
        return adminServer->InfoServer();
        break;
    case GlobalServer::User:
        return userServer->InfoServer();
    }
    return QStringList();
}
