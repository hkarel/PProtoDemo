/**
  Сериализация разряженного списка lst::List
*/

#include "shared/list.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"

#include "pproto/serialize/json.h"

#include <QtCore>

namespace pproto {
namespace data {

struct StructData
{
    qint8  value1 = {0};
    qint16 value2 = {0};

    DECLARE_B_SERIALIZE_FUNC

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
    J_SERIALIZE_END
};

struct StructList
{
    lst::List<StructData> values;

    DECLARE_B_SERIALIZE_FUNC

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( values )
    J_SERIALIZE_END
};

void StructData::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << value1;
    stream << value2;
    B_SERIALIZE_RETURN
}

void StructData::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> value1;
    stream >> value2;
    B_DESERIALIZE_END
}

void StructList::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << values;
    B_SERIALIZE_RETURN
}

void StructList::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> values;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 13'";
    QByteArray buff;

    StructList list;

    for (int i = 0; i < 5; ++i)
    {
        if (i == 2 || i == 4)
        {
            list.values.add(nullptr);
        }
        else
        {
            StructData* sd = list.values.add();
            sd->value1 = i;
            sd->value2 = i;
        }
    }

    { //Block for QDataStream
        QDataStream stream1 {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream1);
        stream1 << list;
    }

    StructList listBin;

    { //Block for QDataStream
        QDataStream stream2 {buff};
        STREAM_INIT(stream2);
        stream2 >> listBin;
    }

    log_info << "QBinary deserialization:";

    for (int i = 0; i < 5; ++i)
    {
        StructData* sd = listBin.values.item(i);
        if (sd)
            log_info << log_format("i%?  Value1: %?, Value2: %?", i,
                                   sd->value1, sd->value2);
        else
            log_info << log_format("i%?  null", i);
    }

    buff = list.toJson();

    log_info << "---";
    log_info << "JSON representation:";
    log_info << buff;
    log_info << "---";

    StructList listJson;
    listJson.fromJson(buff);

    log_info << "Json deserialization:";

    for (int i = 0; i < 5; ++i)
    {
        StructData* sd = listJson.values.item(i);
        if (sd)
            log_info << log_format("i%?  Value1: %?, Value2: %?", i,
                                   sd->value1, sd->value2);
        else
            log_info << log_format("i%?  null", i);
    }

    alog::stop();
    return 0;
}
