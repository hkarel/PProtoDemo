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

    DECLARE_B_SERIALIZE_FUNC
};

struct BaseStruct2
{
    qint8  value3 = {0};
    qint64 value4 = {0};

    DECLARE_B_SERIALIZE_FUNC
};

struct BaseDeserializeDummy : BaseStruct
{
    DECLARE_B_SERIALIZE_FUNC
};

struct DerivedStruct : BaseStruct, BaseStruct2
{
    QString value5;
    QUuidEx value6;

    DECLARE_B_SERIALIZE_FUNC
};

bserial::RawVector BaseStruct::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    B_SERIALIZE_RETURN
}

void BaseStruct::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    B_DESERIALIZE_END
}

bserial::RawVector BaseStruct2::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value3;
    stream << value4;
    B_SERIALIZE_RETURN
}

void BaseStruct2::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value3;
    stream >> value4;
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
    stream << B_BASE_CLASS(BaseStruct2);
    stream << value5;
    stream << value6;
    B_SERIALIZE_RETURN
}

void DerivedStruct::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> B_BASE_CLASS(BaseStruct);
    stream >> B_BASE_CLASS(BaseStruct2);
    stream >> value5;
    stream >> value6;
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
    data.value4 = 6;
    data.value5 = "string data";
    data.value6 = QUuid::createUuid();

    log_info << "---";
    log_info << "Source data";
    log_info << "Value1: " << data.value1;
    log_info << "Value2: " << data.value2;
    log_info << "Value3: " << data.value3;
    log_info << "Value4: " << data.value4;
    log_info << "Value5: " << data.value5;
    log_info << "Value6: " << data.value6;

    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << data;
    }

    BaseStruct baseData;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);

        // Для извлечения из потока только базового класса потребуется
        // вспомогательная структура с фиктивной функцией десериализации
        stream >> *((BaseDeserializeDummy*)&baseData);
    }
    log_info << "---";
    log_info << "Deserialized base data";
    log_info << "Value1: " << baseData.value1;
    log_info << "Value2: " << baseData.value2;

    DerivedStruct dataDerived;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> dataDerived;
    }
    log_info << "---";
    log_info << "Deserialized derived data";
    log_info << "Value1: " << dataDerived.value1;
    log_info << "Value2: " << dataDerived.value2;
    log_info << "Value3: " << dataDerived.value3;
    log_info << "Value4: " << dataDerived.value4;
    log_info << "Value5: " << dataDerived.value5;
    log_info << "Value6: " << dataDerived.value6;

    alog::stop();
    return 0;
}
