#pragma once

#include "commands/commands.h"
#include "commands/error.h"

#include "shared/thread/thread_utils.h"

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
    void timerEvent(QTimerEvent* event) override;

    //--- Обработчики команд ---
    void client_command_TDemo05_GetData(const Message::Ptr&);
    void client_command_TDemo05_SendChunk(const Message::Ptr&);
    void client_command_TDemo05_SendFinish(const Message::Ptr&);
    void client_command_TDemo05_SendError(const Message::Ptr&);

private:
    static volatile bool _stop;
    static std::atomic_int _exitCode;

    int _stopTimerId = {-1};

    tcp::Socket::Ptr _clientSocket;
    FunctionInvoker  _cliendFuncInvoker;

    trd::ThreadIdList _threadIds;

    QMutex _sendLock;
    QWaitCondition _sendCond;

    quint32 _sendCrc = {0};
    QMap<QUuidEx /*Message Id*/, bool /*Message answer received*/> _sendAnswers;
};
