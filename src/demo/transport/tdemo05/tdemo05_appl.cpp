#include "tdemo05_appl.h"

#include "shared/spin_locker.h"
#include "shared/steady_timer.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/config/logger_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include "pproto/commands/base.h"
#include "pproto/commands/pool.h"

#include <unistd.h>

#define log_error_m   alog::logger().error   (alog_line_location, "Application")
#define log_warn_m    alog::logger().warn    (alog_line_location, "Application")
#define log_info_m    alog::logger().info    (alog_line_location, "Application")
#define log_verbose_m alog::logger().verbose (alog_line_location, "Application")
#define log_debug_m   alog::logger().debug   (alog_line_location, "Application")
#define log_debug2_m  alog::logger().debug2  (alog_line_location, "Application")

#define KILL_TIMER(TIMER) {if (TIMER != -1) {killTimer(TIMER); TIMER = -1;}}

volatile bool Application::_stop = false;
std::atomic_int Application::_exitCode = {0};

Application::Application(int& argc, char** argv)
    : QCoreApplication(argc, argv)
    , _clientSocket(new tcp::Socket)
{
    _stopTimerId = startTimer(1000);

    chk_connect_q(_clientSocket.get(), &tcp::Socket::message,
                  this, &Application::clientMessage)

    chk_connect_q(_clientSocket.get(), &tcp::Socket::connected,
                  this, &Application::clientSocketConnected)

    chk_connect_q(_clientSocket.get(), &tcp::Socket::disconnected,
                  this, &Application::clientSocketDisconnected)

    chk_connect_q(&tcp::listener(), &tcp::Listener::message,
                  this, &Application::serverMessage)

    chk_connect_q(&tcp::listener(), &tcp::Listener::socketConnected,
                  this, &Application::serverSocketConnected)

    chk_connect_q(&tcp::listener(), &tcp::Listener::socketDisconnected,
                  this, &Application::serverSocketDisconnected)

    #define FUNC_REGISTRATION(COMMAND) \
        _cliendFuncInvoker.registration(command:: COMMAND, &Application::client_command_##COMMAND, this);

    FUNC_REGISTRATION(TDemo05_GetData)
    FUNC_REGISTRATION(TDemo05_SendChunk)
    FUNC_REGISTRATION(TDemo05_SendFinish)
    FUNC_REGISTRATION(TDemo05_SendError)

    #undef FUNC_REGISTRATION
}

bool Application::init()
{
    if (!tcp::listener().init({QHostAddress::AnyIPv4, 33024}))
        return false;

    return true;
}

void Application::deinit()
{
    // Ждем завершения работы потоков
    while (!_threadIds.empty())
        usleep(100*1000);

    _clientSocket->disconnect();
    tcp::listener().close();
}

void Application::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == _stopTimerId)
    {
        if (_stop)
        {
            KILL_TIMER(_stopTimerId);
            exit(_exitCode);
            return;
        }
    }
}

void Application::clietnConnect()
{
    _clientSocket->init({QHostAddress::LocalHost, 33024});
    _clientSocket->connect();
}

void Application::clientMessage(const pproto::Message::Ptr& message)
{
    // Не обрабатываем сообщения если приложение получило команду на остановку
    if (_stop)
        return;

    if (message->processed())
        return;

    if (lst::FindResult fr = _cliendFuncInvoker.findCommand(message->command()))
    {
        if (command::pool().commandIsSinglproc(message->command()))
            message->markAsProcessed();
        _cliendFuncInvoker.call(message, fr);
    }
}

void Application::serverMessage(const pproto::Message::Ptr& message)
{
    // Не обрабатываем сообщения если приложение получило команду на остановку
    if (_stop)
        return;

    if (message->command() == command::TDemo05_GetData)
    {
        // Обрабатываем команду TDemo05_GetData
        log_debug_m << "Server: received command TDemo05_GetData";

        auto tfunc = [this](Message::Ptr message)
        {
            trd::ThreadIdLock threadIdLock(&_threadIds); (void) threadIdLock;

            data::TDemo05_GetData tdemo05_GetData;
            readFromMessage(message, tdemo05_GetData);

            // Предполагаемое число чанков данных. На клиенте эта информация может
            // использоваться, например, для отображения статуса загрузки
            tdemo05_GetData.chunkCount = 10;

            // Отправляем ответ
            Message::Ptr answer = message->cloneForAnswer();
            writeToMessage(tdemo05_GetData, answer);
            tcp::listener().send(answer);

            log_debug_m << "Server: Begin send data";

            //--- Механизм отправки данных ---
            base::Socket::Ptr socket =
                tcp::listener().socketByDescriptor(message->socketDescriptor());

            quint32 crc = 0;
            Message::Ptr msg;
            steady_timer answerTimer;

            for (int i = 0; i < 10; ++i)
            {
                data::TDemo05_SendChunk tdemo05_SendChunk;
                tdemo05_SendChunk.index = i;
                tdemo05_SendChunk.data  = i + 5; // Какие-то данные
                crc += (i + 5);

                msg = createMessage(tdemo05_SendChunk);
                msg->appendDestinationSocket(message->socketDescriptor());
                msg->setPriority(Message::Priority::Low);
                tcp::listener().send(msg);

                answerTimer.reset();
                while (true)
                {
                    if (_stop)
                        return;

                    if (!socket->isConnected())
                    {
                        QMutexLocker locker {&_sendLock}; (void) locker;
                        _sendAnswers.remove(msg->id());
                        return;
                    }

                    { //Block for QMutexLocker
                        QMutexLocker locker {&_sendLock}; (void) locker;
                        if (_sendAnswers[msg->id()])
                        {
                            // Если от клиента получен положительный ответ,
                            // то можем переходить к следующей итерации
                            _sendAnswers.remove(msg->id());
                            break;
                        }
                        _sendCond.wait(&_sendLock, 50);
                    }

                    if (answerTimer.elapsed() > 10*1000 /*10 сек*/)
                    {
                        data::TDemo05_SendError tdemo05_SendError;
                        tdemo05_SendError.errorInfo.assign(error::tdemo05_send_timeout);

                        msg = createMessage(tdemo05_SendError);
                        msg->appendDestinationSocket(message->socketDescriptor());
                        msg->setPriority(Message::Priority::Low);
                        tcp::listener().send(msg);
                        return;
                    }
                }
            }

            data::TDemo05_SendFinish tdemo05_SendFinish;
            tdemo05_SendFinish.crc = crc;

            msg = createMessage(tdemo05_SendFinish);
            msg->appendDestinationSocket(message->socketDescriptor());
            msg->setPriority(Message::Priority::Low);
            tcp::listener().send(msg);

            log_debug_m << "Server: End send data";
        };

        std::thread t {tfunc, message};
        t.detach();

    } // command::TDemo05_GetData

    else if (message->command() == command::TDemo05_SendChunk)
    {
        log_debug_m << "Server: received answer TDemo05_SendChunk";

        QMutexLocker locker {&_sendLock}; (void) locker;
        // Получен ответ от клиента
        _sendAnswers[message->id()] = true;
        _sendCond.wakeAll();
    }
}

void Application::clientSocketConnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Client: connection to server success. Socket: " << socketDescript;

    data::TDemo05_GetData tdemo05_GetData;

    // Отправляем запрос на получение данных
    Message::Ptr m = createMessage(tdemo05_GetData);
    _clientSocket->send(m);
}

void Application::clientSocketDisconnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Client: disconnected from server. Socket: " << socketDescript;
}

void Application::serverSocketConnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Server: client connection. Socket: " << socketDescript;
}

void Application::serverSocketDisconnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Server: client closed connection. Socket: " << socketDescript;
}

void Application::client_command_TDemo05_GetData(const Message::Ptr& message)
{
    log_debug_m << "Client: received answer TDemo05_GetData";

    data::TDemo05_GetData tdemo05_GetData;
    readFromMessage(message, tdemo05_GetData);
    log_debug_m << "Client: GetData chunk count: " << tdemo05_GetData.chunkCount;

    _sendCrc = 0;
}

void Application::client_command_TDemo05_SendChunk(const Message::Ptr& message)
{
    log_debug_m << "Client: received command TDemo05_SendChunk";

    data::TDemo05_SendChunk tdemo05_SendChunk;
    readFromMessage(message, tdemo05_SendChunk);
    log_debug_m << "Client: SendChunk index: " << tdemo05_SendChunk.index;

    _sendCrc += (tdemo05_SendChunk.index + 5);

    Message::Ptr answer = message->cloneForAnswer();
    _clientSocket->send(answer);
}

void Application::client_command_TDemo05_SendFinish(const Message::Ptr& message)
{
    log_debug_m << "Client: received command TDemo05_SendFinish";

    data::TDemo05_SendFinish tdemo05_SendFinish;
    readFromMessage(message, tdemo05_SendFinish);

    alog::Line logLine = log_debug_m << "Client: SendFinish crc ";
    if (tdemo05_SendFinish.crc == _sendCrc)
        logLine << "success";
    else
        logLine << "fail";

    stop();
}

void Application::client_command_TDemo05_SendError(const Message::Ptr& message)
{
    log_debug_m << "Client: received command TDemo05_SendError";

    data::TDemo05_SendError tdemo05_SendError;
    readFromMessage(message, tdemo05_SendError);
    log_debug_m << "Client: SendError description: "
                << tdemo05_SendError.errorInfo.description;
    stop();
}
