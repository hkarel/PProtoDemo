#pragma once

#include "commands/commands.h"
#include "commands/error.h"

#include "pproto/transport/tcp.h"

#include <QtCore>
#include <QCoreApplication>

using namespace pproto;
using namespace pproto::transport;

class Application : public QCoreApplication
{
public:
    Application(int& argc, char** argv);

    bool init();
    void deinit();

public slots:
    void clietnConnect();

    // Функции-обработчики сообщений
    void clientMessage(const pproto::Message::Ptr&);
    void serverMessage(const pproto::Message::Ptr&);

    void clientSocketConnected(pproto::SocketDescriptor);
    void clientSocketDisconnected(pproto::SocketDescriptor);

    void serverSocketConnected(pproto::SocketDescriptor);
    void serverSocketDisconnected(pproto::SocketDescriptor);

private:
    Q_OBJECT
    tcp::Socket::Ptr _clientSocket;

};
