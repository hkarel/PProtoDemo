/**
  Пример обмена сообщениями между клиентом и сервером в синхронном режиме.
  Клиент запрашивает данные у сервера при помощи команды TDemo05_GetData.
  Сервер отправляет данные на клиент чанками используя команду TDemo05_SendChunk,
  при этом сервер ждет подтверждения от клиента, что очередная SendChunk-команда
  получена. После того как все чанки переданы сервер отправляет завершающую
  команду TDemo05_SendFinish с контрольной суммой. Если сервер не может передать
  данные он отправляет сообщение TDemo05_SendError с описанием ошибки
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/commands/pool.h"
#include "pproto/transport/tcp.h"
#include "pproto/transport/udp.h"

#include "commands/commands.h"

#include <QNetworkProxy>
#include <unistd.h>

using namespace pproto;
using namespace pproto::transport;

udp::Socket& socketClient()
{
    return ::safe_singleton<udp::Socket, 1>();
}

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2);

    log_info << "Start 'Transport Demo 06'";

    if (!pproto::command::checkUnique())
    {
        alog::stop();
        return 1;
    }

    if (!pproto::error::checkUnique())
    {
        alog::stop();
        return 1;
    }

    //Application appl {argc, argv};

    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    // Конфигурирование udp-сокета
    if (!udp::socket().init({QHostAddress::AnyIPv4, 33026 /*port*/}))
    {
        alog::stop();
        return 1;
    }
    udp::socket().start();
    udp::socket().waitBinding(3);
    if (!udp::socket().isBound())
    {
        alog::stop();
        return 1;
    }
    QObject::connect(&udp::socket(), &udp::Socket::message,
                     [](const pproto::Message::Ptr& message)
    {
        if (message->command() == command::TDemo06
            && message->type() == Message::Type::Command)
        {
            log_info << "Received UDP-command TDemo06";
            Message::Ptr answer = message->cloneForAnswer();

            data::TDemo06 tdemo06;
            tdemo06.info = "TDemo 06 example";
            tdemo06.hostPoint = {QHostAddress::LocalHost, 33026 /*port*/};

            log_info << "Send UDP-answer TDemo06";
            writeToMessage(tdemo06, answer);
            udp::socket().send(answer);
        }
    });

    // Конфигурирование клиентского udp-сокета
    if (!socketClient().init({QHostAddress::AnyIPv4, 33026+1 /*port*/}))
    {
        alog::stop();
        return 1;
    }
    socketClient().start();
    socketClient().waitBinding(3);
    if (!socketClient().isBound())
    {
        alog::stop();
        return 1;
    }

    volatile bool exitFlag = false;

    QObject::connect(&socketClient(), &udp::Socket::message,
                     [&exitFlag](const pproto::Message::Ptr& message)
    {
        if (message->command() == command::TDemo06
            && message->type() == Message::Type::Answer)
        {
            log_info << "Received UDP-answer TDemo06";

            data::TDemo06 tdemo06;
            readFromMessage(message, tdemo06);
            log_info << "TDemo06 info: " << tdemo06.info
                     << "; HostPoint: "  << tdemo06.hostPoint;
        }
        exitFlag = true;
    });

    // Отправка сообщения на localhost
    log_info << "Send UDP-command TDemo06";
    Message::Ptr message = createMessage(command::TDemo06);
    message->appendDestinationPoint({QHostAddress::LocalHost, 33026});
    socketClient().send(message);

    while (!exitFlag)
        usleep(100*1000);

    socketClient().stop();
    udp::socket().stop();

    log_info << "Stop 'Transport Demo 06'";
    alog::stop();
    return 0;
}
