/**
  Пример обмена сообщениями между C++ сервером и Python-клиентом
*/

#include "tdemo03_serv_appl.h"

#include "shared/logger/logger.h"
#include "shared/logger/config.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/commands/base.h"
#include "pproto/commands/pool.h"
#include "pproto/transport/tcp.h"

#include <QNetworkProxy>

#ifdef MINGW
#include <windows.h>
#else
#include <csignal>
#endif
#include <unistd.h>

using namespace pproto;
using namespace pproto::transport;

/**
  Используется для уведомления основного потока о завершении работы программы
*/
#ifdef MINGW
BOOL WINAPI stopProgramHandler(DWORD sig)
{
    if ((sig == CTRL_C_EVENT) || (sig == CTRL_BREAK_EVENT))
    {
        const char* sigName = (sig == CTRL_C_EVENT) ? "CTRL_C_EVENT" : "CTRL_BREAK_EVENT";
        log_verbose << "Signal " << sigName << " is received. Program will be stopped";
        Application::stop();
    }
    else
        log_verbose << "Signal " << sig << " is received";
    return TRUE;
}
#else
void stopProgramHandler(int sig)
{
    if ((sig == SIGTERM) || (sig == SIGINT))
    {
        const char* sigName = (sig == SIGTERM) ? "SIGTERM" : "SIGINT";
        log_verbose << "Signal " << sigName << " is received. Program will be stopped";
        Application::stop();
    }
    else
        log_verbose << "Signal " << sig << " is received";
}
#endif // #ifdef MINGW

void stopProgram()
{
    tcp::listener().close();

    log_info << "Stop 'Transport Demo 03 Server'";
    alog::stop();
}

int main(int argc, char* argv[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2);

    // Именуем листенер (опционально)
    tcp::listener().setName("python-client");

#ifdef MINGW
    if (!SetConsoleCtrlHandler(stopProgramHandler, TRUE))
    {
        log_error << "Could not set control handler";
        alog::stop();
        return 1;
    }
#else
    signal(SIGTERM, &stopProgramHandler);
    signal(SIGINT,  &stopProgramHandler);
#endif

    log_info << "Start 'Transport Demo 03 Server'";

    alog::printSaversInfo();

    if (!pproto::command::checkUnique())
    {
        stopProgram();
        return 1;
    }

    if (!pproto::error::checkUnique())
    {
        stopProgram();
        return 1;
    }

    Application appl {argc, argv};

    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    if (!appl.init())
    {
        stopProgram();
        return 1;
    }

    if (!tcp::listener().init({QHostAddress::AnyIPv4, 33022}))
    {
        stopProgram();
        return 1;
    }
    tcp::listener().setMessageWebFlags(true);
    tcp::listener().setCheckProtocolCompatibility(false);

    chk_connect_q(&tcp::listener(), &tcp::Listener::message,
                  &appl, &Application::message)

    chk_connect_q(&tcp::listener(), &tcp::Listener::socketConnected,
                  &appl, &Application::socketConnected)

    chk_connect_q(&tcp::listener(), &tcp::Listener::socketDisconnected,
                  &appl, &Application::socketDisconnected)

    appl.exec();
    appl.deinit();

    stopProgram();
    return 0;
}
