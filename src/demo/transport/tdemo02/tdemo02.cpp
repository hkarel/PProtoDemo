/**
  Пример обмена сообщениями между клиентом и сервером.
  В примере используются следующие команды:
    - TDemo02_01 отправляет данные на сервер, но не предполагает получения данных
                 от сервера (только статус выполнения);
    - TDemo02_02 отправляет данные на сервер и получает данные от сервера;
    - TDemo02_03 отправляет на сервер одну структуру данных, а получает
                 в ответе другую;
    - TDemo02_04 отправляет на сервер только команду (без данных), в ответе
                 приходит статус выполнения с ошибкой (эмуляция ошибки на сервере).
    - TDemo02_05 отправляет на сервер только команду (без данных), в ответе
                 приходит статус выполнения с расширенной ошибкой.
    - TDemo02_06 отправляет на сервер только команду (без данных), в ответе
                 приходит статус выполнения с расширенной информацией о неудачном
                 выполнении.
*/

#include "tdemo02_appl.h"

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

    log_info << "Start 'Transport Demo 02'";

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

    log_info << "Stop 'Transport Demo 02'";
    alog::stop();
    return 0;
}
