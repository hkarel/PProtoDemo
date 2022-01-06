#include "main_window.h"

#include "shared/logger/logger.h"
#include "shared/logger/config.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include <QApplication>
#include <QNetworkProxy>

void stopProgram()
{
    log_info << "Stop 'Transport Demo 04 Client'";
    alog::stop();
}

int main(int argc, char *argv[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2);

    log_info << "Start 'Transport Demo 04 Client'";

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

    QApplication appl {argc, argv};

    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    MainWindow mw;
    if (!mw.init())
    {
        stopProgram();
        return 1;
    }
    mw.show();

    appl.exec();
    mw.deinit();

    stopProgram();
    return 0;
}
