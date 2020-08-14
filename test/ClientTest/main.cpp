#include <QCoreApplication>
#include <client.h>

class runClient : public QObject
{
public:
    runClient(int type, int privilege, QString clientName)
    {
        c = new client(type,privilege,clientName);
        connect(c, &client::Ready, this, &runClient::sendData);
        connect(c, &client::close, qApp, &QCoreApplication::exit);
    }
    void Connect(QString ip,int port,QString password) {
        c->Connect(ip,port,password);
    }

public slots:
    void sendData() {
        qDebug() << "Send data to server";
        c->SendToServer("Test");
    }

private:
    client *c;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString arg;
    int type, privilege, port = 0;
    QString ip, password, name = "";
    for(QString arg : a.arguments()) {
        if(arg.contains("type=")) {
            type = arg.split('=').last().toInt();
        }
        else if(arg.contains("privilege=")) {
            privilege = arg.split('=').last().toInt();
        }
        else if(arg.contains("port=")) {
            port = arg.split('=').last().toInt();
        }
        else if(arg.contains("ip=")) {
            ip = arg.split('=').last();
        }
        else if(arg.contains("password")) {
            password = arg.split('=').last();
        }
        else if(arg.contains("name=")) {
            name = arg.split('=').last();
        }
    }

    for(int i=0;i<1;i++) {
    runClient *c = new runClient(type,privilege,name);
    c->Connect(ip,port,password);
    }

    return a.exec();
}

