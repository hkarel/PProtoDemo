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
  Команда возвращает сообщение-приветствие "PProto hello!"
*/
extern const QUuidEx WebPProtoHello;

} // namespace command

//---------------- Структуры данных используемые в сообщениях ----------------

namespace data {

struct WebPProtoHello : Data<&command::WebPProtoHello,
                              Message::Type::Answer>
{
    QString value;
    J_SERIALIZE_ONE( value )
};

} // namespace data
} // namespace pproto


