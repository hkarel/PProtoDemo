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
#include "pproto/commands_base.h"

namespace pproto {
namespace command {

//----------------------------- Список команд --------------------------------

/**
  Команда для примера 'TDemo 01'
*/
extern const QUuidEx TDemo01;

/**
  Команда возвращает сообщение-приветствие "PProto hello!"
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
        J_SERIALIZE_ITEM( beginTest )
        J_SERIALIZE_ITEM( endTest   )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto


