#include "wdemo01_serv_appl.h"
//#include "functions.h"

#include "shared/defmac.h"
#include "shared/logger/logger.h"
#include "shared/logger/config.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/config/logger_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include "pproto/commands_base.h"
#include "pproto/commands_pool.h"
#include "pproto/transport/tcp.h"

#include <QNetworkProxy>

#ifdef MINGW
#include <windows.h>
#else
#include <csignal>
#endif
#include <unistd.h>

#define APPLICATION_NAME "WDemo01 Server"

using namespace std;
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
    #define STOP_THREAD(THREAD_FUNC, NAME, TIMEOUT) \
        if (!THREAD_FUNC.stop(TIMEOUT * 1000)) { \
            log_info << "Thread '" NAME "': Timeout expired, thread will be terminated"; \
            THREAD_FUNC.terminate(); \
        }

//    STOP_THREAD(udp::socket(),          "TransportUDP", 15)
//    STOP_THREAD(task::loader(),         "TaskLoader",   15)
//    STOP_THREAD(task::trainManager(),   "TrainManager", 15)

    #undef STOP_THREAD

    tcp::listener().close();

    log_info << log_format("'%?' service is stopped", APPLICATION_NAME);
    alog::stop();
}

int main(int argc, char *argv[])
{
    // Устанавливаем в качестве разделителя целой и дробной части символ '.',
    // если этого не сделать - функции преобразования строк в числа (std::atof)
    // буду неправильно работать.
    qputenv("LC_NUMERIC", "C");

    // Именуем листенеры (опционально)
    tcp::listener().setName("web-client");

    int ret = 0;
    try
    {
        alog::logger().start();

#ifdef NDEBUG
        alog::logger().addSaverStdOut(alog::Level::Info, true);
#else
        alog::logger().addSaverStdOut(alog::Level::Debug2);
#endif
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
        QString configFile = config::qdir() + "/wdemo01_serv.conf";
        config::dirExpansion(configFile);

        if (QFile::exists(configFile))
        {
            if (!config::base().readFile(configFile.toStdString()))
            {
                alog::stop();
                return 1;
            }
        }
        else
        {
            QFile file;
            QByteArray conf;

            file.setFileName("://wdemo01_serv.conf");
            file.open(QIODevice::ReadOnly);
            conf = file.readAll();

            if (!config::base().readString(conf.toStdString()))
            {
                alog::stop();
                return 1;
            }
        }
        config::base().setReadOnly(true);
        config::base().setSaveDisabled(true);

        QString configFileS;
#ifdef MINGW
        config::base().getValue("state.file_win", configFileS);
#else
        config::base().getValue("state.file", configFileS);
#endif
        config::dirExpansion(configFileS);
        config::state().readFile(configFileS.toStdString());

        // Создаем дефолтный сэйвер для логгера
        if (!alog::configDefaultSaver())
        {
            alog::stop();
            return 1;
        }

        log_info << log_format(
            "'%?' service is running (version: %?; gitrev: %?)",
            APPLICATION_NAME, productVersion().toString(), GIT_REVISION);
        alog::logger().flush();

        //alog::logger().removeSaverStdOut();
        //alog::logger().removeSaverStdErr();

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

        // Устанавливаем текущую директорию. Эта конструкция работает только
        // когда создан экземпляр QCoreApplication.
        if (QDir::setCurrent(QCoreApplication::applicationDirPath()))
        {
            log_debug << "Set work directory: " << QCoreApplication::applicationDirPath();
        }
        else
        {
            log_error << "Failed set work directory";
            stopProgram();
            return 1;
        }

        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

        if (!appl.init())
        {
            stopProgram();
            return 1;
        }

        // Конфигурирование client-подключений
        QHostAddress hostAddress = QHostAddress::AnyIPv4;
        config::readHostAddress("listener_client.address", hostAddress);

        int port = 28052;
        config::base().getValue("listener_client.port", port);

        if (!tcp::listener().init({hostAddress, port}))
        {
            stopProgram();
            return 1;
        }
        tcp::listener().setMessageWebFlags(true);
        tcp::listener().setCheckProtocolCompatibility(false);

        chk_connect_q(&tcp::listener(), &tcp::Listener::message,
                      &appl, &Application::message)

        chk_connect_q(&tcp::listener(), &tcp::Listener::socketConnected,
                      &appl, &Application::clientSocketConnected)

        chk_connect_q(&tcp::listener(), &tcp::Listener::socketDisconnected,
                      &appl, &Application::clientSocketDisconnected)

        if (Application::isStopped())
        {
            stopProgram();
            return 0;
        }

        //config::changeChecker().start();

        ret = appl.exec();
        appl.deinit();

        //config::changeChecker().stop();

        if (config::state().changed())
            config::state().saveFile();

        stopProgram();
        return ret;
    }
    catch (std::exception& e)
    {
        log_error << "Failed initialization. Detail: " << e.what();
        ret = 1;
    }
    catch (...)
    {
        log_error << "Failed initialization. Unknown error";
        ret = 1;
    }

    stopProgram();
    return ret;
}

