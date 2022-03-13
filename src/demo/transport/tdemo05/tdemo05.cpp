/**
  Пример обмена сообщениями между клиентом и сервером в синхронном режиме.
  Для наглядности клиент и сервер расположены в одном  программном модуле,
  в классе Application. Клиент запрашивает данные  у сервера  при  помощи
  команды TDemo05_GetData.  Сервер  отправляет  данные  на клиент чанками
  используя команду TDemo05_SendChunk, при этом сервер ждет подтверждения
  от клиента, что очередная SendChunk-команда получена. После того как все
  чанки переданы сервер отправляет завершающую команду TDemo05_SendFinish
  с контрольной суммой. Если сервер не может передать данные он отправляет
  сообщение TDemo05_SendError с описанием ошибки
*/

#include "tdemo05_appl.h"

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/commands/pool.h"
#include "pproto/transport/tcp.h"

#include <QNetworkProxy>

using namespace pproto;
using namespace pproto::transport;

int main(int argc, char* argv[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2);

    log_info << "Start 'Transport Demo 05'";

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

    Application appl {argc, argv};

    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    appl.init();
    QMetaObject::invokeMethod(&appl, "clietnConnect", Qt::QueuedConnection);

    appl.exec();
    appl.deinit();

    log_info << "Stop 'Transport Demo 05'";
    alog::stop();
    return 0;
}
