/**
  Базовый пример создания сообщения
*/

#include "commands/commands.h"

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/functions.h"

#include <QtCore>

using namespace pproto;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Transport Demo 01'";

    // ------------
    // Приложение 1
    // ------------

    // Создаем сообщение на основе команды с пустым контентом
    Message::Ptr message = createMessage(command::TDemo01);
    message->clearContent();

    // Пытаемся прочитать из сообщения пустой контент
    data::TDemo01 tdemo01;
    SResult res = readFromMessage(message, tdemo01);

    log_info << log_format("Read from message status: %?, Error descript: %?",
                           bool(res), res.description());

    log_info << "value1: " << tdemo01.value1;

    tdemo01.value1 = 5;

    // Записываем структуру TDemo01 в сообщение
    writeToMessage(tdemo01, message);

    // Передаем сообщение в другое приложение (или в другой поток)
    // ...

    // ------------
    // Приложение 2
    // ------------

    // Читаем контент (TDemo01) из сообщения
    data::TDemo01 tdemo01_2;
    res = readFromMessage(message, tdemo01_2);

    log_info << log_format("Read from message status: %?, Error descript: %?",
                           bool(res), res.description());

    log_info << "value1 (2): " << tdemo01_2.value1;

    tdemo01_2.value1 = 10;

    // Содаем новое сообщение (ответ) на основе структуры TDemo01
    Message::Ptr answer = createMessage(tdemo01_2);

    // Отправляем ответ в первое приложение
    // ...

    // Читаем из сообщения ответ
    data::TDemo01 tdemo01_A;
    res = readFromMessage(answer, tdemo01_A);

    log_info << log_format("Read from message status: %?, Error descript: %?",
                           bool(res), res.description());

    log_info << "value1 (A): " << tdemo01_A.value1;

    alog::stop();
    return 0;
}
