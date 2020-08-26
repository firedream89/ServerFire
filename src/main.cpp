#include <QCoreApplication>

#include <serverfire.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServerFire serv;
    serv.Test();

    return a.exec();
}
