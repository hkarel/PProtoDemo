#include "main_window.h"

#include "shared/container_ptr.h"
#include "shared/utils.h"
#include "shared/steady_timer.h"
#include "shared/logger/logger.h"
#include "shared/logger/config.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/config/logger_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include <QApplication>
#include <stdlib.h>
#include <unistd.h>

#define APPLICATION_NAME "WDemo01 Client"

using namespace std;

void stopProgram()
{
    log_info << log_format("'%?' is stopped", APPLICATION_NAME);
    alog::stop();
}

int main(int argc, char *argv[])
{
    // Устанавливаем в качестве разделителя целой и дробной части символ '.',
    // если этого не сделать - функции преобразования строк в числа (std::atof)
    // буду неправильно работать.
    qputenv("LC_NUMERIC", "C");

    int ret = 0;
    try
    {
        alog::logger().start();

#ifdef NDEBUG
        alog::logger().addSaverStdOut(alog::Level::Info, true);
#else
        alog::logger().addSaverStdOut(alog::Level::Debug2);
#endif
        QString configFile = config::qdir() + "/wdemo01_client.conf";
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

            file.setFileName("://wdemo01_client.conf");
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
            "'%?' is running (version: %?; gitrev: %?)",
            APPLICATION_NAME, productVersion().toString(), GIT_REVISION);
        alog::logger().flush();

        //alog::logger().removeSaverStdOut();
        //alog::logger().removeSaverStdErr();

        alog::printSaversInfo();

        QApplication appl {argc, argv};

        appl.setApplicationDisplayName(APPLICATION_NAME);
        appl.setApplicationVersion(VERSION_PROJECT);

        MainWindow mw;
        if (!mw.init())
        {
            stopProgram();
            return 1;
        }
        mw.show();

        ret = appl.exec();
        mw.deinit();

        if (config::state().changed())
            config::state().saveFile();
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
