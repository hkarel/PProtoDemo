#include "tdemo03_serv_appl.h"

#include "shared/spin_locker.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/commands/pool.h"

#define log_error_m   alog::logger().error   (alog_line_location, "Application")
#define log_warn_m    alog::logger().warn    (alog_line_location, "Application")
#define log_info_m    alog::logger().info    (alog_line_location, "Application")
#define log_verbose_m alog::logger().verbose (alog_line_location, "Application")
#define log_debug_m   alog::logger().debug   (alog_line_location, "Application")
#define log_debug2_m  alog::logger().debug2  (alog_line_location, "Application")

#define KILL_TIMER(TIMER) {if (TIMER != -1) {killTimer(TIMER); TIMER = -1;}}

QUuidEx Application::_applId;
volatile bool Application::_stop = false;
std::atomic_int Application::_exitCode = {0};

Application::Application(int& argc, char** argv)
    : QCoreApplication(argc, argv)
{
    _stopTimerId = startTimer(1000);

    #define FUNC_REGISTRATION(COMMAND) \
        _funcInvoker.registration(command:: COMMAND, &Application::command_##COMMAND, this);

    FUNC_REGISTRATION(TDemo03_01)
    FUNC_REGISTRATION(TDemo03_02)

    #undef FUNC_REGISTRATION
}

bool Application::init()
{
    return true;
}

void Application::deinit()
{
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

void Application::stop(int exitCode)
{
    _exitCode = exitCode;
    stop();
}

void Application::message(const pproto::Message::Ptr& message)
{
    // Не обрабатываем сообщения если приложение получило команду на остановку
    if (_stop)
        return;

    if (message->processed())
        return;

    if (lst::FindResult fr = _funcInvoker.findCommand(message->command()))
    {
        if (command::pool().commandIsSinglproc(message->command()))
            message->markAsProcessed();
        _funcInvoker.call(message, fr);
    }
}

void Application::socketConnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Client connection to server. Socket: " << socketDescript;
}

void Application::socketDisconnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Client disconnected from server. Socket: " << socketDescript;
}

void Application::command_TDemo03_01(const Message::Ptr& message)
{
    data::TDemo03_01 tdemo03_01;
    readFromMessage(message, tdemo03_01);

    log_debug_m << log_format("TDemo03_01 data: %?, %?",
                              tdemo03_01.value1, tdemo03_01.value2);

    Message::Ptr answer = message->cloneForAnswer();

    tdemo03_01.value1 = 10;
    tdemo03_01.value2 = "Response";

    writeToJsonMessage(tdemo03_01, answer);
    tcp::listener().send(answer);
}

void Application::command_TDemo03_02(const Message::Ptr& message)
{
    data::TDemo03_02 tdemo03_02;
    readFromMessage(message, tdemo03_02);

    log_debug_m << log_format("TDemo03_02 data: %?, %?",
                              tdemo03_02.value1, tdemo03_02.value2);

    Message::Ptr answer = message->cloneForAnswer();

    tdemo03_02.value1 = 15;
    tdemo03_02.value2 = QUuid::createUuid();

    writeToJsonMessage(tdemo03_02, answer);
    tcp::listener().send(answer);
}
