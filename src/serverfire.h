#ifndef SERVERFIRE_H
#define SERVERFIRE_H

#include <QObject>

class ServerFire : public QObject
{
    Q_OBJECT

public:
    explicit ServerFire(QObject *parent = nullptr);
    void Test();
};

#endif // SERVERFIRE_H
