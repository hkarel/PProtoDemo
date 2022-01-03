#pragma once

#include "commands/commands.h"
#include "commands/error.h"

#include "pproto/func_invoker.h"
#include "pproto/transport/tcp.h"

#include <QtCore>
#include <QCoreApplication>
#include <atomic>

using namespace pproto;
using namespace pproto::transport;

class Application : public QCoreApplication
{
public:
    Application(int& argc, char** argv);

    bool init();
    void deinit();

    static void stop() {_stop = true;}
    static bool isStopped() {return _stop;}

public slots:
    void stop(int exitCode);
    void message(const pproto::Message::Ptr&);

    void socketConnected(pproto::SocketDescriptor);
    void socketDisconnected(pproto::SocketDescriptor);

private:
    Q_OBJECT
    void timerEvent(QTimerEvent* event) override;

    //--- Обработчики команд ---
    void command_TDemo03_01(const Message::Ptr&);
    void command_TDemo03_02(const Message::Ptr&);

private:
    static QUuidEx _applId;
    static volatile bool _stop;
    static std::atomic_int _exitCode;

    int _stopTimerId = {-1};

    FunctionInvoker _funcInvoker;
};
