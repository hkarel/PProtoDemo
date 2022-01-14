#include "wdemo01_serv_appl.h"

#include "shared/logger/logger.h"
#include "shared/logger/config.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"
#include "shared/thread/thread_pool.h"
#include "shared/thread/thread_utils.h"

#include "pproto/commands/pool.h"
//#include "pproto/transport/udp.h"

#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <thread>

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

    FUNC_REGISTRATION(WebPProtoHello)
    FUNC_REGISTRATION(WebSpeedTest)

    #undef FUNC_REGISTRATION
}

bool Application::init()
{
//    if (config::state().getValue("application.id", _applId, false))
//    {
//        log_verbose_m << "Read ApplId: " << _applId;
//    }
//    else
//    {
//        _applId = QUuidEx::createUuid();
//        config::state().setValue("application.id", _applId);
//        config::state().saveFile();
//        log_verbose_m << "Generated new ApplId: " << _applId;
//    }
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

void Application::message(const Message::Ptr& message)
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

void Application::clientSocketConnected(SocketDescriptor /*socketDesc*/)
{
}

void Application::clientSocketDisconnected(SocketDescriptor /*socketDesc*/)
{
}

void Application::command_WebPProtoHello(const Message::Ptr& message)
{
    Message::Ptr answer = message->cloneForAnswer();

    data::WebPProtoHello webPProtoHello;
    webPProtoHello.value = "Hello PProto-Web!";

    writeToJsonMessage(webPProtoHello, answer);
    tcp::listener().send(answer);
}

void Application::command_WebSpeedTest(const Message::Ptr& message)
{
    //static QUuidEx uuid {QUuid::createUuid()};
    Message::Ptr answer = message->cloneForAnswer();

    data::WebSpeedTest webSpeedTest;
    readFromMessage(message, webSpeedTest);
    //webSpeedTest.uuid = uuid;

    if (webSpeedTest.beginTest)
        alog::logger().off();

    if (webSpeedTest.endTest)
        alog::logger().on();

    writeToJsonMessage(webSpeedTest, answer);
    tcp::listener().send(answer);
}
