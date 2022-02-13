/*****************************************************************************
  В модуле представлен список идентификаторов команд для коммуникации между
  клиентской и серверной частями приложения.
  В данном модуле представлен список команд персональный для этого приложения.

  Требование надежности коммуникаций: однажды назначенный идентификатор коман-
  ды не должен более меняться.
*****************************************************************************/

#pragma once

#include "error.h"
#include "shared/clife_base.h"
#include "shared/clife_ptr.h"
#include "shared/qt/quuidex.h"
#include "pproto/commands/base.h"

namespace pproto {
namespace command {

//----------------------------- Список команд --------------------------------

/**
  Команда для примера 'TDemo 01'
*/
extern const QUuidEx TDemo01;

/**
  Команды для примера 'TDemo 02'.
  TDemo02_01 отправляет данные на сервер, но не предполагает получения данных
             от сервера (только статус выполнения);
  TDemo02_02 отправляет данные на сервер и получает данные от сервера;
  TDemo02_03 отправляет на сервер одну структуру данных, а получает
             в ответе другую;
  TDemo02_04 отправляет на сервер только команду (без данных), в ответе
             приходит статус выполнения с ошибкой (эмуляция ошибки на сервере).
*/
extern const QUuidEx TDemo02_01;
extern const QUuidEx TDemo02_02;
extern const QUuidEx TDemo02_03;
extern const QUuidEx TDemo02_04;

/**
  Команды для примера 'TDemo 03'
*/
extern const QUuidEx TDemo03_01;
extern const QUuidEx TDemo03_02;

/**
  Команды для примера 'TDemo 04'
*/
extern const QUuidEx TDemo04_01; // Команда возвращает сообщение-приветствие "Hello PProto-Python!"
extern const QUuidEx TDemo04_02; // Комбинированный тест: на скорость, и на загрузку TCP буфера
                                 // короткими сообщениями

/**
  Команды для примера 'TDemo 05'
*/
extern const QUuidEx TDemo05_GetData;    // Отправляет запрос на сервер для получения данных
extern const QUuidEx TDemo05_SendChunk;  // Отправляет чанк данных клиенту
extern const QUuidEx TDemo05_SendFinish; // Сигнализирует об окончании пересылки данных
extern const QUuidEx TDemo05_SendError;  // Сигнализирует о неудаче при пересылке данных

/**
  Команда возвращает сообщение-приветствие "Hello PProto-Web!"
*/
extern const QUuidEx WebPProtoHello;

/**
  Команда выполняет комбинированный тест на скорость, и на загрузку TCP буфера
  короткими сообщениями
*/
extern const QUuidEx WebSpeedTest;


} // namespace command

//---------------- Структуры данных используемые в сообщениях ----------------

namespace data {

struct TDemo01 : Data<&command::TDemo01,
                       Message::Type::Command,
                       Message::Type::Answer>
{
    qint32 value1 = {0};
    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo02_01 : Data<&command::TDemo02_01,
                          Message::Type::Command>
{
    qint32 value1 = {0};
    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo02_02 : Data<&command::TDemo02_02,
                          Message::Type::Command,
                          Message::Type::Answer>
{
    qint32  value1 = {0};
    QString value2;
    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo02_03 : Data<&command::TDemo02_03,
                          Message::Type::Command>
{
    qint32 value1 = {0};
    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo02_03A /*Answer*/ : Data<&command::TDemo02_03,
                                      Message::Type::Answer>
{
    qint32  value1 = {0};
    QString value2;
    QUuidEx value3;
    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo03_01 : Data<&command::TDemo03_01,
                          Message::Type::Command,
                          Message::Type::Answer>
{
    qint32  value1 = {0};
    QString value2;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
    J_SERIALIZE_END
};

struct TDemo03_02 : Data<&command::TDemo03_02,
                          Message::Type::Command,
                          Message::Type::Answer>
{
    qint32  value1 = {0};
    QUuidEx value2;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
    J_SERIALIZE_END
};

struct TDemo04_01 : Data<&command::TDemo04_01,
                          Message::Type::Command,
                          Message::Type::Answer>
{
    qint32  value1 = {0};
    QString value2;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
    J_SERIALIZE_END
};

struct TDemo04_02 : Data<&command::TDemo04_02,
                          Message::Type::Command,
                          Message::Type::Answer>
{
    QUuidEx uuid;
    bool beginTest = {false};
    bool endTest = {false};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( uuid )
        J_SERIALIZE_OPT ( beginTest )
        J_SERIALIZE_OPT ( endTest )
    J_SERIALIZE_END
};

struct TDemo05_GetData : Data<&command::TDemo05_GetData,
                               Message::Type::Command,
                               Message::Type::Answer>
{
    // Количество чанков (возвращается в ответе)
    qint32 chunkCount = {0};

    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo05_SendChunk : Data<&command::TDemo05_SendChunk,
                                 Message::Type::Command>
{
    // Индекс чанка
    qint32 index = {0};

    // Данные чанка
    quint32 data = {0};

    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo05_SendFinish : Data<&command::TDemo05_SendFinish,
                                  Message::Type::Command>
{
    // Контрольная сумма
    quint32 crc = {0};

    DECLARE_B_SERIALIZE_FUNC
};

struct TDemo05_SendError : Data<&command::TDemo05_SendError,
                                 Message::Type::Command>
{
    ErrorInfo errorInfo;
    DECLARE_B_SERIALIZE_FUNC
};

struct WebPProtoHello : Data<&command::WebPProtoHello,
                              Message::Type::Answer>
{
    QString value;
    J_SERIALIZE_ONE( value )
};

struct WebSpeedTest : Data<&command::WebSpeedTest,
                            Message::Type::Command,
                            Message::Type::Answer>
{
    QUuidEx uuid;
    bool beginTest = {false};
    bool endTest = {false};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( uuid      )
        J_SERIALIZE_OPT ( beginTest )
        J_SERIALIZE_OPT ( endTest   )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto
