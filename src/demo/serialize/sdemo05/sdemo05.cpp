/**
  Пример сериализации унаследованных структур
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"

#include <QtCore>

namespace pproto {
namespace data {

struct BaseStruct
{
    qint8   value1 = {0};
    quint32 value2 = {0};
    quint64 value3 = {0};

    DECLARE_B_SERIALIZE_FUNC
};

struct BaseDeserializeDummy : BaseStruct
{
    DECLARE_B_SERIALIZE_FUNC
};

struct DerivedStruct : BaseStruct
{
    QString value4;
    QUuidEx value5;

    DECLARE_B_SERIALIZE_FUNC
};

bserial::RawVector BaseStruct::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    stream << value3;
    B_SERIALIZE_RETURN
}

void BaseStruct::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    stream >> value3;
    B_DESERIALIZE_END
}

//bserial::RawVector BaseDeserializeDummy::toRaw() const
//{
//    B_SERIALIZE_V1(stream)
//    stream << B_BASE_CLASS(BaseStruct);
//    B_SERIALIZE_RETURN
//}

void BaseDeserializeDummy::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> B_BASE_CLASS(BaseStruct);
    B_DESERIALIZE_END
}

bserial::RawVector DerivedStruct::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << B_BASE_CLASS(BaseStruct);
    stream << value4;
    stream << value5;
    B_SERIALIZE_RETURN
}

void DerivedStruct::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> B_BASE_CLASS(BaseStruct);
    stream >> value4;
    stream >> value5;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 05'";
    QByteArray buff;

    DerivedStruct data;
    data.value1 = 3;
    data.value2 = 4;
    data.value3 = 5;
    data.value4 = "string data";
    data.value5 = QUuid::createUuid();

    log_info << "---";
    log_info << "Source data";
    log_info << "Value1: " << data.value1;
    log_info << "Value2: " << data.value2;
    log_info << "Value3: " << data.value3;
    log_info << "Value4: " << data.value4;
    log_info << "Value5: " << data.value5;

    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << data;
    }

    BaseStruct data2;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);

        // Для извлечения из потока только базового класса потребуется
        // вспомогательная структура с фиктивной функцией десериализации
        stream >> *((BaseDeserializeDummy*)&data2);
    }
    log_info << "---";
    log_info << "Deserialized base data";
    log_info << "Value1: " << data2.value1;
    log_info << "Value2: " << data2.value2;
    log_info << "Value3: " << data2.value3;

    DerivedStruct data3;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> data3;
    }
    log_info << "---";
    log_info << "Deserialized derived data";
    log_info << "Value1: " << data3.value1;
    log_info << "Value2: " << data3.value2;
    log_info << "Value3: " << data3.value3;
    log_info << "Value4: " << data3.value4;
    log_info << "Value5: " << data3.value5;

    alog::stop();
    return 0;
}
