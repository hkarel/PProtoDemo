/**
  Пример сериализации агрегированных структур данных
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"

#include <QtCore>

namespace pproto {
namespace data {

struct Field1
{
    qint8   value1;
    quint32 value2;

    DECLARE_B_SERIALIZE_FUNC
};

struct Field2
{
    QString value3;
    QVector<qint32> value4;

    DECLARE_B_SERIALIZE_FUNC
};

struct Field3
{
    qint16 value5;
    qint32 value6;

    DECLARE_B_SERIALIZE_FUNC
};

struct StructData
{
    Field1 field1;
    Field2 field2;
    QList<Field3> field3;

    DECLARE_B_SERIALIZE_FUNC
};

bserial::RawVector Field1::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    B_SERIALIZE_RETURN
}

void Field1::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    B_DESERIALIZE_END
}

bserial::RawVector Field2::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value3;
    stream << value4;
    B_SERIALIZE_RETURN
}

void Field2::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value3;
    stream >> value4;
    B_DESERIALIZE_END
}

bserial::RawVector Field3::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value5;
    stream << value6;
    B_SERIALIZE_RETURN
}

void Field3::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value5;
    stream >> value6;
    B_DESERIALIZE_END
}

bserial::RawVector StructData::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << field1;
    stream << field2;
    stream << field3;
    B_SERIALIZE_RETURN
}

void StructData::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> field1;
    stream >> field2;
    stream >> field3;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto

namespace alog {

Line& operator<< (Line& line, const pproto::data::Field3& f3)
{
    line << log_format("{%?, %?}", f3.value5, f3.value6);
    return line;
}

} // namespace alog {

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 04'";
    QByteArray buff;

    StructData data;
    data.field1.value1 = 1;
    data.field1.value2 = 2;
    data.field2.value3 = "string data";
    data.field2.value4 = {1,2,3,4,5};
    data.field3 = {{1,1}, {2,2}, {3,3}};

    log_info << "---";
    log_info << "Source data";
    log_info << "Field1 Value1:   " << data.field1.value1;
    log_info << "Field1 Value2:   " << data.field1.value2;
    log_info << "Field2 Value3:   " << data.field2.value3;
    log_info << "Field2 Value4:   " << data.field2.value4;
    log_info << "Field3 Value5,6: " << data.field3;

    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << data;
    }

    StructData data2;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> data2;
    }
    log_info << "---";
    log_info << "Deserialized data";
    log_info << "Field1 Value1:   " << data2.field1.value1;
    log_info << "Field1 Value2:   " << data2.field1.value2;
    log_info << "Field2 Value3:   " << data2.field2.value3;
    log_info << "Field2 Value4:   " << data2.field2.value4;
    log_info << "Field3 Value5,6: " << data2.field3;

    alog::stop();
    return 0;
}
