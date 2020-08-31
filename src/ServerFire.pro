QT -= gui
QT += network
QT += websockets

TEMPLATE = lib
DEFINES += SERVERFIRE_LIBRARY
VERSION = 1.0.1

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    globalserver.cpp \
    serverfire.cpp \
    tcpserver.cpp \
    webserver.cpp

HEADERS += \
    ServerFire_global.h \
    globalserver.h \
    serverfire.h \
    tcpserver.h \
    webserver.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../../CryptoFire/Lib/RPI/ -lCryptoFire

INCLUDEPATH += $$PWD/../../CryptoFire/RPI/Lib
DEPENDPATH += $$PWD/../../CryptoFire/RPI/Lib
