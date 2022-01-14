#include "tdemo02_appl.h"

#include "shared/spin_locker.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/config/logger_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include "pproto/commands/base.h"
#include "pproto/commands/pool.h"

#define log_error_m   alog::logger().error   (alog_line_location, "Application")
#define log_warn_m    alog::logger().warn    (alog_line_location, "Application")
#define log_info_m    alog::logger().info    (alog_line_location, "Application")
#define log_verbose_m alog::logger().verbose (alog_line_location, "Application")
#define log_debug_m   alog::logger().debug   (alog_line_location, "Application")
#define log_debug2_m  alog::logger().debug2  (alog_line_location, "Application")

Application::Application(int& argc, char** argv)
    : QCoreApplication(argc, argv)
    , _clientSocket(new tcp::Socket)
{
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
}

bool Application::init()
{
    if (!tcp::listener().init({QHostAddress::AnyIPv4, 33021}))
        return false;

    return true;
}

void Application::deinit()
{
    _clientSocket->disconnect();
    tcp::listener().close();
}

void Application::clietnConnect()
{
    _clientSocket->init({QHostAddress::LocalHost, 33021});
    _clientSocket->connect();
}

void Application::clientMessage(const pproto::Message::Ptr& message)
{
    // Обрабатываем команду TDemo02_01
    if (message->command() == command::TDemo02_01)
    {
        if (message->type() != Message::Type::Answer)
        {
            // Обрабатываем ситуацию когда прислали не 'Answer'
            log_error_m << "Client: command TDemo02_01"
                        << ". Messages of type 'Answer' are allowed only";
            qApp->quit();
            return;
        }

        log_debug_m << "Client: received a answer for command TDemo02_01";

        // Отправляем команду TDemo02_02
        data::TDemo02_02 tdemo02_02;
        tdemo02_02.value1 = 7;
        tdemo02_02.value2 = "Request";

        Message::Ptr m = createMessage(tdemo02_02);
        _clientSocket->send(m);
    }

    // Обрабатываем команду TDemo02_02
    else if (message->command() == command::TDemo02_02)
    {
        data::TDemo02_02 tdemo02_02;
        readFromMessage(message, tdemo02_02);
        log_debug_m << log_format(
            "Client: received a answer for command TDemo02_02. Value: %?, %?",
            tdemo02_02.value1, tdemo02_02.value2);

        // Отправляем команду TDemo02_03
        data::TDemo02_03 tdemo02_03;
        tdemo02_03.value1 = 12;

        Message::Ptr m = createMessage(tdemo02_03);
        _clientSocket->send(m);
    }

    // Обрабатываем команду TDemo02_03
    else if (message->command() == command::TDemo02_03)
    {
        data::TDemo02_03A tdemo02_03A;
        readFromMessage(message, tdemo02_03A);
        log_debug_m << log_format(
            "Client: received a answer for command TDemo02_03. Value: %?, %?, %?",
            tdemo02_03A.value1, tdemo02_03A.value2, tdemo02_03A.value3);

        // Отправляем команду TDemo02_04
        Message::Ptr m = createMessage(command::TDemo02_04);
        _clientSocket->send(m);
    }

    // Обрабатываем команду TDemo02_04
    else if (message->command() == command::TDemo02_04)
    {
        log_debug_m << "Client: received a answer for command TDemo02_04";

        if (message->execStatus() == Message::ExecStatus::Error)
        {
            data::MessageError err;
            readFromMessage(message, err);
            log_error_m << err.description;
        }
        else if (message->execStatus() == Message::ExecStatus::Failed)
        {
            data::MessageFailed fail;
            readFromMessage(message, fail);
            log_error_m << fail.description;
        }
        else
            log_debug_m << "Client: TDemo02 success executed";

        //qApp->quit();
        _clientSocket->disconnect();
    }
}

void Application::serverMessage(const pproto::Message::Ptr& message)
{
    // Обрабатываем команду TDemo02_01
    if (message->command() == command::TDemo02_01)
    {
        log_debug_m << "Server: received command TDemo02_01";

        data::TDemo02_01 tdemo02_01;
        readFromMessage(message, tdemo02_01);
        log_debug_m << "Server: TDemo02_01 data: " << tdemo02_01.value1;

        // Отправляем ответ (только статус сообщения)
        Message::Ptr answer = message->cloneForAnswer();
        tcp::listener().send(answer);
    }

    // Обрабатываем команду TDemo02_02
    else if (message->command() == command::TDemo02_02)
    {
        log_debug_m << "Server: Received command TDemo02_02";

        data::TDemo02_02 tdemo02_02;
        readFromMessage(message, tdemo02_02);
        log_debug_m << log_format("Server: TDemo02_02 data: %?, %?",
                                  tdemo02_02.value1, tdemo02_02.value2);
        tdemo02_02.value1 = 14;
        tdemo02_02.value2 = "Response";

        // Отправляем ответ (данные и статус сообщения)
        Message::Ptr answer = message->cloneForAnswer();
        writeToMessage(tdemo02_02, answer);
        tcp::listener().send(answer);
    }

    // Обрабатываем команду TDemo02_03
    else if (message->command() == command::TDemo02_03)
    {
        log_debug_m << "Server: received command TDemo02_03";

        data::TDemo02_03 tdemo02_03;
        readFromMessage(message, tdemo02_03);
        log_debug_m << log_format("Server: TDemo02_03 data: %?", tdemo02_03.value1);

        data::TDemo02_03A tdemo02_03A; /*Answer*/
        tdemo02_03A.value1 = 22;
        tdemo02_03A.value2 = "Response";
        tdemo02_03A.value3 = QUuid::createUuid();

        // Отправляем ответ (данные и статус сообщения)
        Message::Ptr answer = message->cloneForAnswer();
        writeToMessage(tdemo02_03A, answer);
        tcp::listener().send(answer);
    }

    // Обрабатываем команду TDemo02_04
    else if (message->command() == command::TDemo02_04)
    {
        log_debug_m << "Server: received command TDemo02_04";

        // Отправляем ответ (эмулируем ошибку выполнения команды на сервере)
        Message::Ptr answer = message->cloneForAnswer();
        writeToMessage(error::tdemo02_04_bad, answer);
        tcp::listener().send(answer);
    }

}

void Application::clientSocketConnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Client: connection to server success. Socket: " << socketDescript;

    // Отправляем первую команду
    data::TDemo02_01 tdemo02_01;
    tdemo02_01.value1 = 5;

    Message::Ptr m = createMessage(tdemo02_01);
    _clientSocket->send(m);
}

void Application::clientSocketDisconnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Client: disconnected from server. Socket: " << socketDescript;
}

void Application::serverSocketConnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Server: client connection. Socket: " << socketDescript;

    //qApp->quit();
    alog::logger().flush();
    QTimer::singleShot(300, this, &QCoreApplication::quit);
}

void Application::serverSocketDisconnected(pproto::SocketDescriptor socketDescript)
{
    log_debug_m << "Server: client closed connection. Socket: " << socketDescript;
}

